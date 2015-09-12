/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef image_h
#define image_h

#include "image_data.h"

#include <vector>
#include <string>

#include <SDL.h>

class Image{
public:

    Image_Data* error_image;

    std::vector<Image_Data> images;

    std::vector<std::string> image_names;

    void set_error_image();

    void add_image(std::string name,SDL_Surface* surface);

    void load_images();
    void unload_images();

    Image_Data* get_image(std::string image_name);
};

#endif
