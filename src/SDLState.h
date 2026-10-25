// this stores the window, renderer, window width, height, logw , logh

#pragma once

#include <SDL3/SDL.h>
#include "Level.h"

struct SDLState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height, logW, logH;

    SDLState()
    {
        width = 1024;
        height = 900;
        logW = 512;
        logH = 512;
    }
};

//sprites are 32 X 32 px
//grid of 32 sprites at max with logical dimensions at 1024
//map will be stored on a 32 X 32 matrix