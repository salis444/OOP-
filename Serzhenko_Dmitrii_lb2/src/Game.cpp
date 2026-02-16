#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>

Game::Game(int width, int height, int enemyCount) 
    : field(width, height), gameOver(false), enemyCount(enemyCount), enemiesDefeated(0) {
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    int maxPossibleEnemies = width * height - 1; 
    if (enemyCount > maxPossibleEnemies) {
        std::cout << "Количество врагов (" << enemyCount 
                  << ") слишком велико для поля " << width << "x" << height 
                  << ". Установлено максимальное возможное: " << maxPossibleEnemies << std::endl;
        this->enemyCount = maxPossibleEnemies;
    }
    
    Point playerStartPos = field.getRandomEmptyCell();
    field.setCellType(playerStartPos, Cell::Type::PLAYER);
    field.setPlayerPosition(playerStartPos);
    player.setPosition(playerStartPos);
    
    initializeSpellDeck();
    spawnEnemies();
    updateTargetsList();
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "НАЧАЛО ИГРЫ" << std::endl;
    std::cout << std::string(50, '=') << "\n" << std::endl;
    std::cout << "Вы начинаете со случайным заклинанием!" << std::endl;
    player.getHand().drawRandomSpell(spellDeck);
    std::cout << "Уничтожьте 5 врагов, чтобы получить новое заклинание." << std::endl;
    std::cout << "Каждое заклинание стоит 6 маны." << std::endl;
    std::cout << "Заклинания многоразовые и остаются в руке после использования." << std::endl;
    std::cout << "\n" << std::string(50, '=') << "\n" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void Game::initializeSpellDeck() {

    spellDeck.push_back(std::make_unique<DirectDamageSpell>("Удар молнии", 30, 6, 3));
    spellDeck.push_back(std::make_unique<AreaDamageSpell>("Ледяной вихрь", 25, 6, 2, 2));
}

void Game::spawnEnemies() {
    for (int i = 0; i < enemyCount; ++i) {
        Point enemyPos = field.getRandomEmptyCell();
        field.setCellType(enemyPos, Cell::Type::ENEMY);
        
        auto enemy = std::make_shared<Enemy>(50, 5, enemyPos);
        enemies.push_back({enemy, enemyPos});
    }
}

void Game::updateTargetsList() {
    allTargets.clear();
    
    for (const auto& enemyInfo : enemies) {
        allTargets.push_back(enemyInfo.enemy);
    }
    
    DirectDamageSpell::setAvailableTargets(allTargets);
    AreaDamageSpell::setAvailableTargets(allTargets);
}

void Game::run() {
    while (!gameOver && player.isAlive()) {
        render();
        processPlayerInput();
        
        checkForSpellRewards();
        
        if (enemies.empty()) {
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "ВСЕ ВРАГИ ПОБЕЖДЕНЫ! ВЫ ВЫИГРАЛИ!" << std::endl;
            std::cout << std::string(50, '=') << "\n" << std::endl;
            break;
        }
        
        std::cout << "\n" << std::string(50, '-') << "\n" << std::endl;
    }
}

void Game::processPlayerInput() {
    std::cout << "ВЫБЕРИТЕ ДЕЙСТВИЕ:" << std::endl;
    std::cout << "1. Перемещение" << std::endl;
    std::cout << "2. Использовать заклинание" << std::endl;
    std::cout << "3. Показать заклинания в руке" << std::endl;
    std::cout << "\n Ваш выбор: ";
    
    int choice;
    std::cin >> choice;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n Неверный ввод! Пожалуйста, введите число 1, 2 или 3.\n" << std::endl;
        return;
    }
    
    bool playerMadeTurn = false; 
    
    switch (choice) {
        case 1:
            processMovement();
            playerMadeTurn = true; 
            break;
        case 2:
            if (processSpellCast()) {
                playerMadeTurn = true; 
            }
            break;
        case 3:
            
            player.getHand().display();
            std::cout << "\n Нажмите Enter чтобы продолжить...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            return; 
        default:
            std::cout << "\n Неверный выбор! Пожалуйста, выберите 1, 2 или 3.\n" << std::endl;
            return;
    }
    
    if (playerMadeTurn) {
        updateEnemies();
    }
}

