#include "image_data.h"
#include "render.h"
#include "world.h"

using namespace std;

Image_Data::Image_Data(){
    texture=0;
    w=0.0;
    h=0.0;
}

void Image_Data::load_image(string filename){
    texture=load_texture(filename,this);

    SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
}

void Image_Data::load_image(SDL_Surface* surface){
    w=surface->w;
    h=surface->h;

    texture=SDL_CreateTextureFromSurface(main_window.renderer,surface);

    SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
}

void Image_Data::unload_image(){
    SDL_DestroyTexture(texture);

    texture=0;
    w=0.0;
    h=0.0;
}
