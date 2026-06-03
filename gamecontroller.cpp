#include "gamecontroller.h"
#include <QDebug>

GameController::GameController(QObject *parent)
    : QObject(parent)
    , m_inMenu(true)
    , m_gameState("PLAYING")
    , m_currentLevel(1)
    , m_rows(8)  
    , m_cols(8)
{
    qDebug() << "GameController inicializado no estado de Menu.";
}

GameController::~GameController()
{

}

// GETTERS FUNDAMENTAIS 
bool GameController::inMenu() const
{
    return m_inMenu;
}

QString GameController::gameState() const
{
    return m_gameState;
}

int GameController::currentLevel() const
{
    return m_currentLevel;
}

int GameController::rows() const
{
    return m_rows;
}

int GameController::cols() const
{
    return m_cols;
}


// MÉTODOS DE CONTROLO DE FLUXO E NAVEGAÇÃO
void GameController::setInMenu(bool inMenu)
{
    if (m_inMenu != inMenu) {
        m_inMenu = inMenu;
        emit inMenuChanged();
        qDebug() << "Estado do Menu alterado para:" << m_inMenu;
    }
}

void GameController::goToMenu()
{
    setInMenu(true);
    m_gameState = "PLAYING";
    emit gameStateChanged();
}

void GameController::setupTestLevel()
{
    qDebug() << "A inicializar estrutura básica do nível de teste...";
    
    // Configura o estado inicial do motor e sai do menu para o jogo
    m_gameState = "PLAYING";
    emit gameStateChanged();
    
    setInMenu(false);
    emit showNotification("Nível Inicial Carregado!");
}