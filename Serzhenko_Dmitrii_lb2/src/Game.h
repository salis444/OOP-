#ifndef GAME_H
#define GAME_H

#include "Field.h"
#include "Player.h"
#include "Enemy.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include <vector>
#include <memory>

class Game {
public:
    Game(int width, int height, int enemyCount);
    
    void run();
    
private:
    Field field;
    Player player;
    
    struct EnemyInfo {
        std::shared_ptr<Enemy> enemy;
        Point position;
    };
    std::vector<EnemyInfo> enemies;
    
    std::vector<std::shared_ptr<ITarget>> allTargets; 
    
    std::vector<std::unique_ptr<ISpell>> spellDeck;
    
    bool gameOver;
    int enemyCount;
    int enemiesDefeated;  
    
    void initializeSpellDeck();
    void spawnEnemies();
    void updateTargetsList();
    
    void processPlayerInput();
    bool processSpellCast();  
    void processMovement();
    void updateEnemies();
    void render();
    
    void handleEnemyAttack(Enemy& enemy);
    void handlePlayerAttack(const Point& enemyPosition);
    
    Point getTargetPositionFromInput() const;
    void checkForSpellRewards();
};

#endif