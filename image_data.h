#ifndef image_data_h
#define image_data_h

#include <string>

#include <SDL.h>

class Image_Data{
public:

    SDL_Texture* texture;
    double w;
    double h;

    Image_Data();

    void load_image(std::string filename);

    void load_image(SDL_Surface* surface);

    void unload_image();
};

#endif
