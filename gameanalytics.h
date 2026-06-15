#ifndef GAMEANALYTICS_H
#define GAMEANALYTICS_H

#include <QString>
#include <vector>
#include <utility>

class GameAnalytics {
public:

    // Estrutura imutável para snapshots
    struct BoardSnapshot {
        std::vector<std::pair<int,int>> busPositions;
        int moveCount;
        int score;
    };

    // Função pura: aplica movimento a uma snapshot e retorna nova
    static BoardSnapshot applyMove(const BoardSnapshot& current,
                                   int busIndex,
                                   int deltaRow,
                                   int deltaCol)
    {
        BoardSnapshot next = current;
        if (busIndex >= 0 && busIndex < static_cast<int>(next.busPositions.size())) {
            next.busPositions[busIndex].first  += deltaRow;
            next.busPositions[busIndex].second += deltaCol;
            next.moveCount++;
        }
        return next; //estado restaurado, original preservado
    }

    // Função pura: cálculo da pontuação
    static int calculateScore(int moveCount, int initialPassengers, int remainingPassengers) {
        int boarded = initialPassengers - remainingPassengers;
        if (boarded < 0) boarded = 0;
        int baseScore = boarded * 100;  // 100 pontos por passageiro embarcado
        int movePenalty = moveCount * 5; // Penalidade de 5 pontos por movimento
        int finalScore = baseScore - movePenalty;
        return (finalScore < 0) ? 0 : finalScore;
    }
};

#endif // GAMEANALYTICS_H