#ifndef GAMETESTS_H
#define GAMETESTS_H

#include <iostream>
#include <cassert>
#include "gameanalytics.h"
#include "board.h"
#include "bus.h"

class GameTests {
public:
    static void runAllTests() {
        std::cout << "========================================" << std::endl;
        std::cout << "?? A iniciar Testes Unitários do Motor..." << std::endl;

        testGameAnalytics();
        testBusLogic();
        testBoardLogic();

        std::cout << "? Todos os Testes Unitários passaram com sucesso!" << std::endl;
        std::cout << "========================================" << std::endl;
    }

private:

    // Lógica do GameAnalytics (cálculo de pontuação e avaliação de perigo)
    static void testGameAnalytics() {

        // 10 passageiros embarcaram (10-0), 5 movimentos -> 10*100 - 5*5 = 975
        int score = GameAnalytics::calculateScore(5, 10, 0);
        assert(score == 975);

        // A pontuação nunca fica negativa
        int negScore = GameAnalytics::calculateScore(1000, 1, 1);
        assert(negScore == 0);

        // 0 vagas livres + autocarros ativos -> CRÍTICO
        QString danger = GameAnalytics::evaluateBoardDanger(3, 0, 5);
        assert(danger == "CRÍTICO 💀");

        // Estado seguro -> ESTAVEL
        QString safe = GameAnalytics::evaluateBoardDanger(0, 6, 0);
        assert(safe == "ESTÁVEL 🟢");

        std::cout << "   testGameAnalytics OK" << std::endl;
    }

    // Capacidade do autocarro e identificação de passageiros
    static void testBusLogic() {
        Bus b("red", 4, Direction::Right, 0, 0);

        assert(b.getColor()    == "red");
        assert(b.getCapacity() == 4);
        assert(b.getCurrentPassengers() == 0);
        assert(!b.isFull());

        b.addPassenger();
        b.addPassenger();
        b.addPassenger();
        b.addPassenger();

        assert(b.isFull());
        assert(b.getCurrentPassengers() == 4);

        // Posição definida
        b.setPosition(3, 5);
        assert(b.getRow() == 3);
        assert(b.getCol() == 5);

        std::cout << "   testBusLogic OK" << std::endl;
    }