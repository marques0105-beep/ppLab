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
    // ------------------------------------------------------------------
    // Test 1 — Pure functions in GameAnalytics
    // ------------------------------------------------------------------
    static void testGameAnalytics() {
        // 10 passengers boarded (10-0), 5 moves -> 10*100 - 5*5 = 975
        int score = GameAnalytics::calculateScore(5, 10, 0);
        assert(score == 975);

        // Score never goes negative
        int negScore = GameAnalytics::calculateScore(1000, 1, 1);
        assert(negScore == 0);

        // 0 free slots + active buses -> CRITICO
        QString danger = GameAnalytics::evaluateBoardDanger(3, 0, 5);
        assert(danger == "CRÍTICO 💀");

        // Safe state -> ESTAVEL
        QString safe = GameAnalytics::evaluateBoardDanger(0, 6, 0);
        assert(safe == "ESTÁVEL 🟢");

        std::cout << "   testGameAnalytics OK" << std::endl;
    }


    // ------------------------------------------------------------------
    // Test 2 — Bus capacity and passenger tracking
    // ------------------------------------------------------------------
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

        // setPosition
        b.setPosition(3, 5);
        assert(b.getRow() == 3);
        assert(b.getCol() == 5);

        std::cout << "   testBusLogic OK" << std::endl;
    }



    // ------------------------------------------------------------------
    // Test 3 — Board slot management and collision detection
    // ------------------------------------------------------------------
    static void testBoardLogic() {
        Board board(8, 8);

        assert(board.getRows() == 8);
        assert(board.getCols() == 8);
        assert(board.getNumSlots() == 6);
        assert(board.hasFreeSlot());
        assert(board.getNextFreeSlotIndex() == 0);

        // Occupy all slots
        for (int i = 0; i < board.getNumSlots(); ++i)
            board.occupySlot(i);
        assert(!board.hasFreeSlot());
        assert(board.getNextFreeSlotIndex() == -1);

        // Clear and verify
        board.clearSlots();
        assert(board.hasFreeSlot());

        // Collision detection — horizontal bus (Right) at (2,3) capacity 4 = 2 cells
        Bus hBus("blue", 4, Direction::Right, 2, 3); // occupies (2,3) and (2,4)
        board.addBus(hBus);
        assert( board.isOccupied(2, 3));
        assert( board.isOccupied(2, 4));
        assert(!board.isOccupied(2, 5));

        // Collision detection — vertical bus (Down) at (5,1) capacity 6 = 3 cells
        Bus vBus("green", 6, Direction::Down, 5, 1); // occupies (5,1),(6,1),(7,1)
        board.addBus(vBus);
        assert( board.isOccupied(5, 1));
        assert( board.isOccupied(6, 1));
        assert( board.isOccupied(7, 1));
        assert(!board.isOccupied(4, 1));

        // getBusesMutable — verify mutator does not crash and bus count is correct
        auto& mutableBuses = board.getBusesMutable();
        assert(mutableBuses.size() == 2);

        std::cout << "   testBoardLogic OK" << std::endl;
    }
};


#endif // GAMETESTS_H