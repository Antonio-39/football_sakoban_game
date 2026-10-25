#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>
#include <algorithm>

#include "Game.h"
#include "SDLState.h"


bool init(SDLState& state);
void cleanup(SDLState& state);

enum class Action {None, Up, Down, Left, Right, Undo, Reset, Quit};
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

struct Resources
{
    std::vector<SDL_Texture*> textures;
    SDL_Texture* texPlayer;
    SDL_Texture* texBox;
    TTF_Font* font = nullptr;


    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filepath)
    {
        SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());

        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        // this changes the texture scale mode to nearest so pixels don't blend into each other

        textures.push_back(tex);
        return tex;
    }

    void load(SDLState& state)
    {
        texPlayer = loadTexture(state.renderer, "../data/footballer.png");
        texBox = loadTexture(state.renderer, "../data/ball.png");
        font = TTF_OpenFont("../data/font.ttf", 20); // path + point size
    }

    void unload()
    {
        for (SDL_Texture* tex : textures)
        {
            SDL_DestroyTexture(tex);
        }
        TTF_CloseFont(font);
        TTF_Quit();
    }

    void drawLevel(SDLState& state, Level level, Action last)
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
            SDL_RenderTextureRotated(state.renderer, texBox, &src, &dst, 0, nullptr, SDL_FLIP_NONE);
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
        SDL_RenderTextureRotated(state.renderer, texPlayer, &src, &dst, 0, nullptr, flip);
    }

    // Draws text horizontally centred on cx, with its top edge at y
    void drawTextCentered(SDLState& state, const std::string& text,
                          float cx, float y, SDL_Color color) {
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), 0, color);
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

    void drawMenu(SDLState& state, const Game& game) {
        float SCREEN_W = (float)state.logW;
        constexpr SDL_Color WHITE  {255, 255, 255, 255};
        constexpr SDL_Color YELLOW {255, 220,  60, 255};
        constexpr SDL_Color GREY   {150, 150, 150, 255};

        const float cx = SCREEN_W / 2;

        // Background
        SDL_SetRenderDrawColor(state.renderer, 20, 20, 30, 255);


        // Title
        drawTextCentered(state,"FootBall SAKOBAN", cx, 60, WHITE);

        // Level list
        constexpr float START_Y = 200.0f;
        constexpr float LINE_H  = 44.0f;

        for (int i = 0; i < (int)game.levelPaths.size(); ++i) {
            bool isSelected = (i == game.selected);

            std::string label = "Level " + std::to_string(i + 1);
            if (isSelected) label = "> " + label + " <";

            drawTextCentered(state, label, cx,
                             START_Y + i * LINE_H, isSelected ? YELLOW : GREY);
        }

        // Hint at the bottom
        drawTextCentered(state, "Up/Down: choose   Space: play   Esc: quit",
                         cx, state.logH - 50, GREY);
    }
    void drawWon(SDLState& state, const Game& game, Action last)
    {
        constexpr SDL_Color WHITE  {255, 255, 255, 255};
        constexpr SDL_Color YELLOW {255, 220,  60, 255};
        constexpr SDL_Color GREY   {200, 200, 200, 255};

        const float cx = (float)state.logW / 2;
        const float cy = (float)state.logH / 2;

        // keep the solved board visible behind the text
        drawLevel(state, game.level, last);

        // dim overlay
        SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 170);
        SDL_FRect overlay{0, 0, (float)state.logW, (float)state.logH};
        SDL_RenderFillRect(state.renderer, &overlay);
        SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);

        // stats from the undo history
        int moves  = (int)game.level.history.size();
        int pushes = (int)std::count_if(game.level.history.begin(), game.level.history.end(),
                                        [](const Move& m) { return m.pushedBox; });

        drawTextCentered(state, "LEVEL COMPLETE!", cx, cy - 70, YELLOW);
        drawTextCentered(state, "Moves: " + std::to_string(moves) +
                                "   Pushes: " + std::to_string(pushes), cx, cy - 10, WHITE);
        drawTextCentered(state, "Press any key to return to the menu", cx, cy + 50, GREY);
    }
};



int main(int argc, char* argv[])
{
    SDLState state = SDLState();
    if (init(state) == false)
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
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
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
                    if (event.key.key == SDLK_ESCAPE) running = false;
                    if (event.key.key == SDLK_UP)   game.selected = std::max(0, game.selected - 1);
                    if (event.key.key == SDLK_DOWN) game.selected = std::min((int)game.levelPaths.size() - 1, game.selected + 1);
                    if (event.key.key == SDLK_SPACE)
                        if (!game.startLevel())
                            running = false;
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
                    case Action::Quit: running = false; break;
                    default: break;
                }
            }
            if (game.screen == Screen::Won && event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE) running = false;
                else game.screen = Screen::Menu;
            }
        }

        // draw the level
        SDL_SetRenderDrawColor(state.renderer, 196, 219, 122, 255);
        SDL_RenderClear(state.renderer);

        if (game.screen == Screen::Menu)
        {
            res.drawMenu(state, game);
        }
        if (game.screen == Screen::Playing)
        {
            res.drawLevel(state, game.level, lastAction);

            // if level is solved end the game
            if (game.level.isSolved())
            {
                game.screen = Screen::Won;
            }
        }
        if (game.screen == Screen::Won)
        {
            res.drawWon(state, game, lastAction);
        }

        SDL_RenderPresent(state.renderer);
    }

    res.unload();
    cleanup(state);
    return 0;
}

bool init(SDLState& state)
{
    bool flag = true;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL Initialization failed: %s", SDL_GetError());
        return -1;
    }

    state.window = SDL_CreateWindow("Sokoban", state.width, state.height, SDL_WINDOW_RESIZABLE);
    if (!state.window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // create renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        flag = false;
    }

    TTF_Init();                                           // once, after SDL_Init


    // CONFIGURE LOGICAL PRESENTATION
    SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    // this pres style retains the 640X320 res regardless of the resizing of the window by introducing black bars
    return flag;
}
void cleanup(SDLState& state)
{
    SDL_DestroyWindow(state.window);
    SDL_DestroyRenderer(state.renderer);
    SDL_Quit();
}