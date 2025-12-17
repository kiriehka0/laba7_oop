#ifndef NPCS_H
#define NPCS_H

#include <string>
#include <memory>
#include <vector>

class NPCVisitor;

// Базовый класс для всех NPC
class NPC {
protected:
    std::string name;
    float x, y;
    bool alive;          // статус жизни
    int moveDistance;    // дистанция перемещения
    int killDistance;    // дистанция убийства
    char symbol;         // символ для карты

public:
    NPC(const std::string& name, float x, float y, 
        int moveDist, int killDist, char sym);
    virtual ~NPC() = default;
    
    virtual std::string getType() const = 0;
    virtual void accept(NPCVisitor& visitor) = 0;
    
    std::string getName() const;
    float getX() const;
    float getY() const;
    
    float distanceTo(const NPC& other) const;
    bool isInRange(const NPC& other, float range) const;
    
    virtual bool canAttack(const NPC& other) const = 0;
    virtual bool canBeAttackedBy(const NPC& other) const = 0;
    bool isAlive() const { return alive; }
    void die() { alive = false; }
    int getMoveDistance() const { return moveDistance; }
    int getKillDistance() const { return killDistance; }
    char getSymbol() const { return symbol; }
    
    // Методы для перемещения
    void move(int dx, int dy, int maxX, int maxY);
    std::pair<int, int> getPosition() const;
};

// Конкретные классы NPC
class Orc : public NPC {
public:
    Orc(const std::string& name, float x, float y);
    std::string getType() const override;
    void accept(NPCVisitor& visitor) override;
    bool canAttack(const NPC& other) const override;
    bool canBeAttackedBy(const NPC& other) const override;
};

class Knight : public NPC {
public:
    Knight(const std::string& name, float x, float y);
    std::string getType() const override;
    void accept(NPCVisitor& visitor) override;
    bool canAttack(const NPC& other) const override;
    bool canBeAttackedBy(const NPC& other) const override;
};

class Bear : public NPC {
public:
    Bear(const std::string& name, float x, float y);
    std::string getType() const override;
    void accept(NPCVisitor& visitor) override;
    bool canAttack(const NPC& other) const override;
    bool canBeAttackedBy(const NPC& other) const override;
};

// Visitor для обработки NPC
class NPCVisitor {
public:
    virtual ~NPCVisitor() = default;
    virtual void visit(Orc& orc) = 0;
    virtual void visit(Knight& knight) = 0;
    virtual void visit(Bear& bear) = 0;
};

#endif
