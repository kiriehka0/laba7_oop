cmake_minimum_required(VERSION 3.10)
project(Laba7_Multithread)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим пакеты
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# Основные исходные файлы ДЛЯ ОСНОВНОЙ ПРОГРАММЫ
set(MAIN_SOURCES
    src/main.cpp
    src/dungeon_editor.cpp
    src/npcs.cpp
    src/observer.cpp
    src/factory.cpp
    src/game_manager.cpp
)

# Файлы БЕЗ main.cpp для тестов
set(TEST_SOURCES
    src/dungeon_editor.cpp
    src/npcs.cpp
    src/observer.cpp
    src/factory.cpp
    src/game_manager.cpp
)

# Заголовочные файлы
set(HEADERS
    include/dungeon_editor.h
    include/npcs.h
    include/observer.h
    include/factory.h
    include/game_manager.h
)

# Основная программа
add_executable(laba7 ${MAIN_SOURCES})

# Включаем директории
target_include_directories(laba7 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(laba7 
    Threads::Threads
)

# Тесты - используем TEST_SOURCES (БЕЗ src/main.cpp)
add_executable(all_tests
    tests/test_all.cpp
    ${TEST_SOURCES}
)

target_include_directories(all_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(all_tests 
    Threads::Threads
    GTest::gtest 
    GTest::gtest_main  # Эта библиотека содержит main() для тестов
)

# Включение тестирования
enable_testing()
add_test(NAME AllTests COMMAND all_tests)
