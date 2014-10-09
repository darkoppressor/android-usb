#include "world.h"

using namespace std;

bool world_loaded=false;

bool save_location_loaded=false;

string CURRENT_WORKING_DIRECTORY="./";
string CHECKSUM="";

double UPDATE_RATE=60.0;
double SKIP_TICKS=1000.0/UPDATE_RATE;

double UPDATE_RATE_RENDER=200.0;
double SKIP_TICKS_RENDER=1000.0/UPDATE_RATE_RENDER;

Message_Log message_log;

String_Stuff string_stuff;

Game_Window main_window;

SDL_Event event;

Engine_Interface engine_interface;

Update update;

File_IO file_io;

Network network;

Image image;

Rtt_Manager rtt_manager;

Music music;

Sound sound_system;

Game game;

bool load_world(){
    engine_interface.load_data_game_options();

    if(!engine_interface.load_options()){
        return false;
    }

    if(!main_window.init()){
        return false;
    }

    image.load_images();

    sound_system.load_sounds();

    music.prepare_tracks();

    engine_interface.load_data_main();

    rtt_manager.create_textures();

    engine_interface.console.setup(false);
    engine_interface.chat.setup(true);

    if(!engine_interface.load_game_commands()){
        return false;
    }

    engine_interface.load_servers();

    //To be safe, this should be at the very bottom of load_world().
    image.set_error_image();

    world_loaded=true;

    return true;
}

void unload_world(){
    if(world_loaded){
        world_loaded=false;

        image.unload_images();

        rtt_manager.unload_textures();

        sound_system.unload_sounds();

        music.unload_tracks();

        engine_interface.unload_data();
    }
}
