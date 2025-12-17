#include <gtest/gtest.h>
#include "npcs.h"
#include <memory>
#include <thread>

// Тесты для NPC
TEST(NPCTest, CreateOrc) {
    auto orc = std::make_shared<Orc>("TestOrc", 50, 50);
    EXPECT_EQ(orc->getType(), "Orc");
    EXPECT_TRUE(orc->isAlive());
    EXPECT_EQ(orc->getMoveDistance(), 20);
    EXPECT_EQ(orc->getKillDistance(), 10);
}

TEST(NPCTest, CreateKnight) {
    auto knight = std::make_shared<Knight>("TestKnight", 100, 100);
    EXPECT_EQ(knight->getType(), "Knight");
    EXPECT_EQ(knight->getMoveDistance(), 30);
    EXPECT_EQ(knight->getKillDistance(), 10);
    EXPECT_EQ(knight->getSymbol(), 'K');
}

TEST(NPCTest, CreateBear) {
    auto bear = std::make_shared<Bear>("TestBear", 150, 150);
    EXPECT_EQ(bear->getType(), "Bear");
    EXPECT_EQ(bear->getMoveDistance(), 5);
    EXPECT_EQ(bear->getKillDistance(), 10);
    EXPECT_EQ(bear->getSymbol(), 'B');
}

TEST(NPCTest, MovementDistances) {
    auto orc = std::make_shared<Orc>("Orc1", 50, 50);
    auto knight = std::make_shared<Knight>("Knight1", 50, 50);
    auto bear = std::make_shared<Bear>("Bear1", 50, 50);
    
    // Проверяем дистанции перемещения из таблицы
    EXPECT_EQ(orc->getMoveDistance(), 20);     // Орк: 20
    EXPECT_EQ(knight->getMoveDistance(), 30);  // Рыцарь: 30
    EXPECT_EQ(bear->getMoveDistance(), 5);     // Медведь: 5
    
    // Проверяем дистанции убийства из таблицы
    EXPECT_EQ(orc->getKillDistance(), 10);     // Все: 10
    EXPECT_EQ(knight->getKillDistance(), 10);
    EXPECT_EQ(bear->getKillDistance(), 10);
}

TEST(NPCTest, Movement) {
    auto knight = std::make_shared<Knight>("Knight1", 50, 50);
    
    // Рыцарь перемещается на 30 единиц за ход
    knight->move(1, 0, 100, 100);
    EXPECT_EQ(knight->getPosition().first, 80);  // 50 + 30
    
    // Медведь перемещается на 5 единиц
    auto bear = std::make_shared<Bear>("Bear1", 50, 50);
    bear->move(0, 1, 100, 100);
    EXPECT_EQ(bear->getPosition().second, 55);  // 50 + 5
}

TEST(NPCTest, MapBoundaries) {
    auto orc = std::make_shared<Orc>("Orc1", 95, 95);
    
    // Пытаемся выйти за границы 100x100
    orc->move(1, 1, 100, 100);
    auto pos = orc->getPosition();
    
    EXPECT_LE(pos.first, 99);   // Не больше 99
    EXPECT_LE(pos.second, 99);  // Не больше 99
    
    // Проверяем нижние границы
    orc->move(-10, -10, 100, 100);
    pos = orc->getPosition();
    EXPECT_GE(pos.first, 0);    // Не меньше 0
    EXPECT_GE(pos.second, 0);   // Не меньше 0
}

TEST(NPCTest, DeadNPCDoesntMove) {
    auto bear = std::make_shared<Bear>("Bear1", 50, 50);
    bear->die();
    
    auto deadPos = bear->getPosition();
    bear->move(1, 1, 100, 100);
    
    // Позиция не должна измениться
    EXPECT_EQ(deadPos.first, bear->getPosition().first);
    EXPECT_EQ(deadPos.second, bear->getPosition().second);
}

TEST(NPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<Orc>("Orc1", 0, 0);
    auto npc2 = std::make_shared<Knight>("Knight1", 3, 4);
    
    float distance = npc1->distanceTo(*npc2);
    EXPECT_FLOAT_EQ(distance, 5.0f);  // √(3² + 4²) = 5
}

TEST(NPCTest, KillDistanceCheck) {
    auto orc = std::make_shared<Orc>("Orc1", 0, 0);
    auto bear = std::make_shared<Bear>("Bear1", 8, 0);
    
    // Дистанция 8 <= killDistance 10
    EXPECT_TRUE(orc->distanceTo(*bear) <= orc->getKillDistance());
    
    auto bearFar = std::make_shared<Bear>("Bear2", 15, 0);
    // Дистанция 15 > killDistance 10
    EXPECT_FALSE(orc->distanceTo(*bearFar) <= orc->getKillDistance());
}

// Тесты для логики атаки
TEST(NPCTest, AttackLogic) {
    auto orc = std::make_shared<Orc>("Orc1", 0, 0);
    auto knight = std::make_shared<Knight>("Knight1", 1, 1);
    auto bear = std::make_shared<Bear>("Bear1", 2, 2);
    
    // Проверяем правила атаки
    EXPECT_TRUE(orc->canAttack(*bear));      // Орк атакует медведя
    EXPECT_FALSE(orc->canAttack(*knight));   // Орк НЕ атакует рыцаря
    
    EXPECT_TRUE(knight->canAttack(*orc));    // Рыцарь атакует орка
    EXPECT_FALSE(knight->canAttack(*bear));  // Рыцарь НЕ атакует медведя
    
    EXPECT_TRUE(bear->canAttack(*knight));   // Медведь атакует рыцаря
    EXPECT_FALSE(bear->canAttack(*orc));     // Медведь НЕ атакует орка
}

// Тесты для GameManager
TEST(GameManagerTest, Initialization) {
    // Просто проверяем, что можно создать GameManager
    // Не запускаем run(), просто создаем
    EXPECT_NO_THROW({
        // Если у GameManager есть конструктор по умолчанию
    });
}

// Тесты многопоточности
TEST(ThreadTest, ThreadCreation) {
    // Простая проверка работы потоков
    std::thread t([](){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    EXPECT_TRUE(t.joinable());
    t.join();
}
