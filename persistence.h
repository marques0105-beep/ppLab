#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QString>
#include <QSettings>

class Persistence
{
public:
    // Métodos para gestão de Pontuação Máxima (High Scores)
    static int getHighScore(int levelNumber);
    static void saveScore(int levelNumber, int score);

    // Métodos para gestão do Melhor Tempo (Best Times - Preparado para o Passo 14)
    static int getBestTime(int levelNumber);
    static void saveBestTime(int levelNumber, int seconds);

    // Métodos para validação do progresso do utilizador
    static bool isLevelCompleted(int levelNumber);
    static void markLevelCompleted(int levelNumber);
};

#endif // PERSISTENCE_H