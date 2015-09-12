/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef sound_data_h
#define sound_data_h

#include <string>

#include <SDL_mixer.h>

class Sound_Data{
public:

    Mix_Chunk* chunk;

    Sound_Data();

    void load_sound(std::string filename);

    void unload_sound();
};

#endif
