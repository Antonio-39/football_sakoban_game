//
// Created by anton on 10/25/2026.
//

#include "Renderer.h"

#include "Resources.h"

Action actionFromKey(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_UP: case SDLK_W: return Action::Up;
        case SDLK_DOWN: case SDLK_S: return Action::Down;
        case SDLK_LEFT: case SDLK_A: return Action::Left;
        case SDLK_RIGHT: case SDLK_D: return Action::Right;
        case SDLK_U: return Action::Undo;
        case SDLK_R: return Action::Reset;
        case SDLK_ESCAPE: return Action::Quit;
        default: return Action::None;
    }
}
void Renderer::drawLevel(SDLState& state, const Level& level, Action last, const Resources &res)
{
    const float dimension = 32.0;
    const float leftSpace = (float)(state.logW - level.w * dimension) / 2;
    const float upSpace = (float)(state.logH - level.h * dimension) / 2;

    // draw map
    for (int i = 0 ; i < level.tiles.size(); i++)
    {
        Tile tile = level.tiles[i];
        Pair pos = { i % level.w, i / level.w};
        if (tile == Tile::WALL)
        {
            SDL_SetRenderDrawColor(state.renderer , 255 , 0, 0, 255);
            SDL_FRect rect = {pos.x * dimension + leftSpace, pos.y * dimension + upSpace, dimension, dimension};
            SDL_RenderFillRect(state.renderer , &rect);
        }
        else if (tile == Tile::GOAL)
        {
            SDL_SetRenderDrawColor(state.renderer , 0 , 255, 0, 255);
            SDL_FRect rect = {pos.x * dimension + leftSpace, pos.y * dimension + upSpace, dimension, dimension};
            SDL_RenderFillRect(state.renderer , &rect);
        }
    }

    //draw the boxes
    for (Pair box : level.boxes)
    {
        SDL_FRect src{ 0, 0, dimension,  dimension };
        SDL_FRect dst{ box.x * dimension + leftSpace, box.y * dimension + upSpace, dimension,  dimension };
        SDL_RenderTextureRotated(state.renderer, res.texBox, &src, &dst, 0, nullptr, SDL_FLIP_NONE);
    }

    //gasim partea din animatie pt player
    int playerDir = 0;
    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (last == Action::Right && !level.history.empty() && level.history.back().pushedBox)
        playerDir = 2;
    else if (last == Action::Right)
        playerDir = 3;
    else if (last == Action::Left && !level.history.empty() && level.history.back().pushedBox)
    {
        playerDir = 2;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (last == Action::Left)
    {
        playerDir = 3;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (last == Action::Up && !level.history.empty() && level.history.back().pushedBox)
    {
        playerDir = 1;
    }
    else if (last == Action::Up)
    {
        playerDir = 0;
    }
    else if (last == Action::Down && !level.history.empty() && level.history.back().pushedBox)
    {
        playerDir = 1;
        flip = SDL_FLIP_VERTICAL;
    }
    else if (last == Action::Down)
    {
        playerDir = 0;
    }

    //draw the player
    SDL_FRect src{ (float)playerDir * 32, 0, dimension,  dimension };
    SDL_FRect dst{ level.player.x * dimension + leftSpace, level.player.y * dimension + upSpace, dimension,  dimension };
    SDL_RenderTextureRotated(state.renderer, res.texPlayer, &src, &dst, 0, nullptr, flip);
}

    // Draws text horizontally centred on cx, with its top edge at y
void Renderer::drawTextCentered(SDLState& state, const std::string& text, float cx, float y, SDL_Color color,const Resources &res)
{
    SDL_Surface* surf = TTF_RenderText_Blended(res.font, text.c_str(), 0, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(state.renderer, surf);
    SDL_DestroySurface(surf);
    if (!tex) return;

    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    SDL_FRect dst{cx - w / 2, y, w, h};
    SDL_RenderTexture(state.renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Renderer::drawMenu(SDLState& state, const Game& game, const Resources &res) {
    float SCREEN_W = (float)state.logW;
    constexpr SDL_Color WHITE  {255, 255, 255, 255};
    constexpr SDL_Color YELLOW {255, 220,  60, 255};
    constexpr SDL_Color GREY   {150, 150, 150, 255};

    const float cx = SCREEN_W / 2;

    // Background
    SDL_SetRenderDrawColor(state.renderer, 20, 20, 30, 255);


    // Title
    drawTextCentered(state,"FootBall SAKOBAN", cx, 60, WHITE , res);

    // Level list
    constexpr float START_Y = 200.0f;
    constexpr float LINE_H  = 44.0f;

    for (int i = 0; i < (int)game.levelPaths.size(); ++i) {
        bool isSelected = (i == game.selected);

        std::string label = "Level " + std::to_string(i + 1);
        if (isSelected) label = "> " + label + " <";

        drawTextCentered(state, label, cx, START_Y + i * LINE_H, isSelected ? YELLOW : GREY,res);
    }

    // Hint at the bottom
    drawTextCentered(state, "Up/Down: choose   Space: play   Esc: quit", cx, state.logH - 50, GREY, res);
}
void Renderer::drawWon(SDLState& state, const Game& game, Action last, const Resources &res)
{
    constexpr SDL_Color WHITE  {255, 255, 255, 255};
    constexpr SDL_Color YELLOW {255, 220,  60, 255};
    constexpr SDL_Color GREY   {200, 200, 200, 255};

    const float cx = (float)state.logW / 2;
    const float cy = (float)state.logH / 2;

    // keep the solved board visible behind the text
    drawLevel(state, game.level, last, res);

    // dim overlay
    SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 170);
    SDL_FRect overlay{0, 0, (float)state.logW, (float)state.logH};
    SDL_RenderFillRect(state.renderer, &overlay);
    SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);

    // stats from the undo history
    int moves  = (int)game.level.history.size();
    int pushes = (int)std::count_if(game.level.history.begin(), game.level.history.end(), [](const Move& m) { return m.pushedBox; });

    drawTextCentered(state, "LEVEL COMPLETE!", cx, cy - 70, YELLOW, res);
    drawTextCentered(state, "Moves: " + std::to_string(moves) + "   Pushes: " + std::to_string(pushes), cx, cy - 10, WHITE, res);
    drawTextCentered(state, "Press any key to return to the menu", cx, cy + 50, GREY, res);
}
