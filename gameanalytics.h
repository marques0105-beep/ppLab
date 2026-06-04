#ifndef GAMEANALYTICS_H
#define GAMEANALYTICS_H

#include <QString>
#include "board.h"

class GameAnalytics
{
public:
    // Calcula a pontuação acumulada: cada passageiro dá 100 pontos, cada movimento desconta 5
    static int calculateScore(int passengersBoarded, int moveCount);

    // Avalia o nível de perigo com base na ocupação da matriz e das paragens ("LOW", "MEDIUM", "HIGH")
    static QString evaluateBoardDanger(const Board* board);
};

#endif // GAMEANALYTICS_H