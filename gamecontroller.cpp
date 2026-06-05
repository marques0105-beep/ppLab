#include "gamecontroller.h"
#include "gameanalytics.h"
#include "persistence.h"
#include <QtConcurrent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

// Construtor – inicializa o tabuleiro, variáveis de estado, timers e liga sinais
GameController::GameController(QObject *parent)
    : QObject(parent), m_board(8, 8), m_moveCount(0),
    m_currentLevel(1), m_gameState("PLAYING"), m_score(0),
    m_dangerLevel("ESTÁVEL 🟢"), m_inMenu(true), m_initialPassengersCount(0),
    m_elapsedSeconds(0), m_currentParkedBusIndex(-1), m_stepBusIndex(-1),
    m_stepTargetRow(-1), m_stepTargetCol(-1), m_stepDeltaRow(0),
    m_stepDeltaCol(0), m_stepExitedBoard(false), m_stepFreeSlot(-1)
{

    // Timer do cronómetro – incrementa os segundos decorridos e notifica a UI
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_elapsedSeconds++;
        emit elapsedSecondsChanged();
    });

    // Estes timers NÃO são single-shot; serão parados manualmente quando necessário
    m_moveStepTimer.setSingleShot(false);
    m_boardingTimer.setSingleShot(false);
}

// Destrutor – garante que todos os timers são parados para evitar execução após destruição
GameController::~GameController() {
    m_timer.stop();
    m_boardingTimer.stop();
    m_moveStepTimer.stop();
}

// Converte a lista de passageiros (C++) num QVariantList utilizável pelo QML
QVariantList GameController::getPassengerQueueForDisplay() const {
    QVariantList list;
    for (const auto& passenger : m_passengerQueue)
        list.append(passenger.toVariantMap());
    return list;
}

// Converte a lista de autocarros no tabuleiro para QVariantList (cada autocarro é um mapa)
QVariantList GameController::getBusesForDisplay() const {
    QVariantList list;
    const auto& buses = m_board.getBuses();
    for (const auto& bus : buses) {
        QVariantMap map;
        map["color"]    = bus.getColor();
        map["capacity"] = bus.getCapacity();
        map["currentPassengers"] = bus.getCurrentPassengers();

        // Direção convertida para string de um caractere: l, r, u, d
        QString dStr = "r";
        if      (bus.getDirection() == Direction::Left)  dStr = "l";
        else if (bus.getDirection() == Direction::Up)    dStr = "u";
        else if (bus.getDirection() == Direction::Down)  dStr = "d";
        map["direction"] = dStr;
        map["row"] = bus.getRow();
        map["col"] = bus.getCol();
        list.append(map);
    }
    return list;
}

// Converte a lista de autocarros estacionados (nas plataformas) para QVariantList
QVariantList GameController::getParkedBusesForDisplay() const {
    QVariantList list;
    for (const auto& parked : m_parkedBuses) {
        QVariantMap map;
        map["color"]             = parked.bus.getColor();
        map["capacity"]          = parked.bus.getCapacity();
        map["currentPassengers"] = parked.bus.getCurrentPassengers();
        map["slotIndex"]         = parked.slotIndex;
        list.append(map);
    }
    return list;
}

