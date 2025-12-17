#include "../include/factory.h"
#include <sstream>
#include <stdexcept>

std::unique_ptr<NPC> NPCFactory::createNPC(const std::string& type, 
                                          const std::string& name, 
                                          float x, float y) {
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::invalid_argument("Coordinates must be in range 0-500");
    }
    
    if (type == "Orc") {
        return std::make_unique<Orc>(name, x, y);
    } else if (type == "Knight") {
        return std::make_unique<Knight>(name, x, y);
    } else if (type == "Bear") {
        return std::make_unique<Bear>(name, x, y);
    } else {
        throw std::invalid_argument("Unknown NPC type: " + type);
    }
}

std::unique_ptr<NPC> NPCFactory::createNPCFromString(const std::string& data) {
    std::istringstream iss(data);
    std::string type, name;
    float x, y;
    
    if (!(iss >> type >> name >> x >> y)) {
        throw std::invalid_argument("Invalid NPC data format");
    }
    
    return createNPC(type, name, x, y);
}

std::string NPCFactory::serializeNPC(const NPC& npc) {
    std::ostringstream oss;
    oss << npc.getType() << " " << npc.getName() << " " 
        << npc.getX() << " " << npc.getY();
    return oss.str();
}
