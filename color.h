#ifndef color_h
#define color_h

#include <string>

struct Color_Data{
    double red;
    double green;
    double blue;
    double alpha;
};

class Color{
public:

    std::string name;

    Color_Data rgb;

    Color();

    double get_red();
    double get_green();
    double get_blue();
    double get_alpha();

    short get_red_short();
    short get_green_short();
    short get_blue_short();
    short get_alpha_short();

    void set_rgb(short red,short green,short blue,short alpha=255);

    bool is_equal(Color* color);
};

#endif