void Game::processMovement() {
    std::cout << "\n ВВЕДИТЕ НАПРАВЛЕНИЕ:" << std::endl;
    std::cout << "w - вверх, s - вниз, a - влево, d - вправо" << std::endl;
    std::cout << "Удар по врагу: переместитесь на клетку с врагом" << std::endl;
    std::cout << "Направление: ";
    
    char input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    Point currentPos = player.getPosition();
    Point newPos = currentPos;
    
    switch (input) {
        case 'w': case 'W': newPos = currentPos + Point(0, -1); break;
        case 's': case 'S': newPos = currentPos + Point(0, 1); break;
        case 'a': case 'A': newPos = currentPos + Point(-1, 0); break;
        case 'd': case 'D': newPos = currentPos + Point(1, 0); break;
        default: 
            std::cout << "\n Неверный ввод! Используйте w, s, a или d.\n" << std::endl;
            return;
    }
    
    if (!field.isWithinBounds(newPos)) {
        std::cout << "\n Нельзя выйти за границы поля!\n" << std::endl;
        return;
    }
    
    Cell::Type cellType = field.getCellType(newPos);
    
    if (cellType == Cell::Type::ENEMY) {
        std::cout << "\n>>> ВЫ АТАКУЕТЕ ВРАГА! <<<\n" << std::endl;
        handlePlayerAttack(newPos);
    } else if (cellType == Cell::Type::EMPTY) {
        field.setCellType(currentPos, Cell::Type::EMPTY);
        field.setCellType(newPos, Cell::Type::PLAYER);
        field.setPlayerPosition(newPos);
        player.setPosition(newPos);
        std::cout << "\n Вы переместились на новую позицию.\n" << std::endl;
    }
}

bool Game::processSpellCast() {
    if (player.getHand().isEmpty()) {
        std::cout << "\n В вашей руке нет заклинаний!\n" << std::endl;
        return false;
    }
    
    player.getHand().display();
    std::cout << "\n Выберите заклинание (1-" << player.getHand().getSize() << "): ";
    
    int spellIndex;
    std::cin >> spellIndex;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (spellIndex < 1 || spellIndex > player.getHand().getSize()) {
        std::cout << "\n Неверный выбор заклинания!\n" << std::endl;
        return false;
    }
    
    ISpell* selectedSpell = player.getHand().getSpell(spellIndex - 1);
    if (!selectedSpell) {
        std::cout << "\n Ошибка: заклинание не найдено!\n" << std::endl;
        return false;
    }
    
    int spellManaCost = selectedSpell->getManaCost();
    

    if (player.getMana() < spellManaCost) {
        std::cout << "\n НЕДОСТАТОЧНО МАНЫ!" << std::endl;
        std::cout << "Заклинание '" << selectedSpell->getName() 
                  << "' требует " << spellManaCost << " маны, а у вас " 
                  << player.getMana() << "\n" << std::endl;
        return false;
    }
    
    std::string spellName = selectedSpell->getName();
    bool spellWasCast = false;
    Point targetPosition;
    
    if (spellName == "Удар молнии") {
        std::cout << "\n Введите координаты цели (x y): ";
        int targetX, targetY;
        std::cin >> targetX >> targetY;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        targetPosition = Point(targetX, targetY);
        
        if (!field.isWithinBounds(targetPosition)) {
            std::cout << "\n Цель вне границ поля!\n" << std::endl;
            return false;
        }
        
        Point playerPos = player.getPosition();
        int distance = playerPos.distanceTo(targetPosition);
        int castRange = selectedSpell->getCastRange();
        
        if (distance > castRange) {
            std::cout << "\n Цель слишком далеко! Максимальный радиус: " 
                      << castRange << " клеток\n" << std::endl;
            return false;
        }
        
        spellWasCast = player.getHand().useSpell(spellIndex - 1, player.getPosition(), targetPosition);
        
    } else if (spellName == "Ледяной вихрь") {
        std::cout << "\n>>> " << spellName << " активирован вокруг вас! <<<\n" << std::endl;
        
        targetPosition = player.getPosition();
        spellWasCast = player.getHand().useSpell(spellIndex - 1, player.getPosition(), player.getPosition());
    } else {
        std::cout << "\n Неизвестное заклинание!\n" << std::endl;
        return false;
    }
    
    if (spellWasCast) {
        int currentMana = player.getMana();
        int newMana = currentMana - spellManaCost;
        
        player.setMana(newMana);
        
        std::cout << "\n Потрачено " << spellManaCost << " маны. Осталось: " 
                  << player.getMana() << "/30" << std::endl;
        
        updateTargetsList();
        
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const EnemyInfo& info) { return !info.enemy->isAlive(); }),
            enemies.end()
        );
        
        for (const auto& enemyInfo : enemies) {
            field.setCellType(enemyInfo.position, Cell::Type::ENEMY);
        }
        
        return true; 
    }
    
    return false; 
}

void Game::updateEnemies() {
    Point playerPos = player.getPosition();
    
    for (auto& enemyInfo : enemies) {
        if (!enemyInfo.enemy->isAlive()) {
            continue;
        }
        
        Point moveDir = enemyInfo.enemy->chooseMoveDirection(enemyInfo.position, playerPos);
        Point newPos = enemyInfo.position + moveDir;
        
        if (!field.isWithinBounds(newPos)) {
            continue;
        }
        
        Cell::Type cellType = field.getCellType(newPos);
        
        if (cellType == Cell::Type::PLAYER) {
            handleEnemyAttack(*enemyInfo.enemy);
        } 
        else if (cellType == Cell::Type::EMPTY) {
            field.setCellType(enemyInfo.position, Cell::Type::EMPTY);
            field.setCellType(newPos, Cell::Type::ENEMY);
            enemyInfo.position = newPos;
            enemyInfo.enemy->setPosition(newPos);
        }
    }
}

