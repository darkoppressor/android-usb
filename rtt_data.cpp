#include "rtt_data.h"
#include "world.h"

using namespace std;

Rtt_Data::Rtt_Data(){
    texture=0;
    w=0.0;
    h=0.0;
}

void Rtt_Data::create_texture(double get_w,double get_h){
    w=get_w;
    h=get_h;

    texture=SDL_CreateTexture(main_window.renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,w,h);

    SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
}

void Rtt_Data::unload_texture(){
    SDL_DestroyTexture(texture);

    texture=0;
    w=0.0;
    h=0.0;
}

void Rtt_Data::make_render_target(){
    if(SDL_SetRenderTarget(main_window.renderer,texture)!=0){
        string msg="Unable to set render target: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    SDL_SetRenderDrawColor(main_window.renderer,0,0,0,0);
    SDL_RenderClear(main_window.renderer);
}

void Rtt_Data::reset_render_target(){
    if(SDL_SetRenderTarget(main_window.renderer,NULL)!=0){
        string msg="Unable to set render target: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }
}
