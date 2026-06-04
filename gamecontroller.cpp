#include "gamecontroller.h"
#include "gameanalytics.h"
#include <QDebug>

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
        map["color"] = bus.getColor();
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

// MÉTODO DE INTERAÇÃO 
void GameController::handleBusClick(int busIndex) {
    if (m_gameState != "PLAYING") return;

    auto& buses = m_board.getBusesMutable();
    if (busIndex < 0 || busIndex >= static_cast<int>(buses.size())) return;

    Bus& bus = buses[busIndex];
    if (bus.getRow() == -10 || bus.getCol() == -10) return; // já estacionado

    int currentR = bus.getRow();
    int currentC = bus.getCol();
    int len = m_board.getBusLength(bus.getCapacity());

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
        emit showNotification("⚠️ Caminho bloqueado!");
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
            emit showNotification("🚌 Autocarro estacionado na plataforma " + QString::number(freeSlot+1));
            processPassengerBoarding();
        } else {
            emit showNotification("⚠️ Sem plataformas livres!");
        }
        return;
    }

    if (finalR != currentR || finalC != currentC) {
        bus.setPosition(finalR, finalC);
        m_moveCount++;
        emit moveCountChanged();
        emit dataChanged();
    }
}

void GameController::processPassengerBoarding() {
    bool progressMade = true;
    while (progressMade && !m_passengerQueue.isEmpty() && !m_parkedBuses.empty()) 
    {
        progressMade = false;

        QString nextPassengerColor = m_passengerQueue.first().color;

        for (auto& parked : m_parkedBuses) {
            if (parked.bus.getColor() == nextPassengerColor && !parked.bus.isFull()) 
            {
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


void GameController::loadLevelAsync(int) {}

void GameController::setupTestLevel() {
    loadLevelAsync(m_currentLevel);
}

void GameController::goToMenu()
{
    setInMenu(true);
    m_moveCount = 0;
    emit moveCountChanged();
}

int GameController::getLevelHighScore(int) const { return 0; }

int GameController::getLevelBestTime(int) const { return 0; }

bool GameController::isLevelCompleted(int) const { return false; }
