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

    // PROPRIEDADES QML
    Q_PROPERTY(int rows READ rows CONSTANT)
    Q_PROPERTY(int cols READ cols CONSTANT)
    Q_PROPERTY(int numSlots READ numSlots NOTIFY dataChanged)
    Q_PROPERTY(int moveCount READ moveCount NOTIFY moveCountChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY gameStateChanged)
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(QString dangerLevel READ dangerLevel NOTIFY dangerLevelChanged)
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
    QString dangerLevel() const { return m_dangerLevel; }
    bool inMenu() const { return m_inMenu; }
    int elapsedSeconds() const { return m_elapsedSeconds; }

    QVariantList getPassengerQueueForDisplay() const;
    QVariantList getBusesForDisplay() const;
    QVariantList getParkedBusesForDisplay() const;

    Q_INVOKABLE void handleBusClick(int busIndex);
    Q_INVOKABLE void loadLevel(int levelNumber);      // síncrono por agora
    Q_INVOKABLE void goToMenu();

    // Para o menu – versões dummy (persistência virá depois)
    Q_INVOKABLE int  getLevelHighScore(int levelNumber) const { return 0; }
    Q_INVOKABLE int  getLevelBestTime(int levelNumber) const { return 0; }
    Q_INVOKABLE bool isLevelCompleted(int levelNumber) const { return false; }

signals:
    void moveCountChanged();
    void gameStateChanged();
    void scoreChanged();
    void dangerLevelChanged();
    void inMenuChanged();
    void dataChanged();
    void elapsedSecondsChanged();
    void showNotification(QString message);

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

    LevelData readLevelFromJson(int levelNumber);
    void applyLevel(const LevelData& data, int levelNumber);
    void processPassengerBoarding();
    void updateAnalytics();
    void checkGameStatus();

    Board m_board;
    std::vector<ParkedBusInfo> m_parkedBuses;
    QList<Passenger> m_passengerQueue;
    int m_initialPassengersCount;
    int m_moveCount;
    int m_currentLevel;
    QString m_gameState;
    int m_score;
    QString m_dangerLevel;
    bool m_inMenu;
    QTimer m_timer;
    int m_elapsedSeconds;
};

#endif // GAMECONTROLLER_H