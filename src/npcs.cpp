#include "../include/npcs.h"
#include <cmath>
#include <iostream>

// Реализация базового класса NPC
NPC::NPC(const std::string& name, float x, float y, 
         int moveDist, int killDist, char sym) 
    : name(name), x(x), y(y), alive(true), 
      moveDistance(moveDist), killDistance(killDist), symbol(sym) {}

std::string NPC::getName() const { return name; }
float NPC::getX() const { return x; }
float NPC::getY() const { return y; }
std::pair<int, int> NPC::getPosition() const { 
    return {static_cast<int>(x), static_cast<int>(y)}; 
}

float NPC::distanceTo(const NPC& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool NPC::isInRange(const NPC& other, float range) const {
    return distanceTo(other) <= range;
}

void NPC::move(int dx, int dy, int maxX, int maxY) {
    if (!alive) return;
    
    dx *= moveDistance;
    dy *= moveDistance;
    
    x += dx;
    y += dy;
    
    // Проверяем границы карты (100x100)
    if (x < 0) x = 0;
    if (x >= maxX) x = maxX - 1;
    if (y < 0) y = 0;
    if (y >= maxY) y = maxY - 1;
}

// Реализация Orc (20 - дистанция хода, 10 - дистанция убийства)
Orc::Orc(const std::string& name, float x, float y) 
    : NPC(name, x, y, 20, 10, 'O') {}

std::string Orc::getType() const { return "Orc"; }

void Orc::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

bool Orc::canAttack(const NPC& other) const {
    return other.getType() == "Bear";
}

bool Orc::canBeAttackedBy(const NPC& other) const {
    return other.getType() == "Knight";
}

// Реализация Knight (30 - дистанция хода, 10 - дистанция убийства)
Knight::Knight(const std::string& name, float x, float y) 
    : NPC(name, x, y, 30, 10, 'K') {}

std::string Knight::getType() const { return "Knight"; }

void Knight::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

bool Knight::canAttack(const NPC& other) const {
    return other.getType() == "Orc";
}

bool Knight::canBeAttackedBy(const NPC& other) const {
    return other.getType() == "Bear";
}

// Реализация Bear (5 - дистанция хода, 10 - дистанция убийства)
Bear::Bear(const std::string& name, float x, float y) 
    : NPC(name, x, y, 5, 10, 'B') {}

std::string Bear::getType() const { return "Bear"; }

void Bear::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

bool Bear::canAttack(const NPC& other) const {
    return other.getType() == "Knight";
}

bool Bear::canBeAttackedBy(const NPC& other) const {
    return other.getType() == "Orc";
}
