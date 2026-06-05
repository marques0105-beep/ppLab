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

};


#endif // GAMETESTS_H

