#ifndef tooltip_h
#define tooltip_h

#include <string>

class Tooltip{
public:

    bool on;

    std::string message;

    std::string font;

    short x,y;
    short w,h;

    Tooltip();

    void setup(std::string get_message,int mouse_x,int mouse_y);

    void set_dimensions();

    void render();
};

#endif
