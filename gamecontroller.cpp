#include "gamecontroller.h"
#include <QTimer>

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

    // Adicionar um autocarro de exemplo para teste de visualização
    m_board.addBus(Bus("red", 4, Direction::Right, 2, 2));
}

GameController::~GameController()
{
    delete m_board;
}

QVariantList GameController::getBusesForDisplay() const {
    QVariantList list;
    
    const auto& buses = m_board.getBuses();
    
    // Mapeia cada objeto Bus do C++ para um QVariantMap aceitável pelo QML
    for (const auto& bus : buses) {
        QVariantMap map;
        map["color"]    = bus.getColor();
        map["capacity"] = bus.getCapacity();
        QString dStr = "r";
        if (bus.getDirection() == Direction::Left) dStr = "l";
        else if (bus.getDirection() == Direction::Up) dStr = "u";
        else if (bus.getDirection() == Direction::Down) dStr = "d";
        map["direction"] = dStr;
        map["row"] = bus.getRow();
        map["col"] = bus.getCol();

        list.append(map);
    }
    return list;
}

QVariantList GameController::getParkedBusesForDisplay() const { return {}; }


// GETTERS 
bool GameController::inMenu() const { return m_inMenu; }

QString GameController::gameState() const { return m_gameState; }

int GameController::currentLevel() const { return m_currentLevel; }

int GameController::rows() const { return m_rows; }

int GameController::cols() const { return m_cols; }

int GameController::moveCount() const { return m_moveCount; }

// MÉTODOS DE CONTROLO DE NAVEGAÇÃO 
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
    
    m_gameState = "PLAYING";
    emit gameStateChanged();
    
    setInMenu(false);
    emit showNotification("Nível Inicial Carregado!");
}

// MÉTODO DE INTERAÇÃO 
void GameController::handleBusClick(int busIndex)
{
    // Salvaguarda: verificar se o tabuleiro existe e se o índice é válido
    if (!m_board || busIndex < 0 || busIndex >= static_cast<int>(m_board->getBuses().size())) {
        return;
    }

    // Obter uma referência ao autocarro clicado para o podermos modificar
    // Nota: Como vi na tua Imagem 6 que tens o método getBusesMutable(), usamos esse!
    std::vector<Bus>& busesList = m_board->getBusesMutable();
    Bus& targetBus = busesList[busIndex];

    // Guardar as coordenadas atuais do autocarro
    int currentRow = targetBus.row();
    int currentCol = targetBus.col();

    // Mover apenas UMA célula na direção correspondente (Lógica cega do Passo 5.1)
    switch (targetBus.direction()) {
        case 'r': // Right (Direita) -> Avança na coluna
            currentCol++;
            break;
        case 'l': // Left (Esquerda) -> Recua na coluna
            currentCol--;
            break;
        case 'd': // Down (Baixo) -> Avança na linha
            currentRow++;
            break;
        case 'u': // Up (Cima) -> Recua na linha
            currentRow--;
            break;
        default:
            break;
    }

    // Atualizar a posição do autocarro com as novas coordenadas
    targetBus.setPosition(currentRow, currentCol);

    // Incrementar o contador de movimentos do jogo (moveCount)
    m_moveCount++;
    emit moveCountChanged();

    // Avisar o QML que a lista de autocarros mudou para ele se redesenhar no ecrã
    emit busesChanged();

    qDebug() << "Autocarro" << busIndex << "moveu-se para a posição:" << currentRow << "," << currentCol;
}

void GameController::handleBusClick(int busIndex) {
    if (m_gameState != "PLAYING") return;

    auto& buses = m_board.getBusesMutable();
    if (busIndex < 0 || busIndex >= static_cast<int>(buses.size())) return;

    Bus& bus = buses[busIndex];
    if (bus.getRow() == -10 || bus.getCol() == -10) return; // já estacionado

    int row = bus.getRow();
    int col = bus.getCol();
    Direction dir = bus.getDirection();

    // Calcula nova posição (apenas uma célula por agora)
    int newRow = row;
    int newCol = col;
    switch (dir) {
    case Direction::Right: newCol++; break;
    case Direction::Left: newCol--; break;
    case Direction::Down: newRow++; break;
    case Direction::Up: newRow--; break;
    }

    // Verifica limites do tabuleiro (ainda não sai, apenas não move se for para fora)
    if (newRow < 0 || newRow >= m_board.getRows() ||
        newCol < 0 || newCol >= m_board.getCols()) {
        emit showNotification("⚠️ Atingiu o limite! (futuro estacionamento)");
        return;
    }

    // Move o autocarro
    bus.setPosition(newRow, newCol);
    m_moveCount++;
    emit moveCountChanged();
    emit dataChanged();
}


void GameController::loadLevelAsync(int) {}

void GameController::setupTestLevel() {}

void GameController::goToMenu() {}

int GameController::getLevelHighScore(int) const { return 0; }

int GameController::getLevelBestTime(int) const { return 0; }

bool GameController::isLevelCompleted(int) const { return false; }
