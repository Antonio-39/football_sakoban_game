#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>
#include <algorithm>

#include "Renderer.h"

bool handleEvent(SDL_Event &, SDLState &, Game &, Action &);

int main(int, char**)
{
    SDLState state = SDLState();
    if (SDLState::init(state) == false)
    {
        SDL_Log("SDL Initialization failed");
        return 1;
    }

    // load resources
    Resources res;
    res.load(state);

    // declare game variable
    Game game;
    game.levelPaths.push_back("../levels/level1.txt");

    SDL_Event event{ 0 };
    bool running = true;
    Action lastAction = Action::Up;
    while (running)
    {
        // handle events
        while (SDL_PollEvent(&event))
            running = handleEvent(event, state, game, lastAction);

        // draw the level
        SDL_SetRenderDrawColor(state.renderer, 196, 219, 122, 255);
        SDL_RenderClear(state.renderer);

        if (game.screen == Screen::Menu)
            Renderer::drawMenu(state, game, res);
        if (game.screen == Screen::Playing)
        {
            Renderer::drawLevel(state, game.level, lastAction, res);

            // if level is solved end the game
            if (game.level.isSolved())
                game.screen = Screen::Won;
        }
        if (game.screen == Screen::Won)
            Renderer::drawWon(state, game, lastAction, res);

        SDL_RenderPresent(state.renderer);
    }

    res.unload();
    SDLState::cleanup(state);
    return 0;
}

bool handleEvent(SDL_Event &event, SDLState &state, Game &game, Action &lastAction)
{
    bool flag = true;
    if (event.type == SDL_EVENT_QUIT)
    {
        flag = false;
    }
    else if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
        state.width = event.window.data1;
        state.height = event.window.data2;
    }
    if (game.screen == Screen::Menu)
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_ESCAPE) flag = false;
            if (event.key.key == SDLK_UP)   game.selected = std::max(0, game.selected - 1);
            if (event.key.key == SDLK_DOWN) game.selected = std::min((int)game.levelPaths.size() - 1, game.selected + 1);
            if (event.key.key == SDLK_SPACE)
                if (!game.startLevel())
                    flag = false;
        }
    }
    if (game.screen == Screen::Playing && event.type == SDL_EVENT_KEY_DOWN)
    {
        switch (actionFromKey(event.key.key))
        {
            case Action::Up: game.level.tryMove({0 , -1}); lastAction = Action::Up; break;
            case Action::Down: game.level.tryMove({0 , 1}); lastAction = Action::Down; break;
            case Action::Left: game.level.tryMove({-1 , 0}); lastAction = Action::Left; break;
            case Action::Right: game.level.tryMove({1 , 0}); lastAction = Action::Right; break;
            case Action::Undo: game.level.undo(); break;
            case Action::Reset: game.level.reset(); lastAction = Action::Up; break;
            case Action::Quit: flag = false; break;
            default: break;
        }
    }
    if (game.screen == Screen::Won && event.type == SDL_EVENT_KEY_DOWN)
    {
        if (event.key.key == SDLK_ESCAPE) flag = false;
        else game.screen = Screen::Menu;
    }
    return flag;
}