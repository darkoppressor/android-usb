#ifndef pixels_h
#define pixels_h

#include "color.h"

#include <SDL.h>

Color surface_get_pixel(SDL_Surface* surface,int x,int y);

void surface_put_pixel(SDL_Surface* surface,int x,int y,Color color);

#endif
