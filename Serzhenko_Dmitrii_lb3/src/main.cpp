#include "GameSession.h"
#include "SaveManager.h"
#include <iostream>
#include <limits>
#include <vector>

void displayMenu() {
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "ROGUE-LIKE GAME" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "1. Новая игра" << std::endl;
    std::cout << "2. Загрузить игру" << std::endl;
    std::cout << "3. Выйти" << std::endl;
    std::cout << "Выбор: ";
}

void displaySaves(const std::vector<std::string>& saves) {
    if (saves.empty()) {
        std::cout << "Нет сохраненных игр." << std::endl;
        return;
    }
    
    std::cout << "\nДоступные сохранения:" << std::endl;
    for (size_t i = 0; i < saves.size(); ++i) {
        std::cout << i + 1 << ". " << saves[i] << std::endl;
    }
}

int main() {
    try {
        while (true) {
            displayMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (choice == 3) {
                std::cout << "Выход из игры. До свидания!" << std::endl;
                break;
            }
            
            std::unique_ptr<GameSession> game;
            
            if (choice == 1) {
                int width, height, enemyCount;
                
                std::cout << "\nНастройка новой игры:\n";
                
                do {
                    std::cout << "Ширина поля (10-25): ";
                    std::cin >> width;
                } while (width < 10 || width > 25);
                
                do {
                    std::cout << "Высота поля (10-25): ";
                    std::cin >> height;
                } while (height < 10 || height > 25);
                
                int maxEnemies = width * height - 1;
                do {
                    std::cout << "Количество врагов (1-" << maxEnemies << "): ";
                    std::cin >> enemyCount;
                } while (enemyCount < 1 || enemyCount > maxEnemies);
                
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                game = std::make_unique<GameSession>(width, height, enemyCount);
                
            } else if (choice == 2) {
                auto saves = SaveManager::listSaves();
                displaySaves(saves);
                
                if (saves.empty()) {
                    std::cout << "\nНачинаем новую игру...\n";
                    game = std::make_unique<GameSession>(15, 15, 5);
                } else {
                    std::cout << "\nВыберите сохранение (1-" << saves.size() << "): ";
                    int saveChoice;
                    std::cin >> saveChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    if (saveChoice >= 1 && saveChoice <= static_cast<int>(saves.size())) {
                        try {
                            SaveManager saver("saves/" + saves[saveChoice - 1]);
                            game = saver.loadGame();
                            std::cout << "Игра загружена!\n";
                        } catch (const LoadException& e) {
                            std::cerr << "Ошибка загрузки: " << e.what() << std::endl;
                            std::cout << "Начинаем новую игру...\n";
                            game = std::make_unique<GameSession>(15, 15, 5);
                        }
                    } else {
                        std::cout << "Неверный выбор. Начинаем новую игру...\n";
                        game = std::make_unique<GameSession>(15, 15, 5);
                    }
                }
            } else {
                std::cout << "Неверный выбор. Попробуйте снова.\n";
                continue;
            }
            
            if (game) {
                game->run();
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nКРИТИЧЕСКАЯ ОШИБКА: " << e.what() << std::endl;
        std::cout << "Нажмите Enter для выхода...";
        std::cin.get();
        return 1;
    }
    
    return 0;
}