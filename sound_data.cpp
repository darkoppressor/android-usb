#include "sound_data.h"

using namespace std;

Sound_Data::Sound_Data(){
    chunk=0;
}

void Sound_Data::load_sound(string filename){
    chunk=Mix_LoadWAV(filename.c_str());
}

void Sound_Data::unload_sound(){
    Mix_FreeChunk(chunk);
}