// Movimento do autocarro 
void GameController::handleBusClick(int busIndex) {
    // Só permite movimento se o jogo estiver activo e não houver outro movimento em curso
    if (m_gameState != "PLAYING") return;
    if (m_moveStepTimer.isActive()) return; // já está a mover outro autocarro

    auto& buses = m_board.getBusesMutable();
    if (busIndex < 0 || busIndex >= static_cast<int>(buses.size())) return;

    Bus& bus = buses[busIndex];
    // -10 é a posição especial que indica que o autocarro já saiu do tabuleiro
    if (bus.getRow() == -10 || bus.getCol() == -10) return;

    int currentR = bus.getRow();
    int currentC = bus.getCol();
    int len      = bus.getCapacity() / 2;

    int finalR = currentR, finalC = currentC;
    bool pathBlocked = false, exitedBoard = false;
    int freeSlot = -1;

    // Calcula destino final (mesmo algoritmo de antes)
    while (true) {
        if (bus.getDirection() == Direction::Right) {
            if (finalC + len >= m_board.getCols()) { exitedBoard = true; break; }
            if (m_board.isOccupied(finalR, finalC + len, busIndex)) { pathBlocked = true; break; }
            finalC++;
        }
        else if (bus.getDirection() == Direction::Left) {
            if (finalC - 1 < 0) { exitedBoard = true; break; }
            if (m_board.isOccupied(finalR, finalC - 1, busIndex)) { pathBlocked = true; break; }
            finalC--;
        }
        else if (bus.getDirection() == Direction::Down) {
            if (finalR + len >= m_board.getRows()) { exitedBoard = true; break; }
            if (m_board.isOccupied(finalR + len, finalC, busIndex)) { pathBlocked = true; break; }
            finalR++;
        }
        else if (bus.getDirection() == Direction::Up) {
            if (finalR - 1 < 0) { exitedBoard = true; break; }
            if (m_board.isOccupied(finalR - 1, finalC, busIndex)) { pathBlocked = true; break; }
            finalR--;
        }
    }

    // Se não houve qualquer deslocamento e o caminho estava bloqueado, reporta erro
    if (finalR == currentR && finalC == currentC && pathBlocked) {
        emit showNotification("⚠️ Caminho Bloqueado!");
        return;
    }

    if (exitedBoard) {
        if (!m_board.hasFreeSlot()) {
            emit showNotification("⚠️ Plataformas cheias!");
            return;
        }
        freeSlot = m_board.getNextFreeSlotIndex();
        // Guarda informação para quando o autocarro sair do tabuleiro
        m_stepExitedBoard = true;
        m_stepFreeSlot = freeSlot;
        
    } else {
        m_stepExitedBoard = false;
        m_stepFreeSlot = -1;
    }

    // Configura movimento passo-a-passo
    m_stepBusIndex = busIndex;
    m_stepTargetRow = finalR;
    m_stepTargetCol = finalC;
    m_stepDeltaRow = (finalR > currentR) ? 1 : (finalR < currentR) ? -1 : 0;
    m_stepDeltaCol = (finalC > currentC) ? 1 : (finalC < currentC) ? -1 : 0;

    // Para o caso de já estar no destino (não devia acontecer)
    if (m_stepDeltaRow == 0 && m_stepDeltaCol == 0) {
        if (m_stepExitedBoard) {
            // Estacionamento directo
            Bus& b = buses[busIndex];
            m_board.occupySlot(m_stepFreeSlot);
            m_parkedBuses.push_back({b, m_stepFreeSlot});
            b.setPosition(-10, -10);
            m_moveCount++;
            emit moveCountChanged();
            emit dataChanged();
            processPassengerBoarding();
        }
        return;
    }

    // Desconecta para evitar múltiplas ligações
    disconnect(&m_moveStepTimer, &QTimer::timeout, this, &GameController::performNextMoveStep);
    connect(&m_moveStepTimer, &QTimer::timeout, this, &GameController::performNextMoveStep);
    m_moveStepTimer.start(150); // 150 ms por passo
}

void GameController::performNextMoveStep() {
    auto& buses = m_board.getBusesMutable();
    if (m_stepBusIndex < 0 || m_stepBusIndex >= static_cast<int>(buses.size())) {
        m_moveStepTimer.stop();
        return;
    }

    Bus& bus = buses[m_stepBusIndex];
    int currentR = bus.getRow();
    int currentC = bus.getCol();

    // Se já chegou ao destino
    if (currentR == m_stepTargetRow && currentC == m_stepTargetCol) {
        m_moveStepTimer.stop();
        // Se este movimento deveria terminar com estacionamento
        if (m_stepExitedBoard && m_stepFreeSlot != -1) {
            m_board.occupySlot(m_stepFreeSlot);
            m_parkedBuses.push_back({bus, m_stepFreeSlot});
            bus.setPosition(-10, -10);
            emit showNotification("🚌 Autocarro estacionado");
        }
        // Incrementa moveCount apenas uma vez no final do movimento completo
        m_moveCount++;
        emit moveCountChanged();
        updateAnalytics();
        emit dataChanged();
        checkGameStatus();
        // Inicia embarque se necessário
        processPassengerBoarding();
        return;
    }

    // Calcula próxima posição
    int newR = currentR + m_stepDeltaRow;
    int newC = currentC + m_stepDeltaCol;

    bool willExit = false;
    if (m_stepDeltaRow != 0) {
        if (newR < 0 || newR >= m_board.getRows()) willExit = true;
    } else if (m_stepDeltaCol != 0) {
        if (newC < 0 || newC >= m_board.getCols()) willExit = true;
    }

    if (willExit) {
        // Se era esperado sair e temos slot, então estaciona agora
        if (m_stepExitedBoard && m_stepFreeSlot != -1) {
            m_board.occupySlot(m_stepFreeSlot);
            m_parkedBuses.push_back({bus, m_stepFreeSlot});
            bus.setPosition(-10, -10);
            m_moveStepTimer.stop();
            m_moveCount++;
            emit moveCountChanged();
            emit dataChanged();
            processPassengerBoarding();
        } else {
            // Não devia sair, cancela movimento
            m_moveStepTimer.stop();
            emit showNotification("⚠️ Movimento inválido!");
        }
        return;
    }

    // Verifica colisão (ignora o próprio autocarro)
    if (m_board.isOccupied(newR, newC, m_stepBusIndex)) {
        m_moveStepTimer.stop();
        emit showNotification("⚠️ Colisão durante o movimento!");
        return;
    }

    // Executa o passo
    bus.setPosition(newR, newC);
    emit dataChanged();  
}


