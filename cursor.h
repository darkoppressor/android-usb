#ifndef cursor_h
#define cursor_h

#include "sprite.h"

#include <string>

#include <SDL.h>

class Cursor{
public:

    std::string name;

    Sprite sprite;

    SDL_Cursor* hw_cursor;

    Cursor();

    void load_hw_cursor();
    void free_hw_cursor();

    void animate();
    void render(int x,int y);
};

#endif
