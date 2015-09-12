/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef game_window_h
#define game_window_h

#include <SDL.h>

class Game_Window{
    private:
    public:

    Game_Window();

    void cleanup_video();

    bool set_resolution(short* desired_resolution_x,short* desired_resolution_y);

    bool initialize_video();

    bool init_sdl();

    void set_sdl_hints();

    //Returns true if everything initialized properly.
    //Returns false if initializing failed.
    bool init();

    void screenshot();

    void reinitialize();

    //The logical resolution of the game window.
    short SCREEN_WIDTH;
    short SCREEN_HEIGHT;

    SDL_Window* screen;

    SDL_Renderer* renderer;

    SDL_Surface* icon;
    Uint32 icon_colorkey;
};

#endif
