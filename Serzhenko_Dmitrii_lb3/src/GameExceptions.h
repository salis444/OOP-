#ifndef GAMEEXCEPTIONS_H
#define GAMEEXCEPTIONS_H

#include <stdexcept>
#include <string>

class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message) 
        : std::runtime_error("ОШИБКА: " + message) {}
};

class SaveException : public GameException {
public:
    explicit SaveException(const std::string& message) 
        : GameException("ОШИБКА СОХРАНЕНИЯ: " + message) {}
};

class LoadException : public GameException {
public:
    explicit LoadException(const std::string& message) 
        : GameException("ОШИБКА ЗАГРУЗКИ: " + message) {}
};

class CorruptedSaveException : public LoadException {
public:
    explicit CorruptedSaveException(const std::string& message) 
        : LoadException("ФАЙЛ ПОВРЕЖДЕН: " + message) {}
};

class FileAccessException : public SaveException {
public:
    explicit FileAccessException(const std::string& filename) 
        : SaveException("Не удалось открыть файл: " + filename) {}
};

class InvalidGameStateException : public GameException {
public:
    explicit InvalidGameStateException(const std::string& message) 
        : GameException("НЕКОРРЕКТНОЕ СОСТОЯНИЕ ИГРЫ: " + message) {}
};

#endif