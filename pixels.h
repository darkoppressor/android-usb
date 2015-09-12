/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef pixels_h
#define pixels_h

#include "color.h"

#include <SDL.h>

Color surface_get_pixel(SDL_Surface* surface,int x,int y);

void surface_put_pixel(SDL_Surface* surface,int x,int y,Color color);

#endif
