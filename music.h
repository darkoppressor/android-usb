/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef music_h
#define music_h

#include <vector>
#include <string>

#include <SDL_mixer.h>

struct Music_Data{
    Mix_Chunk* track;
    bool playing;
    int channel;
    double volume;
    double fade_speed;
};

class Music{
public:

    std::vector<Music_Data> tracks;

    std::vector<std::string> track_names;

    friend int get_free_channel();

    void set_track_volumes();

    //Modifies volume by applying the appropriate audio option multipliers.
    int modify_music_volume(int volume);

    //Handle the fading in and out of tracks.
    void fadein_tracks();
    void fadeout_tracks();

    //Turn off the currently playing track, if any, and start playing track.
    void play_track(std::string track_name,double fade_speed=0.01);

    //Turn off the currently playing track, if any.
    void stop_track(double fade_speed=0.01);

    void restart_track(std::string track_name);

    //Prepare all tracks.
    //Does not load anything.
    void prepare_tracks();

    //Load a single track.
    void load_track(std::string track_path,std::string track_name);

    //Unload a single track.
    void unload_track(int track_ident);

    //Unload all tracks.
    void unload_tracks();

    //Returns true if the passed track is currently the playing track.
    //Returns false if the passed track is not playing.
    bool track_is_playing(int track_ident);

    int get_track_ident(std::string track_name);
};

#endif
