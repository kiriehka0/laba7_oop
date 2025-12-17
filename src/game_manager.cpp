#include "game_manager.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include <unordered_set>

GameManager::GameManager() : gen(rd()), diceDist(1, 6), lastPrintedSecond(-1) {
    initializeNPCs();
}

GameManager::~GameManager() {
    stop();
}

void GameManager::initializeNPCs() {
    std::uniform_int_distribution<> typeDist(0, 2);
    std::uniform_int_distribution<> xDist(0, MAP_WIDTH - 1);
    std::uniform_int_distribution<> yDist(0, MAP_HEIGHT - 1);
    
    for (int i = 0; i < INITIAL_NPC_COUNT; ++i) {
        int type = typeDist(gen);
        int x = xDist(gen);
        int y = yDist(gen);
        
        std::string name = "NPC_" + std::to_string(i);
        
        switch (type) {
            case 0:
                editor.addNPC("Orc", name, x, y);
                break;
            case 1:
                editor.addNPC("Knight", name, x, y);
                break;
            case 2:
                editor.addNPC("Bear", name, x, y);
                break;
        }
    }
}

void GameManager::printMap() {
    // Получаем позиции живых NPC
    std::vector<NPC*> aliveNPCs;
    {
        std::shared_lock lock(npcMutex);
        aliveNPCs = editor.getAliveNPCs();
    }
    
    // Создаем полную карту 100x100
    std::vector<std::vector<char>> fullMap(MAP_HEIGHT, std::vector<char>(MAP_WIDTH, '.'));
    
    // Счетчики для статистики наложения
    std::map<std::pair<int, int>, int> cellCounts;
    std::map<std::pair<int, int>, char> cellSymbols;
    
    // Заполняем карту NPC
    for (auto npc : aliveNPCs) {
        auto pos = npc->getPosition();
        if (pos.first >= 0 && pos.first < MAP_WIDTH && 
            pos.second >= 0 && pos.second < MAP_HEIGHT) {
            
            std::pair<int, int> cell = {pos.second, pos.first};
            cellCounts[cell]++;
            cellSymbols[cell] = npc->getSymbol();
            
            if (cellCounts[cell] == 1) {
                fullMap[pos.second][pos.first] = npc->getSymbol();
            } else {
                // Если в клетке более одного NPC, показываем цифру количества
                fullMap[pos.second][pos.first] = '0' + std::min(cellCounts[cell], 9);
            }
        }
    }
    
    // Находим область с NPC для отображения
    int minX = MAP_WIDTH, maxX = -1;
    int minY = MAP_HEIGHT, maxY = -1;
    
    for (auto npc : aliveNPCs) {
        auto pos = npc->getPosition();
        if (pos.first < minX) minX = pos.first;
        if (pos.first > maxX) maxX = pos.first;
        if (pos.second < minY) minY = pos.second;
        if (pos.second > maxY) maxY = pos.second;
    }
    
    // Если NPC нет, показываем центр карты
    if (aliveNPCs.empty() || minX > maxX || minY > maxY) {
        minX = 0;
        maxX = MAP_WIDTH - 1;
        minY = 0;
        maxY = MAP_HEIGHT - 1;
    }
    
    // Добавляем отступы
    int padding = 5;
    minX = std::max(0, minX - padding);
    maxX = std::min(MAP_WIDTH - 1, maxX + padding);
    minY = std::max(0, minY - padding);
    maxY = std::min(MAP_HEIGHT - 1, maxY + padding);
    
    // Ограничиваем размер отображаемой области для читаемости
    const int MAX_DISPLAY_WIDTH = 80;
    const int MAX_DISPLAY_HEIGHT = 40;
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    if (width > MAX_DISPLAY_WIDTH) {
        int centerX = (minX + maxX) / 2;
        minX = std::max(0, centerX - MAX_DISPLAY_WIDTH / 2);
        maxX = std::min(MAP_WIDTH - 1, minX + MAX_DISPLAY_WIDTH - 1);
        width = maxX - minX + 1;
    }
    
    if (height > MAX_DISPLAY_HEIGHT) {
        int centerY = (minY + maxY) / 2;
        minY = std::max(0, centerY - MAX_DISPLAY_HEIGHT / 2);
        maxY = std::min(MAP_HEIGHT - 1, minY + MAX_DISPLAY_HEIGHT - 1);
        height = maxY - minY + 1;
    }
    
    // Выводим карту
    std::lock_guard<std::mutex> lock(printMutex);
    
    std::cout << "\n=== GAME MAP ===" << std::endl;
    std::cout << "Time: " << lastPrintedSecond << "/" << GAME_DURATION << "s" << std::endl;
    std::cout << "Showing area: X[" << minX << "-" << maxX << "] Y[" << minY << "-" << maxY << "]" << std::endl;
    std::cout << "Full map: 100x100, Alive NPCs: " << aliveNPCs.size() << std::endl;
    
    // Выводим координаты X (десятки и единицы)
    std::cout << "    ";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << (x / 10) % 10;
    }
    std::cout << std::endl;
    
    std::cout << "    ";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << x % 10;
    }
    std::cout << std::endl;
    
    // Выводим разделитель
    std::cout << "   +";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << "-";
    }
    std::cout << "+" << std::endl;
    
    // Выводим карту с координатами Y
    for (int y = minY; y <= maxY; ++y) {
        std::cout << std::setw(2) << y << " |";
        for (int x = minX; x <= maxX; ++x) {
            std::cout << fullMap[y][x];
        }
        std::cout << "| " << std::setw(2) << y << std::endl;
    }
    
    // Выводим нижний разделитель
    std::cout << "   +";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << "-";
    }
    std::cout << "+" << std::endl;
    
    // Выводим координаты X снизу
    std::cout << "    ";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << (x / 10) % 10;
    }
    std::cout << std::endl;
    
    std::cout << "    ";
    for (int x = minX; x <= maxX; ++x) {
        std::cout << x % 10;
    }
    std::cout << std::endl;
    
    // Статистика
    int alive = aliveNPCs.size();
    int orcs = 0, knights = 0, bears = 0;
    int overlappingCells = 0;
    
    for (auto npc : aliveNPCs) {
        auto type = npc->getType();
        if (type == "Orc") orcs++;
        else if (type == "Knight") knights++;
        else if (type == "Bear") bears++;
    }
    
    // Подсчитываем клетки с наложением 
    for (const auto& entry : cellCounts) {
        if (entry.second > 1) {
            overlappingCells++;
        }
    }
    
    std::cout << "\n=== STATISTICS ===" << std::endl;
    std::cout << "Alive NPCs: " << alive << std::endl;
    std::cout << "  Orcs: " << orcs << " (O)" << std::endl;
    std::cout << "  Knights: " << knights << " (K)" << std::endl;
    std::cout << "  Bears: " << bears << " (B)" << std::endl;
    std::cout << "Cells with multiple NPCs: " << overlappingCells << " (shown as numbers 2-9)" << std::endl;
    std::cout << "=============================" << std::endl;
}

