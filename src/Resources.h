//
// Created by anton on 10/25/2026.
//

#ifndef SOKOBAN_RESOURCES_H
#define SOKOBAN_RESOURCES_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>
#include "SDLState.h"

class Resources
{
public:
    std::vector<SDL_Texture*> textures;
    SDL_Texture* texPlayer = nullptr;
    SDL_Texture* texBox = nullptr;
    TTF_Font* font = nullptr;

    void load(SDLState &);
    void unload();

private:
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filepath);

};




#endif //SOKOBAN_RESOURCES_H
