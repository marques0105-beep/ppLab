#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include <QStringList>
#include "board.h"
#include "bus.h"
#include "passenger.h"

class GameController : public QObject
{
    Q_OBJECT

    // PROPRIEDADES QML BÁSICAS 
    Q_PROPERTY(int moveCount READ moveCount NOTIFY moveCountChanged)
    Q_PROPERTY(bool inMenu READ inMenu NOTIFY inMenuChanged)
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY gameStateChanged)
    
    // Dimensionamento do tabuleiro acoplado à classe Board
    Q_PROPERTY(int rows READ rows CONSTANT)
    Q_PROPERTY(int cols READ cols CONSTANT)

    Q_PROPERTY(QVariantList buses READ getBusesForDisplay NOTIFY dataChanged)
    Q_PROPERTY(QVariantList parkedBuses READ getParkedBusesForDisplay NOTIFY dataChanged)
    Q_PROPERTY(int numSlots READ numSlots NOTIFY dataChanged)

    Q_PROPERTY(QVariantList passengerQueue READ passengerQueue NOTIFY passengerQueueChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(QString dangerLevel READ dangerLevel NOTIFY dangerLevelChanged)

public:
    explicit GameController(QObject *parent = nullptr);
    virtual ~GameController();

    // Getters fundamentais
    bool inMenu() const;
    QString gameState() const;
    int currentLevel() const;
    int rows() const;
    int cols() const;
    int moveCount() const;
    QVariantList passengerQueue() const;
    int score() const { return m_score; }
    QString dangerLevel() const { return m_dangerLevel; }

    QVariantList getPassengerQueueForDisplay() const;
    QVariantList getBusesForDisplay() const;
    QVariantList getParkedBusesForDisplay() const;

    // Métodos de controlo de fluxo 
    Q_INVOKABLE void setInMenu(bool inMenu);
    Q_INVOKABLE void goToMenu();
    Q_INVOKABLE void setupTestLevel();
    Q_INVOKABLE void handleBusClick(int busIndex); 

signals:
    void inMenuChanged();
    void gameStateChanged();
    void currentLevelChanged();
    void boardDimensionsChanged();
    void busesChanged();
    void passengerQueueChanged();
    void analyticsChanged();
    void showNotification(const QString &message);

private:
    struct ParkedBusInfo {
        Bus bus;
        int slotIndex;
    };

    void updateAnalytics();
    void processPassengerBoarding();
    void checkGameStatus(); 


    Board m_board;
    std::vector<ParkedBusInfo> m_parkedBuses;
    QList<Passenger> m_passengerQueue;

    int m_initialPassengersCount;

    QList<Passenger> m_passengerQueue;

    bool m_inMenu;
    QString m_gameState;
    int m_currentLevel;
    int m_rows;
    int m_cols;
    int m_moveCount;
    int m_score;
    int m_passengersBoardedCount;
    QString m_dangerLevel;

    // Variáveis de estado do Passo 9
    int m_score;
    int m_passengersBoardedCount;
    QString m_dangerLevel;
};

#endif // GAMECONTROLLER_H