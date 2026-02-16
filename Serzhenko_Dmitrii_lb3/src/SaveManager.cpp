#include "SaveManager.h"
#include "GameSession.h"
#include "Player.h"
#include "Enemy.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

SaveManager::SaveManager(const std::string& filename) : filename(filename) {
    if (filename.find('/') == std::string::npos && filename.find('\\') == std::string::npos) {
        this->filename = "saves/" + filename;
    }
}

SaveManager::~SaveManager() {
    if (file.is_open()) {
        file.close();
    }
}

SaveManager::SaveManager(SaveManager&& other) noexcept
    : file(std::move(other.file)), filename(std::move(other.filename)) {}

SaveManager& SaveManager::operator=(SaveManager&& other) noexcept {
    if (this != &other) {
        if (file.is_open()) {
            file.close();
        }
        file = std::move(other.file);
        filename = std::move(other.filename);
    }
    return *this;
}

void SaveManager::checkFileGood() const {
    if (!file.good()) {
        throw FileAccessException(filename);
    }
}

void SaveManager::writeInt(int value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    checkFileGood();
}

int SaveManager::readInt() {
    int value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (file.gcount() != sizeof(value)) {
        throw CorruptedSaveException(filename);
    }
    return value;
}

void SaveManager::writeBool(bool value) {
    char byte = value ? 1 : 0;
    file.write(&byte, 1);
    checkFileGood();
}

bool SaveManager::readBool() {
    char byte;
    file.read(&byte, 1);
    if (file.gcount() != 1) {
        throw CorruptedSaveException(filename);
    }
    return byte != 0;
}

void SaveManager::writeString(const std::string& str) {
    int length = static_cast<int>(str.length());
    writeInt(length);
    file.write(str.c_str(), length);
    checkFileGood();
}

std::string SaveManager::readString() {
    int length = readInt();
    if (length < 0 || length > 10000) {
        throw CorruptedSaveException(filename);
    }
    
    std::vector<char> buffer(length + 1);
    file.read(buffer.data(), length);
    if (file.gcount() != length) {
        throw CorruptedSaveException(filename);
    }
    buffer[length] = '\0';
    return std::string(buffer.data());
}

void SaveManager::writeHeader() {
    writeString("GAME_SAVE_V1");
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    writeString(ss.str());
}

void SaveManager::readHeader() {
    std::string header = readString();
    if (header != "GAME_SAVE_V1") {
        throw CorruptedSaveException(filename);
    }
    readString();
}

void SaveManager::saveGame(const GameSession& game) {
    fs::create_directories("saves");
    
    file.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        throw FileAccessException(filename);
    }
    
    try {
        writeHeader();
        
        // Сохраняем размеры поля
        writeInt(game.getField().getWidth());
        writeInt(game.getField().getHeight());
        
        // Сохраняем данные игрока
        const Player& player = game.getPlayer();
        writeInt(player.getHealth());
        writeInt(player.getMaxHealth());
        writeInt(player.getDamage());
        writeInt(player.getMana());
        writeInt(player.getMaxMana());
        writeInt(player.getScore());
        writeInt(player.getPosition().getX());
        writeInt(player.getPosition().getY());
        
        // Сохраняем заклинания в руке
        const Hand& hand = player.getHand();
        writeInt(hand.getSize());
        writeInt(hand.getCapacity());
        
        for (int i = 0; i < hand.getSize(); ++i) {
            const ISpell* spell = hand.getSpell(i);
            writeString(spell->getName());
            writeInt(spell->getManaCost());
            writeInt(spell->getCastRange());
            
            if (auto directSpell = dynamic_cast<const DirectDamageSpell*>(spell)) {
                writeString("DirectDamageSpell");
                writeInt(directSpell->getDamage());
            } else if (auto areaSpell = dynamic_cast<const AreaDamageSpell*>(spell)) {
                writeString("AreaDamageSpell");
                writeInt(areaSpell->getDamage());
                writeInt(areaSpell->getAreaSize());
            }
        }
        
        // Сохраняем врагов
        const auto& enemies = game.getEnemies();
        writeInt(static_cast<int>(enemies.size()));
        
        for (const auto& enemy : enemies) {
            writeInt(enemy->getHealth());
            writeInt(enemy->getDamage());
            writeInt(enemy->getPosition().getX());
            writeInt(enemy->getPosition().getY());
            writeBool(enemy->isAlive());
        }
        
        // Сохраняем статистику
        writeInt(game.getEnemiesDefeated());
        writeBool(game.isGameOver());
        
        file.close();
        std::cout << "Игра сохранена в файл: " << filename << std::endl;
        
    } catch (const std::exception& e) {
        file.close();
        throw SaveException(e.what());
    }
}

