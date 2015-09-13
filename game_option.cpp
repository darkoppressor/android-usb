/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "game_option.h"
#include "world.h"

using namespace std;

Game_Option::Game_Option(){
    name="";
    description="";
}

string Game_Option::get_value(){
    if(name=="cl_adb_path"){
        return game.option_adb_path;
    }
    else if(name=="cl_starting_path"){
        return game.option_starting_path;
    }
    else if(name=="cl_hidden_files"){
        return string_stuff.bool_to_string(game.option_hidden_files);
    }
    else if(name=="cl_effect_limit"){
        return string_stuff.num_to_string(game.option_effect_limit);
    }
    else if(name=="cl_screen_shake"){
        return string_stuff.bool_to_string(game.option_screen_shake);
    }
    else if(name=="cl_camera_speed"){
        return string_stuff.num_to_string(game.option_camera_speed);
    }
    else if(name=="cl_camera_zoom"){
        return string_stuff.num_to_string(game.option_camera_zoom);
    }
    else if(name=="cl_name"){
        return game.option_name;
    }
    else if(name=="cl_chat_timestamps"){
        return string_stuff.bool_to_string(game.option_chat_timestamps);
    }
    else if(name=="cl_network_rate_bytes"){
        return string_stuff.num_to_string(network.rate_bytes);
    }
    else if(name=="cl_network_rate_updates"){
        return string_stuff.num_to_string(network.rate_updates);
    }
    else if(name=="cl_network_rate_commands"){
        return string_stuff.num_to_string(network.rate_commands);
    }
    else if(name=="sv_max_players"){
        return string_stuff.num_to_string(game.option_max_players);
    }
    else if(name=="sv_network_max_clients"){
        return string_stuff.num_to_string(network.max_clients);
    }
    else if(name=="sv_network_port"){
        return string_stuff.num_to_string(network.port);
    }
    else if(name=="sv_network_password"){
        return network.password;
    }
    else if(name=="sv_network_frequent_connection_protection"){
        return string_stuff.bool_to_string(network.frequent_connection_protection);
    }
    else if(name=="sv_network_ignore_checksum"){
        return string_stuff.bool_to_string(network.ignore_checksum);
    }
    else if(name=="sv_network_rate_bytes_min"){
        return string_stuff.num_to_string(network.rate_bytes_min);
    }
    else if(name=="sv_network_rate_bytes_max"){
        return string_stuff.num_to_string(network.rate_bytes_max);
    }
    else if(name=="sv_network_rate_updates_min"){
        return string_stuff.num_to_string(network.rate_updates_min);
    }
    else if(name=="sv_network_rate_updates_max"){
        return string_stuff.num_to_string(network.rate_updates_max);
    }
    else{
        message_log.add_error("Invalid game option: '"+name+"'");

        return "";
    }
}

void Game_Option::set_value(string new_value){
    if(name=="cl_adb_path"){
        game.option_adb_path=new_value;
    }
    else if(name=="cl_starting_path"){
        game.option_starting_path=new_value;
    }
    else if(name=="cl_hidden_files"){
        game.option_hidden_files=string_stuff.string_to_bool(new_value);
    }
    else if(name=="cl_effect_limit"){
        game.option_effect_limit=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="cl_screen_shake"){
        game.option_screen_shake=string_stuff.string_to_bool(new_value);
    }
    else if(name=="cl_camera_speed"){
        game.option_camera_speed=string_stuff.string_to_double(new_value);
    }
    else if(name=="cl_camera_zoom"){
        game.option_camera_zoom=string_stuff.string_to_double(new_value);
    }
    else if(name=="cl_name"){
        string old_name=game.option_name;

        game.option_name=new_value;

        network.send_name_change(old_name,game.option_name,true);
        network.send_client_data();
    }
    else if(name=="cl_chat_timestamps"){
        game.option_chat_timestamps=string_stuff.string_to_bool(new_value);
    }
    else if(name=="cl_network_rate_bytes"){
        network.rate_bytes=string_stuff.string_to_unsigned_long(new_value);

        network.send_client_data();
    }
    else if(name=="cl_network_rate_updates"){
        network.rate_updates=string_stuff.string_to_unsigned_long(new_value);

        network.send_client_data();
    }
    else if(name=="cl_network_rate_commands"){
        network.rate_commands=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_max_players"){
        game.option_max_players=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_max_clients"){
        network.max_clients=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_port"){
        network.port=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_password"){
        network.password=new_value;
    }
    else if(name=="sv_network_frequent_connection_protection"){
        network.frequent_connection_protection=string_stuff.string_to_bool(new_value);
    }
    else if(name=="sv_network_ignore_checksum"){
        network.ignore_checksum=string_stuff.string_to_bool(new_value);
    }
    else if(name=="sv_network_rate_bytes_min"){
        network.rate_bytes_min=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_bytes_max"){
        network.rate_bytes_max=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_updates_min"){
        network.rate_updates_min=string_stuff.string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_updates_max"){
        network.rate_updates_max=string_stuff.string_to_unsigned_long(new_value);
    }
}
