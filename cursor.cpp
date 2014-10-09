#include "cursor.h"
#include "render.h"
#include "world.h"

using namespace std;

Cursor::Cursor(){
    name="";

    hw_cursor=0;
}

void Cursor::load_hw_cursor(){
    SDL_Surface* surface=load_image("data/images/"+sprite.name+".png");
    SDL_Surface* surface_final=0;

    if(sprite.is_animated()){
        int width=sprite.get_width();
        int height=sprite.get_height();

        uint32_t rmask,gmask,bmask,amask;
        engine_interface.get_rgba_masks(&rmask,&gmask,&bmask,&amask);

        surface_final=SDL_CreateRGBSurface(0,width,height,32,rmask,gmask,bmask,amask);

        if(SDL_MUSTLOCK(surface_final)){
            SDL_LockSurface(surface_final);
        }

        for(int x=0;x<width;x++){
            for(int y=0;y<height;y++){
                surface_put_pixel(surface_final,x,y,surface_get_pixel(surface,x,y));
            }
        }

        if(SDL_MUSTLOCK(surface_final)){
            SDL_UnlockSurface(surface_final);
        }

        SDL_FreeSurface(surface);
    }
    else{
        surface_final=surface;
    }

    hw_cursor=SDL_CreateColorCursor(surface_final,0,0);

    SDL_FreeSurface(surface_final);
}

void Cursor::free_hw_cursor(){
    SDL_FreeCursor(hw_cursor);
}

void Cursor::animate(){
    sprite.animate();
}

void Cursor::render(int x,int y){
    if(engine_interface.option_hw_cursor){
        SDL_SetCursor(hw_cursor);

        SDL_ShowCursor(SDL_ENABLE);
    }
    else{
        SDL_ShowCursor(SDL_DISABLE);

        sprite.render(x,y);
    }
}
