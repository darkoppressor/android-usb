#ifndef font_h
#define font_h

#include "sprite.h"
#include "collision.h"

#include <SDL.h>
#include <string>

static const SDL_Rect FONT_DEFAULT_ALLOWED_AREA={-1,-1,0,0};

class Bitmap_Font{
public:

    //The individual characters in the texture.
    Collision_Rect chars[256];

    std::string name;

    Sprite sprite;

    //Character spacing.
    int spacing_x;
    //Line spacing.
    int spacing_y;

    int gui_padding_x;
    int gui_padding_y;

    //The distance the drop shadow should be rendered from the font's location.
    //A distance of 0 means no shadow.
    int shadow_distance;

    Bitmap_Font();

    void build_font();

    double get_letter_width();
    double get_letter_height();

    void show(double x,double y,std::string text,std::string font_color,double opacity=1.0,double scale_x=1.0,double scale_y=1.0,double angle=0.0,SDL_Rect allowed_area=FONT_DEFAULT_ALLOWED_AREA);
};

#endif