void GameManager::printSurvivors() {
    std::vector<NPC*> aliveNPCs;
    {
        std::shared_lock lock(npcMutex);
        aliveNPCs = editor.getAliveNPCs();
    }
    
    std::lock_guard<std::mutex> printLock(printMutex);
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "=== GAME OVER ===" << std::endl;
    std::cout << "Game duration: " << GAME_DURATION << " seconds" << std::endl;
    std::cout << "Total survivors: " << aliveNPCs.size() << " out of " << INITIAL_NPC_COUNT << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    if (aliveNPCs.empty()) {
        std::cout << "No survivors! All NPCs have been killed." << std::endl;
        return;
    }
    
    // Подсчет по типам
    int orcs = 0, knights = 0, bears = 0;
    for (auto npc : aliveNPCs) {
        auto type = npc->getType();
        if (type == "Orc") orcs++;
        else if (type == "Knight") knights++;
        else if (type == "Bear") bears++;
    }
    
    std::cout << "\nSurvivors by type:" << std::endl;
    std::cout << "  Orcs: " << orcs << std::endl;
    std::cout << "  Knights: " << knights << std::endl;
    std::cout << "  Bears: " << bears << std::endl;
    
    // Группируем по позициям
    std::map<std::pair<int, int>, std::vector<NPC*>> npcsByPosition;
    for (auto npc : aliveNPCs) {
        auto pos = npc->getPosition();
        npcsByPosition[pos].push_back(npc);
    }
    
    std::cout << "\nSurvivor positions:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << std::left << std::setw(15) << "Position" 
              << std::setw(10) << "Count" 
              << std::setw(35) << "NPCs" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& entry : npcsByPosition) {
        const auto& pos = entry.first;
        const auto& npcs = entry.second;
        
        std::stringstream posStr;
        posStr << "(" << pos.first << "," << pos.second << ")";
        
        std::stringstream npcList;
        for (size_t i = 0; i < npcs.size(); ++i) {
            if (i > 0) npcList << ", ";
            npcList << npcs[i]->getType() << " " << npcs[i]->getName();
        }
        
        std::cout << std::left << std::setw(15) << posStr.str()
                  << std::setw(10) << npcs.size()
                  << std::setw(35) << npcList.str() << std::endl;
    }
    
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "\nTotal positions occupied: " << npcsByPosition.size() << std::endl;
    
    // Показываем клетки с несколькими NPC
    int multiNpcCells = 0;
    for (const auto& entry : npcsByPosition) {
        if (entry.second.size() > 1) {
            multiNpcCells++;
        }
    }
    
    if (multiNpcCells > 0) {
        std::cout << "Cells with multiple NPCs: " << multiNpcCells << std::endl;
    }
}

