//
// Created by anton on 10/25/2026.
//

#ifndef SOKOBAN_GAME_H
#define SOKOBAN_GAME_H

#include "Level.h"
enum class Screen { Menu, Playing, Won };

class Game {
public:
    Screen screen = Screen::Menu;
    std::vector<std::string> levelPaths;   // "levels/1.txt", ...
    int selected = 0;                      // menu cursor
    int current = 0;                       // level being played
    Level level;
    bool startLevel()
    {
        // load level
        auto aux = Level::fromFile(levelPaths[selected]);
        if (!aux) {SDL_Log("Failed to load level"); return false;}
        level = std::move(*aux);
        screen = Screen::Playing;
        return true;
    }
};


#endif //SOKOBAN_GAME_H
