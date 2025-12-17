#include "../include/observer.h"
#include <fstream>
#include <iostream>
#include <algorithm> 

FileLogger::FileLogger(const std::string& filename) : filename(filename) {}

void FileLogger::onBattleResult(const std::string& result) {
    std::ofstream file(filename, std::ios_base::app);
    if (file.is_open()) {
        file << result << std::endl;
    }
}

void ConsoleLogger::onBattleResult(const std::string& result) {
    std::cout << result << std::endl;
}

void BattleNotifier::addObserver(std::shared_ptr<BattleObserver> observer) {
    observers.push_back(observer);
}

void BattleNotifier::removeObserver(std::shared_ptr<BattleObserver> observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void BattleNotifier::notifyObservers(const std::string& result) {
    for (auto& observer : observers) {
        observer->onBattleResult(result);
    }
}
