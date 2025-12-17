#ifndef FACTORY_H
#define FACTORY_H

#include "npcs.h"
#include <memory>
#include <string>

class NPCFactory {
public:
    static std::unique_ptr<NPC> createNPC(const std::string& type, 
                                         const std::string& name, 
                                         float x, float y);
    
    static std::unique_ptr<NPC> createNPCFromString(const std::string& data);
    static std::string serializeNPC(const NPC& npc);
};

#endif
