#include "game_manager.h"
#include <iostream>

int main() {
    try {
        std::cout << "Starting NPC Battle Simulation..." << std::endl;
        
        GameManager game;
        game.run();
        
        std::cout << "\nSimulation completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
