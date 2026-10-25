// this stores the window, renderer, window width, height, logw , logh

#pragma once

#include <SDL3/SDL.h>

struct SDLState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height, logW, logH;

    static bool init(SDLState& state)
    {
        bool flag = true;
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL Initialization failed: %s", SDL_GetError());
            return false;
        }

        state.window = SDL_CreateWindow("Sokoban", state.width, state.height, SDL_WINDOW_RESIZABLE);
        if (!state.window) {
            SDL_Log("Window creation failed: %s", SDL_GetError());
            SDL_Quit();
            return false;
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

    static void cleanup(SDLState& state)
    {
        SDL_DestroyWindow(state.window);
        SDL_DestroyRenderer(state.renderer);
        SDL_Quit();
    }

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