#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "GameExceptions.h"

class GameSession;

class SaveManager {
private:
    std::fstream file;
    std::string filename;
    
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;
    
    void writeString(const std::string& str);
    void writeInt(int value);
    void writeBool(bool value);
    
    std::string readString();
    int readInt();
    bool readBool();
    
    void checkFileGood() const;
    void writeHeader();
    void readHeader();

public:
    explicit SaveManager(const std::string& filename);
    ~SaveManager();
    
    SaveManager(SaveManager&& other) noexcept;
    SaveManager& operator=(SaveManager&& other) noexcept;
    
    void saveGame(const GameSession& game);
    std::unique_ptr<GameSession> loadGame();
    
    static bool saveExists(const std::string& filename);
    static std::vector<std::string> listSaves();
};

#endif