#include "sound.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Sound::load_sounds(){
    //Look through all of the files in the directory.
    for(File_IO_Directory_Iterator it("data/sounds");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_name=it.get_file_name();

            boost::algorithm::trim(file_name);

            sound_names.push_back(file_name);
        }
    }

    for(int i=0;i<sound_names.size();i++){
        sounds.push_back(Sound_Data());

        sounds[sounds.size()-1].load_sound("data/sounds/"+sound_names[i]);

        sound_names[i].erase(sound_names[i].end()-4,sound_names[i].end());
    }
}

void Sound::unload_sounds(){
    for(int i=0;i<sounds.size();i++){
        sounds[i].unload_sound();
    }

    sounds.clear();
    sound_names.clear();
}

int Sound::modify_sound_volume(int volume){
    //If either the global mute or the sound effects mute is enabled.
    if(engine_interface.option_mute_global || engine_interface.option_mute_sound){
        return 0;
    }

    double new_volume=volume;

    new_volume*=engine_interface.option_volume_sound;

    new_volume*=engine_interface.option_volume_global;

    return (int)new_volume;
}

int Sound::get_free_channel(){
    int allocated_channels=Mix_AllocateChannels(-1);

    for(int i=0;i<allocated_channels;i++){
        //Check through all music tracks currently loaded.
        bool channel_taken_by_music_track=false;
        for(int n=0;n<music.tracks.size();n++){
            //If this channel is equal to this track's channel, then this channel is taken.
            if(i==music.tracks[n].channel){
                channel_taken_by_music_track=true;
            }
        }

        //If the channel is already playing.
        if(Mix_Playing(i)==1){
        }
        //If this channel is already reserved by a music track.
        else if(channel_taken_by_music_track){
        }
        //All tests were passed, and this channel is currently free, so return it.
        else{
            return i;
        }
    }

    return -1;
}

void Sound::play_sound(string sound_name){
    Sound_Data* ptr_sound=get_sound(sound_name);

    int volume=128;

    volume=modify_sound_volume(volume);

    _play_sound(ptr_sound,volume);
}

void Sound::play_sound(string sound_name,double sound_x,double sound_y,double falloff){
    Sound_Data* ptr_sound=get_sound(sound_name);

    int volume=128;

    if(falloff<0.0){
        falloff=engine_interface.sound_falloff;
    }

    falloff*=game.camera_zoom;

    double sound_center_x=game.camera.center_x();
    double sound_center_y=game.camera.center_y();

    if(game.in_progress){
        /**sound_center_x=game.world.example_player.circle.x;
        sound_center_y=game.world.example_player.circle.y;*/
    }

    double sound_position_x=sound_x*game.camera_zoom;
    double sound_position_y=sound_y*game.camera_zoom;

    double sound_distance=distance_between_points(sound_center_x,sound_center_y,sound_position_x,sound_position_y);
    if(sound_distance<1.0){
        sound_distance=0.0;
    }

    if(sound_distance==0.0){
        volume=128;
    }
    else{
        volume=(int)floor((falloff/sound_distance)*falloff);
    }

    if(volume>128){
        volume=128;
    }

    volume=modify_sound_volume(volume);

    _play_sound(ptr_sound,volume);
}

void Sound::_play_sound(Sound_Data* ptr_sound,int volume){
    if(ptr_sound!=0 && ptr_sound->chunk!=0){
        if(volume!=0){
            int channel=get_free_channel();

            if(channel!=-1){
                Mix_Volume(channel,volume);
                Mix_PlayChannel(channel,ptr_sound->chunk,0);
            }
        }
    }
}

Sound_Data* Sound::get_sound(string sound_name){
    Sound_Data* ptr_sound=0;

    for(int i=0;i<sound_names.size();i++){
        if(sound_names[i]==sound_name){
            ptr_sound=&sounds[i];

            break;
        }
    }

    if(ptr_sound==0){
        message_log.add_error("Error accessing sound '"+sound_name+"'");
    }

    return ptr_sound;
}
