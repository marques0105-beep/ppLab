#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include "board.h"

class GameController : public QObject
{
    Q_OBJECT

    // PROPRIEDADES QML BÁSICAS 
    Q_PROPERTY(bool inMenu READ inMenu WRITE setInMenu NOTIFY inMenuChanged)
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY currentLevelChanged)

    // Dimensionamento do tabuleiro acoplado à classe Board
    Q_PROPERTY(int rows READ rows NOTIFY boardDimensionsChanged)
    Q_PROPERTY(int cols READ cols NOTIFY boardDimensionsChanged)

    Q_PROPERTY(QVariantList buses READ getBusesForDisplay NOTIFY dataChanged)


public:
    explicit GameController(QObject *parent = nullptr);
    virtual ~GameController();

    // Getters fundamentais
    bool inMenu() const;
    QString gameState() const;
    QVariantList buses() const;
    int currentLevel() const;
    int rows() const;
    int cols() const;
    int moveCount() const;

    // Métodos de controlo de fluxo iniciais
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
    void showNotification(const QString &message);

private:
    bool m_inMenu;
    QString m_gameState;
    int m_currentLevel; // "PLAYING", "WON", "LOST"

    int m_rows;
    int m_cols;
    int m_moveCount;
    Board* m_board; // Ponteiro para o tabuleiro que criámos no Passo 2
};

#endif // GAMECONTROLLER_H