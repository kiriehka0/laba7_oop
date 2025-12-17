#include "../include/dungeon_editor.h"
#include <fstream>
#include <iostream>
#include <algorithm>


BattleVisitor::BattleVisitor(float range, BattleNotifier& notifier, 
                           std::vector<std::unique_ptr<NPC>>& npcs)
    : battleRange(range), notifier(notifier), npcs(npcs), currentAttacker(nullptr) {}

void BattleVisitor::setCurrentAttacker(NPC* attacker) {
    currentAttacker = attacker;
}

void BattleVisitor::visit(Orc& orc) {
    if (currentAttacker) {
        performBattle(*currentAttacker, orc);
    }
}

void BattleVisitor::visit(Knight& knight) {
    if (currentAttacker) {
        performBattle(*currentAttacker, knight);
    }
}

void BattleVisitor::visit(Bear& bear) {
    if (currentAttacker) {
        performBattle(*currentAttacker, bear);
    }
}

void BattleVisitor::performBattle(NPC& attacker, NPC& defender) {

    bool attackerWins = attacker.canAttack(defender);
    bool defenderWins = defender.canAttack(attacker);
    
    std::string result;
    
    if (attackerWins && !defenderWins) {
   
        result = attacker.getType() + " " + attacker.getName() + 
                 " kills " + defender.getType() + " " + defender.getName();
        notifier.notifyObservers(result);
        
   
        markedForRemoval.insert(&defender);
    }
    else if (!attackerWins && defenderWins) {
    
        result = defender.getType() + " " + defender.getName() + 
                 " kills " + attacker.getType() + " " + attacker.getName();
        notifier.notifyObservers(result);
    
        markedForRemoval.insert(&attacker);
    }
    else if (attackerWins && defenderWins) {
 
        result = attacker.getType() + " " + attacker.getName() + 
                 " and " + defender.getType() + " " + defender.getName() + 
                 " kill each other";
        notifier.notifyObservers(result);
        
       
        markedForRemoval.insert(&attacker);
        markedForRemoval.insert(&defender);
    }
}

std::unordered_set<NPC*> BattleVisitor::getMarkedForRemoval() const {
    return markedForRemoval;
}

void BattleVisitor::clearMarkedForRemoval() {
    markedForRemoval.clear();
}


DungeonEditor::DungeonEditor() {
    fileLogger = std::make_shared<FileLogger>();
    consoleLogger = std::make_shared<ConsoleLogger>();
    
    notifier.addObserver(fileLogger);
    notifier.addObserver(consoleLogger);
}

bool DungeonEditor::addNPC(const std::string& type, const std::string& name, float x, float y) {
    try {
        auto npc = NPCFactory::createNPC(type, name, x, y);
        npcs.push_back(std::move(npc));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding NPC: " << e.what() << std::endl;
        return false;
    }
}

void DungeonEditor::printNPCs() const {
    std::cout << "NPC List:" << std::endl;
    std::cout << "---------" << std::endl;
    for (const auto& npc : npcs) {
        std::cout << "Type: " << npc->getType() 
                  << ", Name: " << npc->getName()
                  << ", Position: (" << npc->getX() << ", " << npc->getY() << ")"
                  << ", Alive: " << (npc->isAlive() ? "Yes" : "No")
                  << std::endl;
    }
}

bool DungeonEditor::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& npc : npcs) {
        file << NPCFactory::serializeNPC(*npc) << std::endl;
    }
    
    file.close();
    return true;
}

bool DungeonEditor::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    npcs.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                auto npc = NPCFactory::createNPCFromString(line);
                npcs.push_back(std::move(npc));
            } catch (const std::exception& e) {
                std::cerr << "Error loading NPC: " << e.what() << std::endl;
            }
        }
    }
    
    file.close();
    return true;
}

void DungeonEditor::startBattle(float range) {
    std::cout << "Starting battle with range: " << range << std::endl;
    
    std::unordered_set<NPC*> killedNPCs;
    bool battleOccurred;
    
    do {
        battleOccurred = false;
        BattleVisitor visitor(range, notifier, npcs);
    
        for (size_t i = 0; i < npcs.size(); ++i) {
            auto& attacker = npcs[i];
            if (!attacker) continue;
            if (!attacker->isAlive()) continue;
            if (killedNPCs.find(attacker.get()) != killedNPCs.end()) {
                continue;
            }
            
            visitor.setCurrentAttacker(attacker.get());
            
            for (size_t j = i + 1; j < npcs.size(); ++j) {
                auto& defender = npcs[j];
                if (!defender) continue;
                if (!defender->isAlive()) continue;
                
                if (killedNPCs.find(defender.get()) != killedNPCs.end()) {
                    continue;
                }
                
                // Проверяем дистанцию и возможность атаки
                if (attacker->isInRange(*defender, range) && 
                    attacker->canAttack(*defender)) {
                    defender->accept(visitor);
                    
                    auto newlyKilled = visitor.getMarkedForRemoval();
                    if (!newlyKilled.empty()) {
                        killedNPCs.insert(newlyKilled.begin(), newlyKilled.end());
                        battleOccurred = true;
                        visitor.clearMarkedForRemoval();
                    }
                }
            }
        }
    } while (battleOccurred);
    
    // Удаляем убитых NPC
    if (!killedNPCs.empty()) {
        npcs.erase(
            std::remove_if(npcs.begin(), npcs.end(),
                [&killedNPCs](const std::unique_ptr<NPC>& npc) {
                    return npc && killedNPCs.find(npc.get()) != killedNPCs.end();
                }),
            npcs.end()
        );
    }
    
    std::cout << "Battle finished. Remaining NPCs: " << npcs.size() << std::endl;
}

size_t DungeonEditor::getNPCCount() const {
    return npcs.size();
}

const std::vector<std::unique_ptr<NPC>>& DungeonEditor::getNPCs() const {
    return npcs;
}


std::vector<NPC*> DungeonEditor::getAliveNPCs() const {
    std::vector<NPC*> aliveNPCs;
    for (const auto& npc : npcs) {
        if (npc && npc->isAlive()) {
            aliveNPCs.push_back(npc.get());
        }
    }
    return aliveNPCs;
}
