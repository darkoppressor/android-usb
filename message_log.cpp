/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "message_log.h"
#include "world.h"

#include <cstdio>
#include <fstream>

using namespace std;

void Message_Log::clear_error_log(){
    file_io.remove_file(engine_interface.get_home_directory()+"error_log.txt");
}

void Message_Log::add_error(string message){
    message=engine_interface.get_timestamp()+" "+message;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,SDL_LOG_PRIORITY_ERROR,message.c_str());

    if(world_loaded){
        engine_interface.console.add_text(message);
    }

    if(save_location_loaded){
        stringstream save("");

        save<<message<<"\n";

        file_io.save_file(engine_interface.get_home_directory()+"error_log.txt",save.str(),true);
    }
}

void Message_Log::add_log(string message){
    message=engine_interface.get_timestamp()+" "+message;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,SDL_LOG_PRIORITY_INFO,message.c_str());

    if(world_loaded){
        engine_interface.console.add_text(message);
    }
}
