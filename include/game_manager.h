#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "dungeon_editor.h"
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <random>
#include <unordered_set>

struct ThreadBattle {
    NPC* attacker;
    NPC* defender;
};

class GameManager {
private:
    // Константы
    static constexpr int MAP_WIDTH = 100;
    static constexpr int MAP_HEIGHT = 100;
    static constexpr int INITIAL_NPC_COUNT = 50;
    static constexpr int GAME_DURATION = 30; // секунды
    
    // Потоки и синхронизация
    std::thread movementThread;
    std::thread battleThread;
    std::atomic<bool> running{false};
    std::atomic<bool> gameOver{false};
    
    // Мьютексы
    mutable std::shared_mutex npcMutex;  // Для доступа к NPC
    mutable std::mutex printMutex;       // Для вывода в консоль
    std::mutex battleQueueMutex;         // Для очереди битв
    
    // Очередь битв и условная переменная
    std::queue<ThreadBattle> battleQueue;
    std::condition_variable battleCV;
    
    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> diceDist;
    
    // Редактор с NPC
    DungeonEditor editor;
    
    // Переменная для отслеживания времени вывода
    int lastPrintedSecond;
    
public:
    GameManager();
    ~GameManager();
    
    void run();
    void stop();
    
private:
    void initializeNPCs();
    void movementWorker();
    void battleWorker();
    void printMap();
    void printSurvivors();
};

#endif
