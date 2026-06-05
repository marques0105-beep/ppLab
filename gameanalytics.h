#ifndef GAMEANALYTICS_H
#define GAMEANALYTICS_H

#include <QString>
#include <vector>
#include <utility>

// Todos os métodos são funções puras (sem efeitos colaterais, sem estado mutável)
// Atende ao requisito do paradigma de estilo funcional.
// BoardSnapshot + applyMove satisfaz o requisito de estruturas imutáveis
class GameAnalytics {
public:


    // Captura instantânea imutável do estado do tabuleiro (requisito do paradigma funcional)
    struct BoardSnapshot {
        std::vector<std::pair<int,int>> busPositions;
        int moveCount;
        int score;
    };


    // FUNÇÃO PURA — estilo imutável: tira um instantâneo e retorna um NOVO instantâneo.
    // NÃO modifica nada. Usada pelo GameController para pré-visualizar/gravar movimentos.
    static BoardSnapshot applyMove(const BoardSnapshot& current,
                                   int busIndex,
                                   int deltaRow,
                                   int deltaCol)
    {
        BoardSnapshot next = current; // copy — original is untouched
        if (busIndex >= 0 && busIndex < static_cast<int>(next.busPositions.size())) {
            next.busPositions[busIndex].first  += deltaRow;
            next.busPositions[busIndex].second += deltaCol;
            next.moveCount++;
        }
        return next; // Novo estado restaurado, original preservado
    }

    // Cálculo de pontuação
    static int calculateScore(int moveCount, int initialPassengers, int remainingPassengers) {
        int boarded = initialPassengers - remainingPassengers;
        if (boarded < 0) boarded = 0;

        int baseScore = boarded * 100;   // 100 pontos por passageiro embarcado
        int movePenalty = moveCount * 5;   // Penalidade de 5 pontos por movimento

        int finalScore = baseScore - movePenalty;
        return (finalScore < 0) ? 0 : finalScore;
    }

    // Perigo do tabuleiro
    static QString evaluateBoardDanger(int activeBusesCount,
                                       int freeSlotsCount,
                                       int queueLength)
    {
        if (freeSlotsCount == 0 && activeBusesCount > 0)
            return "CRÍTICO 💀";

        double dangerIndex = (activeBusesCount * 5.0)
                             + ((6 - freeSlotsCount) * 12.0)
                             + (queueLength * 4.0);

        if      (dangerIndex >= 75.0) return "PERIGO 🔥";
        else if (dangerIndex >= 40.0) return "MODERADO ⚠️";
        else                          return "ESTÁVEL 🟢";
    }
};

#endif // GAMEANALYTICS_H