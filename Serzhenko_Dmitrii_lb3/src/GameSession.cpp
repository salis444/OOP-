#include "GameSession.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include "SaveManager.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>

GameSession::GameSession(int width, int height, int enemyCount) 
    : field(width, height), 
      player(100, 10, 30, Point(0, 0)),
      gameOver(false),
      enemyCount(enemyCount),
      enemiesDefeated(0) {
    
    int maxPossibleEnemies = width * height - 1;
    if (enemyCount > maxPossibleEnemies) {
        std::cout << "Количество врагов слишком велико. Установлено максимальное: " 
                  << maxPossibleEnemies << std::endl;
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
    std::cout << "НОВАЯ ИГРА НАЧАТА" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Введите 'h' для помощи, 'p' для сохранения, 'l' для загрузки, 'q' для выхода\n" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

GameSession::GameSession(GameSession&& other) noexcept
    : field(std::move(other.field)),
      player(std::move(other.player)),
      enemies(std::move(other.enemies)),
      allTargets(std::move(other.allTargets)),
      spellDeck(std::move(other.spellDeck)),
      turnManager(std::move(other.turnManager)),
      gameOver(other.gameOver),
      enemyCount(other.enemyCount),
      enemiesDefeated(other.enemiesDefeated) {}

GameSession& GameSession::operator=(GameSession&& other) noexcept {
    if (this != &other) {
        field = std::move(other.field);
        player = std::move(other.player);
        enemies = std::move(other.enemies);
        allTargets = std::move(other.allTargets);
        spellDeck = std::move(other.spellDeck);
        turnManager = std::move(other.turnManager);
        gameOver = other.gameOver;
        enemyCount = other.enemyCount;
        enemiesDefeated = other.enemiesDefeated;
    }
    return *this;
}

void GameSession::initializeSpellDeck() {
    spellDeck.push_back(std::make_unique<DirectDamageSpell>("Удар молнии", 30, 6, 3));
    spellDeck.push_back(std::make_unique<AreaDamageSpell>("Ледяной вихрь", 25, 6, 2, 2));
    
    player.getHand().drawRandomSpell(spellDeck);
}

void GameSession::spawnEnemies() {
    for (int i = 0; i < enemyCount; ++i) {
        Point enemyPos = field.getRandomEmptyCell();
        field.setCellType(enemyPos, Cell::Type::ENEMY);
        
        auto enemy = std::make_shared<Enemy>(50, 5, enemyPos);
        enemies.push_back(enemy);
    }
}

void GameSession::addEnemy(std::shared_ptr<Enemy> enemy) {
    enemies.push_back(enemy);
    field.setCellType(enemy->getPosition(), Cell::Type::ENEMY);
    updateTargetsList();
}

void GameSession::updateTargetsList() {
    allTargets.clear();
    for (const auto& enemy : enemies) {
        if (enemy->isAlive()) {
            allTargets.push_back(enemy);
        }
    }
}

Point GameSession::getDirectionOffset(char direction) const {
    switch (direction) {
        case 'w': case 'W': return Point(0, -1);
        case 's': case 'S': return Point(0, 1);
        case 'a': case 'A': return Point(-1, 0);
        case 'd': case 'D': return Point(1, 0);
        default: return Point(0, 0);
    }
}

bool GameSession::isValidDirection(char direction) const {
    char lower = tolower(direction);
    return lower == 'w' || lower == 's' || lower == 'a' || lower == 'd';
}

GameSession::PlayerAction GameSession::getPlayerAction() {
    std::cout << "\nВаш ход. Выберите действие:\n";
    std::cout << "  [w/a/s/d] - переместиться\n";
    std::cout << "  [c] - использовать заклинание\n";
    std::cout << "  [p] - сохранить игру\n";
    std::cout << "  [l] - загрузить игру\n";
    std::cout << "  [h] - помощь\n";
    std::cout << "  [q] - выйти\n";
    std::cout << "> ";
    
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (isValidDirection(choice)) {
        return PlayerAction::makeMove(choice);
    }
    
    switch (tolower(choice)) {
        case 'c':
            if (player.getHand().isEmpty()) {
                std::cout << "У вас нет заклинаний!\n";
                return PlayerAction::makeInvalid();
            }
            
            player.getHand().display();
            
            std::cout << "Выберите заклинание (1-" << player.getHand().getSize() << "): ";
            int spellIndex;
            std::cin >> spellIndex;
            spellIndex--;
            
            std::cout << "Введите координаты цели (x y): ";
            int targetX, targetY;
            std::cin >> targetX >> targetY;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            return PlayerAction::makeCast(spellIndex, targetX, targetY);
            
        case 'p': {
            std::string filename;
            std::cout << "Введите имя файла для сохранения: ";
            std::getline(std::cin, filename);
            return PlayerAction::makeSave(filename);
        }
        
        case 'l': {
            auto saves = SaveManager::listSaves();
            if (saves.empty()) {
                std::cout << "Нет сохраненных игр.\n";
                return PlayerAction::makeInvalid();
            }
            
            std::cout << "\nДоступные сохранения:\n";
            for (size_t i = 0; i < saves.size(); ++i) {
                std::cout << i + 1 << ". " << saves[i] << std::endl;
            }
            
            std::cout << "Выберите сохранение (1-" << saves.size() << "): ";
            int saveChoice;
            std::cin >> saveChoice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (saveChoice >= 1 && saveChoice <= static_cast<int>(saves.size())) {
                return PlayerAction::makeLoad(saves[saveChoice - 1]);
            } else {
                std::cout << "Неверный выбор.\n";
                return PlayerAction::makeInvalid();
            }
        }
            
        case 'h':
            showHelp();
            return PlayerAction::makeInvalid();
            
        case 'q':
            return PlayerAction::makeQuit();
            
        default:
            std::cout << "Неверный ввод!\n";
            return PlayerAction::makeInvalid();
    }
}

void GameSession::processPlayerAction(const PlayerAction& action) {
    try {
        switch (action.type) {
            case PlayerAction::MOVE:
                processMovement(action.direction);
                break;
                
            case PlayerAction::CAST_SPELL:
                processSpellCast(action.spellIndex, action.targetX, action.targetY);
                break;
                
            case PlayerAction::SAVE:
                processSave(action.filename);
                break;
                
            case PlayerAction::LOAD:
                processLoad(action.filename);
                break;
                
            case PlayerAction::QUIT:
                std::cout << "Выход из игры...\n";
                gameOver = true;
                break;
                
            default:
                break;
        }
    } catch (const GameException& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void GameSession::processMovement(char direction) {
    Point currentPos = player.getPosition();
    Point newPos = currentPos + getDirectionOffset(direction);
    
    if (!field.isWithinBounds(newPos)) {
        std::cout << "Нельзя выйти за границы поля!\n";
        return;
    }
    
    Cell::Type cellType = field.getCellType(newPos);
    
    if (cellType == Cell::Type::ENEMY) {
        std::cout << "\n>>> ВЫ АТАКУЕТЕ ВРАГА! <<<\n";
        handlePlayerAttack(newPos);
    } else if (cellType == Cell::Type::EMPTY) {
        field.setCellType(currentPos, Cell::Type::EMPTY);
        field.setCellType(newPos, Cell::Type::PLAYER);
        field.setPlayerPosition(newPos);
        player.setPosition(newPos);
        std::cout << "Вы переместились на новую позицию.\n";
    }
}

bool GameSession::processSpellCast(int spellIndex, int targetX, int targetY) {
    if (spellIndex < 0 || spellIndex >= player.getHand().getSize()) {
        std::cout << "Неверный индекс заклинания!\n";
        return false;
    }
    
    ISpell* selectedSpell = player.getHand().getSpell(spellIndex);
    if (!selectedSpell) {
        std::cout << "Ошибка: заклинание не найдено!\n";
        return false;
    }
    
    Point targetPosition(targetX, targetY);
    if (!field.isWithinBounds(targetPosition)) {
        std::cout << "Цель вне границ поля!\n";
        return false;
    }
    
    if (player.getMana() < selectedSpell->getManaCost()) {
        std::cout << "Недостаточно маны!\n";
        return false;
    }
    
    if (auto directSpell = dynamic_cast<DirectDamageSpell*>(selectedSpell)) {
        auto target = std::find_if(allTargets.begin(), allTargets.end(),
            [&targetPosition](const auto& t) {
                return t->getPosition() == targetPosition && t->isAlive();
            });
        
        if (target == allTargets.end()) {
            std::cout << "В указанной позиции нет врага!\n";
            return false;
        }
        
        std::cout << ">>> " << selectedSpell->getName() << "! <<<\n";
        (*target)->takeDamage(directSpell->getDamage());
        
        if (!(*target)->isAlive()) {
            std::cout << "Враг уничтожен!\n";
            field.setCellType(targetPosition, Cell::Type::EMPTY);
        }
        
    } else if (auto areaSpell = dynamic_cast<AreaDamageSpell*>(selectedSpell)) {
        std::cout << ">>> " << selectedSpell->getName() << "! <<<\n";
        
        std::vector<std::shared_ptr<ITarget>> targetsInArea;
        for (const auto& target : allTargets) {
            if (target->isAlive() && 
                std::abs(target->getPosition().getX() - player.getPosition().getX()) < areaSpell->getAreaSize() &&
                std::abs(target->getPosition().getY() - player.getPosition().getY()) < areaSpell->getAreaSize()) {
                targetsInArea.push_back(target);
            }
        }
        
        if (targetsInArea.empty()) {
            std::cout << "В области нет врагов.\n";
        } else {
            std::cout << "Поражено врагов: " << targetsInArea.size() << std::endl;
            for (auto& target : targetsInArea) {
                target->takeDamage(areaSpell->getDamage());
                if (!target->isAlive()) {
                    field.setCellType(target->getPosition(), Cell::Type::EMPTY);
                }
            }
        }
    }
    
    player.setMana(player.getMana() - selectedSpell->getManaCost());
    std::cout << "Потрачено " << selectedSpell->getManaCost() 
              << " маны. Осталось: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    
    updateTargetsList();
    
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const auto& enemy) { return !enemy->isAlive(); }),
        enemies.end()
    );
    
    return true;
}

void GameSession::processSave(const std::string& filename) {
    try {
        SaveManager saver(filename);
        saver.saveGame(*this);
        std::cout << "Игра успешно сохранена в '" << filename << "'\n";
    } catch (const SaveException& e) {
        std::cerr << "Не удалось сохранить игру: " << e.what() << std::endl;
        std::cout << "Попробуйте другое имя файла.\n";
    }
}

void GameSession::processLoad(const std::string& filename) {
    try {
        std::cout << "Загрузка игры из файла: " << filename << std::endl;
        SaveManager saver(filename);
        auto loadedGame = saver.loadGame();
        
        if (loadedGame) {
            // Создаем новое пустое поле тех же размеров
            Field newField(loadedGame->field.getWidth(), loadedGame->field.getHeight());
            
            // Очищаем текущие данные
            enemies.clear();
            allTargets.clear();
            spellDeck.clear();
            
            // Переносим данные из загруженной игры
            field = std::move(newField);
            player = std::move(loadedGame->player);
            enemies = std::move(loadedGame->enemies);
            spellDeck = std::move(loadedGame->spellDeck);
            turnManager = std::move(loadedGame->turnManager);
            gameOver = loadedGame->gameOver;
            enemyCount = loadedGame->enemyCount;
            enemiesDefeated = loadedGame->enemiesDefeated;
            
            // Заново отрисовываем поле с загруженными данными
            field.setCellType(player.getPosition(), Cell::Type::PLAYER);
            field.setPlayerPosition(player.getPosition());
            
            for (const auto& enemy : enemies) {
                if (enemy->isAlive()) {
                    field.setCellType(enemy->getPosition(), Cell::Type::ENEMY);
                }
            }
            
            updateTargetsList();
            
            std::cout << "Игра успешно загружена!\n";
            std::cout << "Позиция игрока: (" << player.getPosition().getX() 
                      << ", " << player.getPosition().getY() << ")\n";
            std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
            std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
            std::cout << "Врагов: " << enemies.size() << std::endl;
            
            turnManager.setPhase(TurnManager::Phase::PLAYER_TURN);
        }
    } catch (const LoadException& e) {
        std::cerr << "Не удалось загрузить игру: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке: " << e.what() << std::endl;
    }
}

void GameSession::processEnemiesTurn() {
    if (enemies.empty()) {
        std::cout << "Нет врагов для хода.\n";
        return;
    }
    
    std::cout << "\n--- Ход врагов ---\n";
    Point playerPos = player.getPosition();
    
    for (auto& enemy : enemies) {
        if (!enemy->isAlive()) continue;
        
        Point moveDir = enemy->chooseMoveDirection(enemy->getPosition(), playerPos);
        Point newPos = enemy->getPosition() + moveDir;
        
        if (!field.isWithinBounds(newPos)) continue;
        
        Cell::Type cellType = field.getCellType(newPos);
        
        if (cellType == Cell::Type::PLAYER) {
            handleEnemyAttack(*enemy);
        } else if (cellType == Cell::Type::EMPTY) {
            field.setCellType(enemy->getPosition(), Cell::Type::EMPTY);
            field.setCellType(newPos, Cell::Type::ENEMY);
            enemy->setPosition(newPos);
        }
    }
    
    player.restoreMana(1);
    std::cout << "Мана восстановлена на 1. Текущая мана: " 
              << player.getMana() << "/" << player.getMaxMana() << std::endl;
}

void GameSession::handleEnemyAttack(Enemy& enemy) {
    std::cout << "\n>>> ВРАГ АТАКОВАЛ! <<<\n";
    std::cout << "Урон: " << enemy.getDamage() << std::endl;
    player.takeDamage(enemy.getDamage());
    std::cout << "Ваше здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    
    if (!player.isAlive()) {
        std::cout << "\n*** ВЫ ПОГИБЛИ! ***\n";
        gameOver = true;
        turnManager.setPhase(TurnManager::Phase::GAME_OVER);
    }
}

void GameSession::handlePlayerAttack(const Point& enemyPosition) {
    auto it = std::find_if(enemies.begin(), enemies.end(),
        [&enemyPosition](const auto& enemy) {
            return enemy->getPosition() == enemyPosition;
        });
    
    if (it != enemies.end()) {
        (*it)->takeDamage(player.getDamage());
        std::cout << "Вы нанесли " << player.getDamage() << " урона врагу!\n";
        
        if (!(*it)->isAlive()) {
            std::cout << "ВРАГ ПОБЕЖДЕН! +10 очков\n";
            field.setCellType((*it)->getPosition(), Cell::Type::EMPTY);
            player.increaseScore(10);
            enemies.erase(it);
            enemiesDefeated++;
            
            if (enemiesDefeated % 5 == 0 && !player.getHand().isFull()) {
                std::cout << "\n*** ВЫ ПОЛУЧАЕТЕ НОВОЕ ЗАКЛИНАНИЕ! ***\n";
                player.getHand().drawRandomSpell(spellDeck);
            }
        } else {
            std::cout << "У врага осталось " << (*it)->getHealth() << " здоровья\n";
        }
        
        updateTargetsList();
    }
}

bool GameSession::isLevelComplete() const {
    return enemies.empty();
}

void GameSession::resetLevel() {
    std::cout << "\n=== УРОВЕНЬ ПРОЙДЕН! ===\n";
    std::cout << "Переход на следующий уровень...\n";
    
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            Point p(x, y);
            if (field.getCellType(p) == Cell::Type::ENEMY) {
                field.setCellType(p, Cell::Type::EMPTY);
            }
        }
    }
    
    int newEnemyCount = std::min(enemyCount + 2, field.getWidth() * field.getHeight() - 1);
    enemyCount = newEnemyCount;
    
    for (int i = 0; i < enemyCount; ++i) {
        Point enemyPos = field.getRandomEmptyCell();
        field.setCellType(enemyPos, Cell::Type::ENEMY);
        
        int enemyHealth = 50 + (enemiesDefeated / 5) * 10;
        int enemyDamage = 5 + (enemiesDefeated / 5) * 2;
        
        auto enemy = std::make_shared<Enemy>(enemyHealth, enemyDamage, enemyPos);
        enemies.push_back(enemy);
    }
    
    updateTargetsList();
    turnManager.setPhase(TurnManager::Phase::PLAYER_TURN);
}

