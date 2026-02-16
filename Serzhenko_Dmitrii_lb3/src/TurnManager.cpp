#include "TurnManager.h"

TurnManager::TurnManager() : currentPhase(Phase::PLAYER_TURN) {}

void TurnManager::nextPhase() {
    switch (currentPhase) {
        case Phase::PLAYER_TURN:
            currentPhase = Phase::ENEMIES_TURN;
            break;
        case Phase::ENEMIES_TURN:
            currentPhase = Phase::PLAYER_TURN;
            break;
        case Phase::LEVEL_COMPLETE:
        case Phase::GAME_OVER:
            break;
    }
}

std::ostream& operator<<(std::ostream& os, const TurnManager& tm) {
    switch (tm.currentPhase) {
        case TurnManager::Phase::PLAYER_TURN:
            os << "Ход игрока";
            break;
        case TurnManager::Phase::ENEMIES_TURN:
            os << "Ход врагов";
            break;
        case TurnManager::Phase::LEVEL_COMPLETE:
            os << "Уровень пройден";
            break;
        case TurnManager::Phase::GAME_OVER:
            os << "Игра окончена";
            break;
    }
    return os;
}