std::unique_ptr<GameSession> SaveManager::loadGame() {
    if (!fs::exists(filename)) {
        throw FileAccessException(filename);
    }
    
    file.open(filename, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        throw FileAccessException(filename);
    }
    
    try {
        readHeader();
        
        // Загружаем размеры поля
        int width = readInt();
        int height = readInt();
        
        if (width < 10 || width > 25 || height < 10 || height > 25) {
            throw CorruptedSaveException(filename);
        }
        
        // Создаем новую игровую сессию с нулевым количеством врагов
        auto game = std::make_unique<GameSession>(width, height, 0);
        
        // Загружаем данные игрока
        int health = readInt();
        int maxHealth = readInt();
        int damage = readInt();
        int mana = readInt();
        int maxMana = readInt();
        int score = readInt();
        int playerX = readInt();
        int playerY = readInt();
        
        Player& player = game->getPlayer();
        player.setHealth(health);
        player.setMaxHealth(maxHealth);
        player.setDamage(damage);
        player.setMana(mana);
        player.setMaxMana(maxMana);
        player.setScore(score);
        player.setPosition(Point(playerX, playerY));
        
        // Очищаем руку перед загрузкой
        while (player.getHand().getSize() > 0) {
            player.getHand().removeSpell(0);
        }
        
        // Загружаем заклинания
        int handSize = readInt();
        int handCapacity = readInt();
        
        for (int i = 0; i < handSize; ++i) {
            std::string spellName = readString();
            int manaCost = readInt();
            int castRange = readInt();
            std::string spellType = readString();
            
            std::unique_ptr<ISpell> spell;
            
            if (spellType == "DirectDamageSpell") {
                int dmg = readInt();
                spell = std::make_unique<DirectDamageSpell>(spellName, dmg, manaCost, castRange);
            } else if (spellType == "AreaDamageSpell") {
                int dmg = readInt();
                int areaSize = readInt();
                spell = std::make_unique<AreaDamageSpell>(spellName, dmg, manaCost, castRange, areaSize);
            } else {
                throw CorruptedSaveException(filename);
            }
            
            player.getHand().addSpell(std::move(spell));
        }
        
        // Загружаем врагов
        int enemyCount = readInt();
        
        for (int i = 0; i < enemyCount; ++i) {
            int enemyHealth = readInt();
            int enemyDamage = readInt();
            int enemyX = readInt();
            int enemyY = readInt();
            bool enemyAlive = readBool();
            
            if (enemyAlive) {
                auto enemy = std::make_shared<Enemy>(enemyHealth, enemyDamage, Point(enemyX, enemyY));
                game->addEnemy(enemy);
            }
        }
        
        // Загружаем статистику
        int enemiesDefeated = readInt();
        bool gameOver = readBool();
        
        game->setEnemiesDefeated(enemiesDefeated);
        game->setGameOver(gameOver);
        
        file.close();
        return game;
        
    } catch (const std::exception& e) {
        file.close();
        throw LoadException(e.what());
    }
}

bool SaveManager::saveExists(const std::string& filename) {
    std::string fullPath = filename;
    if (fullPath.find('/') == std::string::npos && fullPath.find('\\') == std::string::npos) {
        fullPath = "saves/" + filename;
    }
    return fs::exists(fullPath);
}

std::vector<std::string> SaveManager::listSaves() {
    std::vector<std::string> saves;
    std::string saveDir = "saves";
    
    if (!fs::exists(saveDir)) {
        fs::create_directory(saveDir);
        return saves;
    }
    
    for (const auto& entry : fs::directory_iterator(saveDir)) {
        if (entry.is_regular_file()) {
            saves.push_back(entry.path().filename().string());
        }
    }
    
    return saves;
}