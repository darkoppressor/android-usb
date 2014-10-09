#ifndef rtt_data_h
#define rtt_data_h

#include <string>

#include <SDL.h>

class Rtt_Data{
public:

    SDL_Texture* texture;
    double w;
    double h;

    Rtt_Data();

    void create_texture(double get_w,double get_h);
    void unload_texture();

    void make_render_target();
    void reset_render_target();
};

#endif
