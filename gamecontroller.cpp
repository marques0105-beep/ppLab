#include "gamecontroller.h"
#include "gameanalytics.h"
#include "persistence.h"
#include <QtConcurrent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

// Construtor e destrutor
GameController::GameController(QObject *parent)
    : QObject(parent),
    m_board(8, 8),
    m_moveCount(0),
    m_currentLevel(1),
    m_gameState("PLAYING"),
    m_score(1000),
    m_dangerLevel("ESTÁVEL 🟢"),
    m_inMenu(true),
    m_initialPassengersCount(0),
    m_elapsedSeconds(0)
{
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_elapsedSeconds++;
        emit elapsedSecondsChanged();
    });
}

GameController::~GameController() {
    m_timer.stop();
}

QVariantList GameController::getPassengerQueueForDisplay() const {
    QVariantList list;
    for (const auto& passenger : m_passengerQueue) {
        list.append(passenger.toVariantMap());
    }
    return list;
}

QVariantList GameController::getBusesForDisplay() const {
    QVariantList list;
    const auto& buses = m_board.getBuses();
    for (const auto& bus : buses) {
        QVariantMap map;
        map["color"]    = bus.getColor();
        map["capacity"] = bus.getCapacity();

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

QVariantList GameController::getParkedBusesForDisplay() const {
    QVariantList list;
    for (const auto& parked : m_parkedBuses) {
        QVariantMap map;
        map["color"] = parked.bus.getColor();
        map["capacity"] = parked.bus.getCapacity();
        map["currentPassengers"] = parked.bus.getCurrentPassengers();
        map["slotIndex"] = parked.slotIndex;
        list.append(map);
    }
    return list;
}

// MÉTODO DE INTERAÇÃO PRINCIPAL 
void GameController::handleBusClick(int busIndex) {
    if (m_gameState != "PLAYING") return;

    auto& buses = m_board.getBusesMutable();
    if (busIndex < 0 || busIndex >= static_cast<int>(buses.size())) return;

    Bus& bus = buses[busIndex];

    if (bus.getRow() == -10 || bus.getCol() == -10) return;

    int currentR = bus.getRow();
    int currentC = bus.getCol();
    int len = bus.getCapacity() / 2;

    int finalR = currentR, finalC = currentC;
    bool pathBlocked = false, exitedBoard = false;

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

    if (finalR == currentR && finalC == currentC && pathBlocked) {
        emit showNotification("⚠️ Caminho Bloqueado!");
        return;
    }

    if (exitedBoard) {
        if (m_board.hasFreeSlot()) {
            int freeSlot = m_board.getNextFreeSlotIndex();
            m_board.occupySlot(freeSlot);
            m_parkedBuses.push_back({bus, freeSlot});

            bus.setPosition(-10, -10);

            m_moveCount++;
            emit moveCountChanged();
            emit dataChanged();

            processPassengerBoarding();
        } else {
            emit showNotification("⚠️ Plataformas cheias!");
        }
        return;
    }

    if (finalR != currentR || finalC != currentC) {
        bus.setPosition(finalR, finalC);
        m_moveCount++;
        emit moveCountChanged();
        updateAnalytics();
        emit dataChanged();
        checkGameStatus();
    }
}

void GameController::processPassengerBoarding() {
    bool progressMade = true;
    while (progressMade && !m_passengerQueue.isEmpty() && !m_parkedBuses.empty()) {
        progressMade = false;

        QString nextPassengerColor = m_passengerQueue.first().color;

        for (auto& parked : m_parkedBuses) {
            if (parked.bus.getColor() == nextPassengerColor && !parked.bus.isFull()) {
                parked.bus.addPassenger();
                m_passengerQueue.removeFirst();
                progressMade = true;
                break;
            }
        }
    }

    // Remover autocarros cheios
    for (auto it = m_parkedBuses.begin(); it != m_parkedBuses.end(); ) {
        if (it->bus.isFull()) {
            it = m_parkedBuses.erase(it);
        } else {
            ++it;
        }
    }

    // Atualizar ocupação dos slots
    m_board.clearSlots();
    for (const auto& parked : m_parkedBuses) {
        m_board.occupySlot(parked.slotIndex);
    }

    updateAnalytics();
    emit dataChanged();
    checkGameStatus();
}

void GameController::updateAnalytics() {
    int freeSlotsCount = m_board.getNumSlots() - static_cast<int>(m_parkedBuses.size());
    int activeBusesCount = 0;
    for (const auto& b : m_board.getBuses()) {
        if (b.getRow() != -10) activeBusesCount++;
    }

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
        if (!file.open(QIODevice::ReadOnly)) {
            return result;
        }
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
        QJsonObject currentLvl = levelsArray[i].toObject();
        if (currentLvl["levelId"].toInt() == levelNumber) {
            levelObj = currentLvl;
            levelFound = true;
            break;
        }
    }
    if (!levelFound) return result;

    if (levelObj.contains("slotsCount")) {
        result.slotsCount = levelObj["slotsCount"].toInt();
    }

    for (const auto& val : levelObj["passengers"].toArray()) {
        result.passengers.append(Passenger(val.toString()));
    }

    for (const auto& val : levelObj["buses"].toArray()) {
        QJsonObject bObj = val.toObject();
        QString dirStr = bObj["direction"].toString();

        Direction dir = Direction::Right;
        if      (dirStr == "l") dir = Direction::Left;
        else if (dirStr == "u") dir = Direction::Up;
        else if (dirStr == "d") dir = Direction::Down;

        result.buses.push_back(Bus(bObj["color"].toString(), bObj["capacity"].toInt(),
                                   dir, bObj["row"].toInt(), bObj["col"].toInt()));
    }
    result.success = true;
    return result;
}

void GameController::applyLoadedLevel(LevelData data, int levelNumber) {
    if (!data.success) { emit showNotification("❌ Erro ao carregar o nível!"); return; }

    m_timer.stop();
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
    m_score = 1000;
    m_dangerLevel = "ESTÁVEL 🟢";
    m_gameState = "PLAYING";
    m_inMenu = false;

    m_timer.start(1000);
    emit elapsedSecondsChanged();

    emit moveCountChanged(); emit scoreChanged(); emit dangerLevelChanged();
    emit inMenuChanged(); emit dataChanged(); emit gameStateChanged();
    processPassengerBoarding();
}

void GameController::checkGameStatus() {
    if (m_gameState != "PLAYING") return;

    bool anyBusesLeft = false;
    for (const auto& b : m_board.getBuses()) {
        if (b.getRow() != -10) {
            anyBusesLeft = true;
            break;
        }
    }

    // VITÓRIA
    if (!anyBusesLeft && m_passengerQueue.isEmpty() && m_parkedBuses.empty()) {
        m_gameState = "WON";
        m_timer.stop();

        // GRAVAÇÃO DOS DADOS
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
            emit gameStateChanged();
        }
    }
}

void GameController::setupTestLevel() {
    loadLevelAsync(m_currentLevel);
}

int GameController::getLevelHighScore(int levelNumber) const {
    return Persistence::getHighScore(levelNumber);
}

int GameController::getLevelBestTime(int levelNumber) const {
    return Persistence::getBestTime(levelNumber);
}

bool GameController::isLevelCompleted(int levelNumber) const {
    return Persistence::isLevelCompleted(levelNumber);
}

void GameController::goToMenu() {
    m_timer.stop();
    m_inMenu = true;
    emit inMenuChanged();
}