#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QStringList>
#include "board.h"
#include "bus.h"
#include "passenger.h"

class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows CONSTANT)
    Q_PROPERTY(int cols READ cols CONSTANT)

    Q_PROPERTY(int numSlots READ numSlots NOTIFY dataChanged)
    Q_PROPERTY(int moveCount READ moveCount NOTIFY moveCountChanged)

    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY gameStateChanged)
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)

    Q_PROPERTY(int score READ score NOTIFY scoreChanged)

    Q_PROPERTY(bool inMenu READ inMenu NOTIFY inMenuChanged)
    Q_PROPERTY(int elapsedSeconds READ elapsedSeconds NOTIFY elapsedSecondsChanged)

    Q_PROPERTY(QVariantList passengerQueue READ getPassengerQueueForDisplay NOTIFY dataChanged)
    Q_PROPERTY(QVariantList buses READ getBusesForDisplay NOTIFY dataChanged)
    Q_PROPERTY(QVariantList parkedBuses READ getParkedBusesForDisplay NOTIFY dataChanged)

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    int rows() const { return m_board.getRows(); }
    int cols() const { return m_board.getCols(); }
    int numSlots() const { return m_board.getNumSlots(); }
    int moveCount() const { return m_moveCount; }
    int currentLevel() const { return m_currentLevel; }
    QString gameState() const { return m_gameState; }
    int score() const { return m_score; }
    bool inMenu() const { return m_inMenu; }
    int elapsedSeconds() const { return m_elapsedSeconds; }

    QVariantList getPassengerQueueForDisplay() const;
    QVariantList getBusesForDisplay() const;
    QVariantList getParkedBusesForDisplay() const;

    Q_INVOKABLE void handleBusClick(int busIndex);
    Q_INVOKABLE void loadLevelAsync(int levelNumber);
    Q_INVOKABLE void setupTestLevel();
    Q_INVOKABLE void goToMenu();
    Q_INVOKABLE void testImmutableExample();
    Q_INVOKABLE int  getLevelHighScore(int levelNumber) const;
    Q_INVOKABLE int  getLevelBestTime(int levelNumber) const;
    Q_INVOKABLE bool isLevelCompleted(int levelNumber) const;
    Q_INVOKABLE void resetProgress(); // apaga todo o progresso (recordes, tempos, níveis concluídos)


signals:
    void moveCountChanged();
    void gameStateChanged();
    void scoreChanged();
    void inMenuChanged();
    void dataChanged();
    void elapsedSecondsChanged();
    void showNotification(QString message);
    void progressReset(); // emitido quando o progresso é apagado, para o QML refrescar o menu

private:
    struct ParkedBusInfo {
        Bus bus;
        int slotIndex;
    };

    struct LevelData {
        bool success = false;
        int slotsCount = 4;
        QList<Passenger> passengers;
        std::vector<Bus> buses;
    };

    void processPassengerBoarding();
    void processNextBoardingStep();
    void updateAnalytics();
    void checkGameStatus();
    LevelData readLevelFromJsonWorker(int levelNumber);
    void applyLoadedLevel(LevelData data, int levelNumber);
    void performNextMoveStep(); // movimento passo-a-passo
 
    Board m_board;
    std::vector<ParkedBusInfo> m_parkedBuses;
    QList<Passenger> m_passengerQueue;

    int m_moveCount;
    int m_currentLevel;
    QString m_gameState;
    int m_score;
    bool m_inMenu;
    int m_initialPassengersCount;

    QTimer m_timer;
    int m_elapsedSeconds;

    // Timers para animações
    QTimer m_boardingTimer;
    int m_currentParkedBusIndex;

    QTimer m_moveStepTimer;
    int m_stepBusIndex;
    int m_stepTargetRow;
    int m_stepTargetCol;
    int m_stepDeltaRow;
    int m_stepDeltaCol;
    bool m_stepExitedBoard;
    int m_stepFreeSlot;
};

#endif // GAMECONTROLLER_H