void GameController::processPassengerBoarding() {
    if (m_boardingTimer.isActive()) return;

    if (m_passengerQueue.isEmpty() || m_parkedBuses.empty()) {
        updateAnalytics();
        emit dataChanged();
        checkGameStatus();
        return;
    }

    QString nextColor = m_passengerQueue.first().color;
    for (size_t i = 0; i < m_parkedBuses.size(); ++i) {
        if (m_parkedBuses[i].bus.getColor() == nextColor && !m_parkedBuses[i].bus.isFull()) {
            m_currentParkedBusIndex = static_cast<int>(i);
            connect(&m_boardingTimer, &QTimer::timeout, this, &GameController::processNextBoardingStep, Qt::UniqueConnection);
            m_boardingTimer.start(300);
            return;
        }
    }
    updateAnalytics();
    emit dataChanged();
    checkGameStatus();
}

void GameController::processNextBoardingStep() {
    if (m_passengerQueue.isEmpty() || m_parkedBuses.empty()) {
        m_boardingTimer.stop();
        updateAnalytics();
        emit dataChanged();
        checkGameStatus();
        return;
    }
    if (m_currentParkedBusIndex < 0 || m_currentParkedBusIndex >= static_cast<int>(m_parkedBuses.size())) {
        m_boardingTimer.stop();
        processPassengerBoarding();
        return;
    }

    Bus& currentBus = m_parkedBuses[m_currentParkedBusIndex].bus;
    QString nextColor = m_passengerQueue.first().color;

    if (currentBus.getColor() == nextColor && !currentBus.isFull()) {
        currentBus.addPassenger();
        m_passengerQueue.removeFirst();
        emit dataChanged();

        if (currentBus.isFull()) {
            m_parkedBuses.erase(m_parkedBuses.begin() + m_currentParkedBusIndex);
            m_board.clearSlots();
            for (const auto& parked : m_parkedBuses)
                m_board.occupySlot(parked.slotIndex);
            emit dataChanged();
            m_boardingTimer.stop();
            processPassengerBoarding();
            return;
        }

        if (!m_passengerQueue.isEmpty() && m_passengerQueue.first().color == currentBus.getColor() && !currentBus.isFull()) {
            return; // continua no mesmo autocarro
        } else {
            m_boardingTimer.stop();
            processPassengerBoarding();
        }
    } else {
        m_boardingTimer.stop();
        processPassengerBoarding();
    }
    updateAnalytics();
}

void GameController::updateAnalytics() {
    int freeSlotsCount  = m_board.getNumSlots() - static_cast<int>(m_parkedBuses.size());
    int activeBusesCount = 0;
    for (const auto& b : m_board.getBuses())
        if (b.getRow() != -10) activeBusesCount++;
    m_score = GameAnalytics::calculateScore(m_moveCount, m_initialPassengersCount, m_passengerQueue.size());
    m_dangerLevel = GameAnalytics::evaluateBoardDanger(activeBusesCount, freeSlotsCount, m_passengerQueue.size());
    emit scoreChanged();
    emit dangerLevelChanged();
}

