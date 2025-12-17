#ifndef DUNGEON_EDITOR_H
#define DUNGEON_EDITOR_H

#include "npcs.h"
#include "factory.h"
#include "observer.h"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>


class BattleVisitor;

class DungeonEditor {
private:
    std::vector<std::unique_ptr<NPC>> npcs;
    BattleNotifier notifier;
    std::shared_ptr<FileLogger> fileLogger;
    std::shared_ptr<ConsoleLogger> consoleLogger;

public:
    DungeonEditor();
    
    // Основные методы редактора
    bool addNPC(const std::string& type, const std::string& name, float x, float y);
    void printNPCs() const;
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    void startBattle(float range);
    
    // Вспомогательные методы
    size_t getNPCCount() const;
    const std::vector<std::unique_ptr<NPC>>& getNPCs() const;
    std::vector<NPC*> getAliveNPCs() const;
};

class BattleVisitor : public NPCVisitor {
private:
    NPC* currentAttacker;
    float battleRange;
    BattleNotifier& notifier;
    std::vector<std::unique_ptr<NPC>>& npcs;
    std::unordered_set<NPC*> markedForRemoval;
    
public:
    BattleVisitor(float range, BattleNotifier& notifier, 
                  std::vector<std::unique_ptr<NPC>>& npcs);
    
    void setCurrentAttacker(NPC* attacker);
    void visit(Orc& orc) override;
    void visit(Knight& knight) override;
    void visit(Bear& bear) override;
    
    void performBattle(NPC& attacker, NPC& defender);
    std::unordered_set<NPC*> getMarkedForRemoval() const;
    void clearMarkedForRemoval();
};

#endif
