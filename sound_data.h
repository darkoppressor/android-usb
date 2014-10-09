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
