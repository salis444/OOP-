#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "Field.h"
#include "Player.h"
#include "Enemy.h"
#include "TurnManager.h"
#include "GameExceptions.h"
#include <vector>
#include <memory>
#include <string>

class GameSession {
public:
    GameSession(int width, int height, int enemyCount);
    ~GameSession() = default;
    
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
    
    GameSession(GameSession&& other) noexcept;
    GameSession& operator=(GameSession&& other) noexcept;
    
    void run();
    
    const Field& getField() const { return field; }
    const Player& getPlayer() const { return player; }
    Player& getPlayer() { return player; }
    const std::vector<std::shared_ptr<Enemy>>& getEnemies() const { return enemies; }
    int getEnemiesDefeated() const { return enemiesDefeated; }
    bool isGameOver() const { return gameOver; }
    
    void addEnemy(std::shared_ptr<Enemy> enemy);
    void setEnemiesDefeated(int count) { enemiesDefeated = count; }
    void setGameOver(bool over) { gameOver = over; }
    
private:
    Field field;
    Player player;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<std::shared_ptr<ITarget>> allTargets;
    std::vector<std::unique_ptr<ISpell>> spellDeck;
    TurnManager turnManager;
    
    bool gameOver;
    int enemyCount;
    int enemiesDefeated;
    
    struct PlayerAction {
        enum Type { MOVE, CAST_SPELL, SAVE, LOAD, QUIT, INVALID } type;
        
        char direction;
        int spellIndex;
        int targetX;
        int targetY;
        std::string filename;
        
        PlayerAction() : type(INVALID), direction(0), spellIndex(-1), targetX(0), targetY(0) {}
        
        static PlayerAction makeMove(char dir) {
            PlayerAction action;
            action.type = MOVE;
            action.direction = dir;
            return action;
        }
        
        static PlayerAction makeCast(int index, int x, int y) {
            PlayerAction action;
            action.type = CAST_SPELL;
            action.spellIndex = index;
            action.targetX = x;
            action.targetY = y;
            return action;
        }
        
        static PlayerAction makeSave(const std::string& fname) {
            PlayerAction action;
            action.type = SAVE;
            action.filename = fname;
            return action;
        }
        
        static PlayerAction makeLoad(const std::string& fname) {
            PlayerAction action;
            action.type = LOAD;
            action.filename = fname;
            return action;
        }
        
        static PlayerAction makeQuit() {
            PlayerAction action;
            action.type = QUIT;
            return action;
        }
        
        static PlayerAction makeInvalid() {
            return PlayerAction();
        }
    };
    
    void initializeSpellDeck();
    void spawnEnemies();
    void updateTargetsList();
    
    PlayerAction getPlayerAction();
    void processPlayerAction(const PlayerAction& action);
    void processMovement(char direction);
    bool processSpellCast(int spellIndex, int targetX, int targetY);
    void processSave(const std::string& filename);
    void processLoad(const std::string& filename);
    
    void processEnemiesTurn();
    void handleEnemyAttack(Enemy& enemy);
    void handlePlayerAttack(const Point& enemyPosition);
    bool isLevelComplete() const;
    void resetLevel();
    void resetGame();
    
    void render() const;
    void showHelp() const;
    
    Point getDirectionOffset(char direction) const;
    bool isValidDirection(char direction) const;
};

#endif