void GameController::loadLevelAsync(int levelNumber) {
    emit showNotification("⏳ A carregar Nível...");
    QFuture<LevelData> future = QtConcurrent::run([this, levelNumber]() {
        return readLevelFromJsonWorker(levelNumber);
    });
    auto *watcher = new QFutureWatcher<LevelData>(this);
    connect(watcher, &QFutureWatcher<LevelData>::finished, [this, watcher, levelNumber]() {
        applyLoadedLevel(watcher->result(), levelNumber);
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

GameController::LevelData GameController::readLevelFromJsonWorker(int levelNumber) {
    LevelData result;
    QFile file(":/levels.json");
    if (!file.open(QIODevice::ReadOnly)) {
        file.setFileName("levels.json");
        if (!file.open(QIODevice::ReadOnly))
            return result;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) return result;

    QJsonObject rootObj = doc.object();
    QJsonArray levelsArray = rootObj["levels"].toArray();
    QJsonObject levelObj;
    bool levelFound = false;
    for (int i = 0; i < levelsArray.size(); ++i) {
        QJsonObject cur = levelsArray[i].toObject();
        if (cur["levelId"].toInt() == levelNumber) {
            levelObj = cur;
            levelFound = true;
            break;
        }
    }
    if (!levelFound) return result;
    if (levelObj.contains("slotsCount"))
        result.slotsCount = levelObj["slotsCount"].toInt();
    for (const auto& val : levelObj["passengers"].toArray())
        result.passengers.append(Passenger(val.toString()));
    for (const auto& val : levelObj["buses"].toArray()) {
        QJsonObject bObj = val.toObject();
        QString dirStr = bObj["direction"].toString();
        Direction dir = Direction::Right;
        if (dirStr == "l") dir = Direction::Left;
        else if (dirStr == "u") dir = Direction::Up;
        else if (dirStr == "d") dir = Direction::Down;
        result.buses.push_back(Bus(bObj["color"].toString(),
                                   bObj["capacity"].toInt(),
                                   dir,
                                   bObj["row"].toInt(),
                                   bObj["col"].toInt()));
    }
    result.success = true;
    return result;
}

void GameController::applyLoadedLevel(LevelData data, int levelNumber) {
    if (!data.success) { emit showNotification("❌ Erro ao carregar o nível!"); return; }
    m_timer.stop();
    m_boardingTimer.stop();
    m_moveStepTimer.stop();
    m_elapsedSeconds = 0;
    m_board = Board(8, 8);
    m_board.setNumSlots(data.slotsCount);
    m_parkedBuses.clear();
    m_board.clearSlots();
    for (const auto& bus : data.buses) m_board.addBus(bus);
    m_passengerQueue = data.passengers;
    m_initialPassengersCount = data.passengers.size();
    m_currentLevel = levelNumber;
    m_moveCount = 0;
    m_gameState = "PLAYING";
    m_inMenu = false;
    m_score = GameAnalytics::calculateScore(0, m_initialPassengersCount, m_initialPassengersCount);
    m_dangerLevel = "ESTÁVEL 🟢";
    m_timer.start(1000);
    emit elapsedSecondsChanged();
    emit moveCountChanged();
    emit scoreChanged();
    emit dangerLevelChanged();
    emit inMenuChanged();
    emit dataChanged();
    emit gameStateChanged();
    processPassengerBoarding();
}

void GameController::checkGameStatus() {
    if (m_gameState != "PLAYING") return;

    bool anyBusesLeft = false;
    for (const auto& b : m_board.getBuses())
        if (b.getRow() != -10) { anyBusesLeft = true; break; }

    // VITÓRIA
    if (!anyBusesLeft && m_passengerQueue.isEmpty() && m_parkedBuses.empty()) {
        m_gameState = "WON";
        m_timer.stop();
        m_boardingTimer.stop();
        m_moveStepTimer.stop();
        Persistence::saveScore(m_currentLevel, m_score);
        Persistence::saveBestTime(m_currentLevel, m_elapsedSeconds);
        Persistence::markLevelCompleted(m_currentLevel);
        emit gameStateChanged();
        return;
    }

    // DERROTA
    if (!m_board.hasFreeSlot() && !m_passengerQueue.isEmpty()) {
        bool matchFound = false;
        QString nextColor = m_passengerQueue.first().color;
        for (const auto& parked : m_parkedBuses) {
            if (parked.bus.getColor() == nextColor && !parked.bus.isFull()) {
                matchFound = true;
                break;
            }
        }
        if (!matchFound) {
            m_gameState = "LOST";
            m_timer.stop();
            m_boardingTimer.stop();
            m_moveStepTimer.stop();
            emit gameStateChanged();
            emit showNotification("💀 Derrota! Sem plataformas compatíveis.");
            return;
        }
    }
}

void GameController::setupTestLevel() {
    loadLevelAsync(m_currentLevel);
}

int GameController::getLevelHighScore(int levelNumber) const { return Persistence::getHighScore(levelNumber); }
int GameController::getLevelBestTime(int levelNumber) const { return Persistence::getBestTime(levelNumber); }
bool GameController::isLevelCompleted(int levelNumber) const { return Persistence::isLevelCompleted(levelNumber); }

void GameController::goToMenu() {
    m_timer.stop();
    m_boardingTimer.stop();
    m_moveStepTimer.stop();
    m_inMenu = true;
    emit inMenuChanged();
}

void GameController::testImmutableExample() {
    GameAnalytics::BoardSnapshot snapshot;
    const auto& buses = m_board.getBuses();
    for (const auto& bus : buses)
        snapshot.busPositions.push_back({bus.getRow(), bus.getCol()});
    snapshot.moveCount = m_moveCount;
    snapshot.score = m_score;
    if (!snapshot.busPositions.empty()) {
        GameAnalytics::BoardSnapshot newSnap = GameAnalytics::applyMove(snapshot, 0, 0, 1);
        qDebug() << "[Immutable] original moveCount:" << snapshot.moveCount 
                 << " New:" << newSnap.moveCount;
    }
}