void GameManager::run() {
    running = true;
    lastPrintedSecond = -1;
    
    {
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "=== NPC BATTLE SIMULATION (Lab 7) ===" << std::endl;
        std::cout << "Map size: " << MAP_WIDTH << "x" << MAP_HEIGHT << std::endl;
        std::cout << "Game duration: " << GAME_DURATION << " seconds" << std::endl;
        std::cout << "Initial NPCs: " << INITIAL_NPC_COUNT << std::endl;
        std::cout << "NPC types: Orc (O), Knight (K), Bear (B)" << std::endl;
        std::cout << "Movement distances: Orc=20, Knight=30, Bear=5" << std::endl;
        std::cout << "Kill distance: 10 for all NPCs" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "\nInitial NPC positions are being generated..." << std::endl;
    }
    
    // Даем немного времени на инициализацию
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Выводим начальную карту (время 0)
    lastPrintedSecond = 0;
    printMap();
    
    auto movementLambda = [this]() {
        std::uniform_int_distribution<> dirDist(-1, 1);
        
        while (running) {
            // Получаем список живых NPC
            std::vector<NPC*> aliveNPCs;
            {
                std::shared_lock lock(npcMutex);
                aliveNPCs = editor.getAliveNPCs();
            }
            
            // Перемещаем живых NPC
            for (auto npc : aliveNPCs) {
                npc->move(dirDist(gen), dirDist(gen), MAP_WIDTH, MAP_HEIGHT);
            }
            
            // Множество уже проверенных пар для избежания дублирования битв
            std::unordered_set<std::string> checkedPairs;
            
            // Проверяем столкновения
            for (size_t i = 0; i < aliveNPCs.size(); ++i) {
                NPC* npc1 = aliveNPCs[i];
                if (!npc1->isAlive()) continue;
                
                for (size_t j = i + 1; j < aliveNPCs.size(); ++j) {
                    NPC* npc2 = aliveNPCs[j];
                    if (!npc2->isAlive()) continue;
                    
                    // Создаем уникальный идентификатор пары
                    std::string pairId = std::to_string(reinterpret_cast<uintptr_t>(npc1)) + 
                                         "-" + 
                                         std::to_string(reinterpret_cast<uintptr_t>(npc2));
                    
                    if (checkedPairs.find(pairId) != checkedPairs.end()) {
                        continue; // Уже проверяли эту пару
                    }
                    checkedPairs.insert(pairId);
                    
                    double distance = npc1->distanceTo(*npc2);
                    
                    // Проверяем дистанцию убийства (10 для всех)
                    if (distance <= 10.0) {
                        // Проверяем, могут ли они атаковать друг друга
                        bool canAttack1to2 = npc1->canAttack(*npc2);
                        bool canAttack2to1 = npc2->canAttack(*npc1);
                        
                        if (canAttack1to2 || canAttack2to1) {
                            // Определяем атакующего и защищающегося
                            ThreadBattle battle;
                            
                            if (canAttack1to2 && !canAttack2to1) {
                                battle.attacker = npc1;
                                battle.defender = npc2;
                            } else if (!canAttack1to2 && canAttack2to1) {
                                battle.attacker = npc2;
                                battle.defender = npc1;
                            } else {
                                // Оба могут атаковать - выбираем случайно
                                if (dirDist(gen) > 0) {
                                    battle.attacker = npc1;
                                    battle.defender = npc2;
                                } else {
                                    battle.attacker = npc2;
                                    battle.defender = npc1;
                                }
                            }
                            
                            {
                                std::lock_guard<std::mutex> lock(battleQueueMutex);
                                battleQueue.push(battle);
                            }
                            battleCV.notify_one();
                        }
                    }
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    
    auto battleLambda = [this]() {
        while (running) {
            ThreadBattle battle{nullptr, nullptr};
            bool hasBattle = false;
            
            // Берем битву из очереди
            {
                std::unique_lock<std::mutex> lock(battleQueueMutex);
                if (battleCV.wait_for(lock, std::chrono::milliseconds(100), 
                    [this]() { return !battleQueue.empty(); })) {
                    
                    battle = battleQueue.front();
                    battleQueue.pop();
                    hasBattle = true;
                }
            }
            
            // Обрабатываем битву
            if (hasBattle && battle.attacker && battle.defender) {
                // Проверяем, что оба еще живы и могут сражаться
                if (!battle.attacker->isAlive() || !battle.defender->isAlive()) {
                    continue;
                }
                
                if (!battle.attacker->canAttack(*battle.defender)) {
                    continue;
                }
                
                // Бросаем 6-гранные кубики
                int attackRoll = diceDist(gen);   // 1-6
                int defenseRoll = diceDist(gen);  // 1-6
                
                std::string battleResult;
                
                if (attackRoll > defenseRoll) {
                    // Атака успешна - убиваем защитника
                    {
                        std::lock_guard<std::mutex> npcLock(printMutex);
                        battle.defender->die();
                    }
                    battleResult = " -> " + battle.defender->getName() + " KILLED!";
                } else {
                    battleResult = " -> " + battle.defender->getName() + " DEFENDED!";
                }
                
                // Выводим результат битвы в одну строку
                {
                    std::lock_guard<std::mutex> printLock(printMutex);
                    std::cout << "Battle: " << battle.attacker->getType() 
                              << " " << battle.attacker->getName()
                              << " [" << attackRoll << "] vs "
                              << battle.defender->getType()
                              << " " << battle.defender->getName()
                              << " [" << defenseRoll << "]"
                              << battleResult << std::endl;
                }
            }
        }
    };
    
    movementThread = std::thread(movementLambda);
    battleThread = std::thread(battleLambda);
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Основной цикл
    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        int currentSecond = elapsed.count();
        
        if (currentSecond >= GAME_DURATION) {
            running = false;
            break;
        }
        
        // Выводим карту и статистику каждую секунду
        if (currentSecond > lastPrintedSecond) {
            lastPrintedSecond = currentSecond;
            printMap();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Завершаем игру
    stop();
    
    // Небольшая задержка для завершения всех битв
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    printSurvivors();
}

void GameManager::stop() {
    running = false;
    battleCV.notify_all();
    
    if (movementThread.joinable()) {
        movementThread.join();
    }
    if (battleThread.joinable()) {
        battleThread.join();
    }
}