void Game::handleEnemyAttack(Enemy& enemy) {
    std::cout << "\n>>> ВРАГ АТАКОВАЛ! <<<" << std::endl;
    std::cout << "Урон: " << enemy.getDamage() << std::endl;
    player.takeDamage(enemy.getDamage());
    std::cout << "Ваше здоровье: " << player.getHealth() << "\n" << std::endl;
    
    if (!player.isAlive()) {
        gameOver = true;
    }
}

void Game::handlePlayerAttack(const Point& enemyPosition) {
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        if (it->position == enemyPosition) {
            it->enemy->takeDamage(player.getDamage());
            
            std::cout << "Вы нанесли " << player.getDamage() << " урона врагу!" << std::endl;
            
            if (!it->enemy->isAlive()) {
                std::cout << "ВРАГ ПОБЕЖДЕН! +10 очков" << std::endl;
                field.setCellType(it->position, Cell::Type::EMPTY);
                player.increaseScore(10);
                enemies.erase(it);
            } else {
                std::cout << "У врага осталось " << it->enemy->getHealth() << " здоровья" << std::endl;
            }
            std::cout << std::endl;
            return;
        }
    }
    std::cout << "Ошибка: враг не найден!" << std::endl;
}

void Game::render() {
    for (int i = 0; i < 5; ++i) {
        std::cout << std::endl;
    }
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "ИГРОВОЕ ПОЛЕ" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    std::cout << "  ";
    for (int x = 0; x < field.getWidth(); x++) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    for (int y = 0; y < field.getHeight(); y++) {
        std::cout << y << " ";
        for (int x = 0; x < field.getWidth(); x++) {
            Cell::Type type = field.getCellType(Point(x, y));
            if (type == Cell::Type::EMPTY) {
                std::cout << ". ";
            } else if (type == Cell::Type::PLAYER) {
                std::cout << "P ";
            } else if (type == Cell::Type::ENEMY) {
                std::cout << "E ";
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "ИНФОРМАЦИЯ ОБ ИГРОКЕ" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    Point playerPos = player.getPosition();
    std::cout << "Позиция: (" << playerPos.getX() << ", " << playerPos.getY() << ")" << std::endl;
    std::cout << "Здоровье: " << player.getHealth() << std::endl;
    std::cout << "Урон: " << player.getDamage() << std::endl;
    std::cout << "Очки: " << player.getScore() << std::endl;
    std::cout << "Мана: " << player.getMana() << " / 30" << std::endl;
    std::cout << "Заклинаний в руке: " << player.getHand().getSize() << " / " 
              << player.getHand().getCapacity() << std::endl;
    std::cout << "Врагов осталось: " << enemies.size() << std::endl;
    
    int kills = player.getScore() / 10;
    int killsToNextSpell = 5 - (kills % 5);
    if (killsToNextSpell == 5) killsToNextSpell = 0;
    
    std::cout << "Врагов побеждено: " << kills << std::endl;
    std::cout << "До следующего заклинания: " << killsToNextSpell << " убийств" << std::endl;
    
    std::cout << std::string(50, '=') << "\n" << std::endl;
}

void Game::checkForSpellRewards() {
    int totalKills = player.getScore() / 10;
    

    if (totalKills > enemiesDefeated) {
        int newKills = totalKills - enemiesDefeated;
        
        static int recentlyKilled = 0;
        recentlyKilled += newKills;
        
        if (recentlyKilled >= 5) {
            if (player.getHand().isFull()) {
                std::cout << "\n" << std::string(50, '=') << std::endl;
                std::cout << "НАГРАДА ЗА ЗАКЛИНАНИЕ" << std::endl;
                std::cout << std::string(50, '=') << std::endl;
                std::cout << "Вы победили 5 врагов!" << std::endl;
                std::cout << "Вы получили новое заклинание, но рука полна." << std::endl;
                std::cout << "Теперь у вас 2 заклинания в руке." << std::endl;
                std::cout << std::string(50, '=') << "\n" << std::endl;
            } else {
                std::cout << "\n" << std::string(50, '=') << std::endl;
                std::cout << "НАГРАДА ЗА ЗАКЛИНАНИЕ" << std::endl;
                std::cout << std::string(50, '=') << std::endl;
                std::cout << "Вы победили 5 врагов!" << std::endl;
                std::cout << "Вы получаете новое заклинание!" << std::endl;
                player.getHand().drawRandomSpell(spellDeck);
                std::cout << std::string(50, '=') << "\n" << std::endl;
            }
            
            recentlyKilled = recentlyKilled % 5;
        }
        
        enemiesDefeated = totalKills;
    }
    
    player.restoreMana(1); 
}