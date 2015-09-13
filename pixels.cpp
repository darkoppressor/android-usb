/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "pixels.h"

using namespace std;

Color surface_get_pixel(SDL_Surface* surface,int x,int y){
    //Convert the pixels to 32 bit.
    uint32_t* pixels=(uint32_t*)surface->pixels;

    //Get the requested pixel.
    uint32_t pixel=pixels[(y*surface->w)+x];

    uint8_t check_red;
    uint8_t check_green;
    uint8_t check_blue;
    uint8_t check_alpha;
    SDL_GetRGBA(pixel,surface->format,&check_red,&check_green,&check_blue,&check_alpha);

    Color color;
    color.set_rgb(check_red,check_green,check_blue,check_alpha);

    return color;
}

void surface_put_pixel(SDL_Surface* surface,int x,int y,Color color){
    //Convert the pixels to 32 bit.
    uint32_t* pixels=(uint32_t*)surface->pixels;

    uint32_t pixel=SDL_MapRGBA(surface->format,color.get_red_short(),color.get_green_short(),color.get_blue_short(),color.get_alpha_short());

    //Set the pixel.
    pixels[(y*surface->w)+x]=pixel;
}
