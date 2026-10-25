//
// Created by anton on 10/25/2026.
//

#include "Resources.h"
void Resources::load(SDLState &state)
{
    texPlayer = loadTexture(state.renderer, "../data/footballer.png");
    texBox = loadTexture(state.renderer, "../data/ball.png");
    font = TTF_OpenFont("../data/font.ttf", 20); // path + point size
}

void Resources::unload()
{
    for (SDL_Texture* tex : textures)
    {
        SDL_DestroyTexture(tex);
    }
    TTF_CloseFont(font);
    TTF_Quit();
}

SDL_Texture* Resources::loadTexture(SDL_Renderer* renderer, const std::string& filepath)
{
    SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());

    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    // this changes the texture scale mode to nearest so pixels don't blend into each other

    textures.push_back(tex);
    return tex;
}


