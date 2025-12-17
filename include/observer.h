#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <memory>
#include <vector>  

// Интерфейс Observer
class BattleObserver {
public:
    virtual ~BattleObserver() = default;
    virtual void onBattleResult(const std::string& result) = 0;
};

// Конкретные 
class FileLogger : public BattleObserver {
private:
    std::string filename;

public:
    FileLogger(const std::string& filename = "log.txt");
    void onBattleResult(const std::string& result) override;
};

class ConsoleLogger : public BattleObserver {
public:
    void onBattleResult(const std::string& result) override;
};

// Subject для управления 
class BattleNotifier {
private:
    std::vector<std::shared_ptr<BattleObserver>> observers;

public:
    void addObserver(std::shared_ptr<BattleObserver> observer);
    void removeObserver(std::shared_ptr<BattleObserver> observer);
    void notifyObservers(const std::string& result);
};

#endif
