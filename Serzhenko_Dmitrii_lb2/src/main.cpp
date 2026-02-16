#include "Game.h"
#include <iostream>
#include <limits>

int main() {
    try {
        int width, height, enemyCount;
        
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "НАСТРОЙКА ИГРЫ" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::cout << "\n Введите ширину поля (10-25): ";
        std::cin >> width;
        
        std::cout << "Введите высоту поля (10-25): ";
        std::cin >> height;
        
        if (width < 10 || width > 25 || height < 10 || height > 25) {
            std::cout << "\n Неверные размеры поля. Используются значения по умолчанию (15x15)." << std::endl;
            width = 15;
            height = 15;
        }
        
        int maxPossibleEnemies = width * height - 1;
        std::cout << "\n Введите количество врагов (1-" << maxPossibleEnemies << "): ";
        std::cin >> enemyCount;
        
        if (enemyCount < 1) {
            enemyCount = 3;
            std::cout << "Используется значение по умолчанию: 3 врага" << std::endl;
        }
        
        std::cout << "\n Запуск игры...\n" << std::endl;
        
        Game game(width, height, enemyCount);
        game.run();
        
        std::cout << "\n Игра завершена. Нажмите Enter для выхода...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        
    } catch (const std::exception& e) {
        std::cerr << "\n ОШИБКА: " << e.what() << std::endl;
        std::cerr << "Нажмите Enter для выхода...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return 1;
    }
    
    return 0;
}