void GameSession::resetGame() {
    std::cout << "\n=== НОВАЯ ИГРА ===\n";
    
    player = Player(100, 10, 30, Point(0, 0));
    enemies.clear();
    enemiesDefeated = 0;
    gameOver = false;
    
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            field.setCellType(Point(x, y), Cell::Type::EMPTY);
        }
    }
    
    Point playerStartPos = field.getRandomEmptyCell();
    field.setCellType(playerStartPos, Cell::Type::PLAYER);
    field.setPlayerPosition(playerStartPos);
    player.setPosition(playerStartPos);
    
    while (player.getHand().getSize() > 0) {
        player.getHand().removeSpell(0);
    }
    player.getHand().drawRandomSpell(spellDeck);
    
    spawnEnemies();
    updateTargetsList();
    turnManager.setPhase(TurnManager::Phase::PLAYER_TURN);
}

void GameSession::render() const {
    for (int i = 0; i < 3; ++i) std::cout << std::endl;
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "ИГРОВОЕ ПОЛЕ" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    std::cout << "  ";
    for (int x = 0; x < field.getWidth(); ++x) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    for (int y = 0; y < field.getHeight(); ++y) {
        std::cout << y << " ";
        for (int x = 0; x < field.getWidth(); ++x) {
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
    std::cout << "Позиция: (" << playerPos.getX() << ", " << playerPos.getY() << ")\n";
    std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Урон: " << player.getDamage() << std::endl;
    std::cout << "Очки: " << player.getScore() << std::endl;
    std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    std::cout << "Заклинания: " << player.getHand().getSize() << "/" << player.getHand().getCapacity() << std::endl;
    std::cout << "Врагов осталось: " << enemies.size() << std::endl;
    std::cout << "Врагов побеждено: " << enemiesDefeated << std::endl;
    
    std::cout << "Текущая фаза: " << turnManager << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void GameSession::showHelp() const {
    std::cout << "\n=== ПОМОЩЬ ===\n";
    std::cout << "Управление:\n";
    std::cout << "  w/a/s/d - перемещение (вверх/влево/вниз/вправо)\n";
    std::cout << "  c - использовать заклинание\n";
    std::cout << "  p - сохранить игру\n";
    std::cout << "  l - загрузить игру\n";
    std::cout << "  h - показать эту помощь\n";
    std::cout << "  q - выйти из игры\n\n";
    
    std::cout << "Заклинания:\n";
    std::cout << "  Удар молнии - прямой урон по выбранной цели\n";
    std::cout << "  Ледяной вихрь - урон по области 2x2 вокруг игрока\n\n";
    
    std::cout << "Награды:\n";
    std::cout << "  Каждые 5 убитых врагов - новое заклинание\n";
    std::cout << "  За убийство врага дается 10 очков\n";
    std::cout << "================\n";
}

void GameSession::run() {
    while (!gameOver) {
        try {
            render();
            
            if (turnManager.isPlayerTurn()) {
                PlayerAction action = getPlayerAction();
                
                if (action.type != PlayerAction::INVALID) {
                    processPlayerAction(action);
                    
                    if (action.type != PlayerAction::SAVE && 
                        action.type != PlayerAction::LOAD &&
                        action.type != PlayerAction::QUIT) {
                        turnManager.nextPhase();
                    }
                    
                    if (action.type == PlayerAction::QUIT) {
                        break;
                    }
                }
            }
            
            if (turnManager.isEnemiesTurn() && !gameOver) {
                processEnemiesTurn();
                turnManager.nextPhase();
            }
            
            if (isLevelComplete() && !gameOver) {
                turnManager.setPhase(TurnManager::Phase::LEVEL_COMPLETE);
                render();
                resetLevel();
            }
            
        } catch (const GameException& e) {
            std::cerr << "Игровая ошибка: " << e.what() << std::endl;
            std::cout << "Продолжаем игру...\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } catch (const std::exception& e) {
            std::cerr << "Неожиданная ошибка: " << e.what() << std::endl;
            std::cout << "Продолжаем игру...\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    if (!player.isAlive()) {
        std::cout << "\n=== ИГРА ОКОНЧЕНА ===\n";
        std::cout << "Ваш счет: " << player.getScore() << std::endl;
        std::cout << "Побеждено врагов: " << enemiesDefeated << std::endl;
        
        std::cout << "\nНачать заново? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (tolower(choice) == 'y') {
            resetGame();
            run();
        }
    }
}