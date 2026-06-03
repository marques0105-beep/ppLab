#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QStringList>
#include "board.h"
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
    void moveCountChanged();
    void showNotification(QString message);


private:
    struct ParkedBusInfo {
        Bus bus;
        int slotIndex;
    };

    void processPassengerBoarding();

    Board m_board;
    std::vector<ParkedBusInfo> m_parkedBuses;
    QList<Passenger> m_passengerQueue;

    int m_initialPassengersCount;

    QList<Passenger> m_passengerQueue;

    bool m_inMenu;
    QString m_gameState;
    int m_currentLevel; // "PLAYING", "WON", "LOST"
    int m_initialPassengersCount;
    int m_moveCount;
    
};

#endif // GAMECONTROLLER_H