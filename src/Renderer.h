//
// Created by anton on 10/25/2026.
//

#ifndef SOKOBAN_RENDERER_H
#define SOKOBAN_RENDERER_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

#include "Game.h"
#include "Resources.h"

enum class Action {None, Up, Down, Left, Right, Undo, Reset, Quit};
Action actionFromKey(SDL_Keycode key);

class Renderer {
public :
    static void drawLevel(SDLState& state, const Level& level, Action last, const Resources &);
    static void drawMenu(SDLState& state, const Game& game, const Resources &);
    static void drawWon(SDLState& state, const Game& game, Action last, const Resources &);
private:
    static void drawTextCentered(SDLState& state, const std::string& text, float cx, float y, SDL_Color color, const Resources &);

};


#endif //SOKOBAN_RENDERER_H
