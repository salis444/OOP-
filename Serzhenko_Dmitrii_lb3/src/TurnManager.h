#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <iostream>

class TurnManager {
public:
    enum class Phase {
        PLAYER_TURN,
        ENEMIES_TURN,
        LEVEL_COMPLETE,
        GAME_OVER
    };
    
    TurnManager();
    
    void nextPhase();
    bool isPlayerTurn() const { return currentPhase == Phase::PLAYER_TURN; }
    bool isEnemiesTurn() const { return currentPhase == Phase::ENEMIES_TURN; }
    bool isLevelComplete() const { return currentPhase == Phase::LEVEL_COMPLETE; }
    bool isGameOver() const { return currentPhase == Phase::GAME_OVER; }
    
    Phase getCurrentPhase() const { return currentPhase; }
    void setPhase(Phase phase) { currentPhase = phase; }
    
    friend std::ostream& operator<<(std::ostream& os, const TurnManager& tm);

private:
    Phase currentPhase;
};

#endif