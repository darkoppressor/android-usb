#include "special_info.h"
#include "world.h"

using namespace std;

string Special_Info::get_special_info_text(string special_info){
    string text="";

    if(special_info.length()>0){
        if(special_info=="configure_command"){
            if(engine_interface.configure_command!=-1 && engine_interface.configure_command<engine_interface.game_commands.size()){
                text+="Inputs currently bound to \""+engine_interface.game_commands[engine_interface.configure_command].title+"\":"+"\n\n";

                const char* ckey=SDL_GetScancodeName(engine_interface.game_commands[engine_interface.configure_command].key);
                const char* cbutton=SDL_GameControllerGetStringForButton(engine_interface.game_commands[engine_interface.configure_command].button);
                const char* caxis=SDL_GameControllerGetStringForAxis(engine_interface.game_commands[engine_interface.configure_command].axis);

                bool allow_keys_and_buttons=true;
                bool allow_axes=true;
                if(caxis!=0 && engine_interface.game_commands[engine_interface.configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                    allow_keys_and_buttons=false;
                }
                else{
                    allow_axes=false;
                }

                if(allow_keys_and_buttons){
                    text+="Keyboard Key: ";
                    if(ckey!=0 && engine_interface.game_commands[engine_interface.configure_command].key!=SDL_SCANCODE_UNKNOWN){
                        text+=string_stuff.first_letter_capital(ckey);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";

                    text+="Controller Button: ";
                    if(cbutton!=0 && engine_interface.game_commands[engine_interface.configure_command].button!=SDL_CONTROLLER_BUTTON_INVALID){
                        text+=string_stuff.first_letter_capital(cbutton);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";
                }

                if(allow_axes){
                    text+="Controller Axis: ";
                    if(caxis!=0 && engine_interface.game_commands[engine_interface.configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                        text+=string_stuff.first_letter_capital(caxis);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";
                }
            }
        }
        else if(special_info=="browser_current_path"){
            text=game.world.current_path;
        }
        else if(special_info=="browser_selected_files"){
            if(game.world.selected_files.size()>0){
                text=string_stuff.num_to_string(game.world.selected_files.size())+" file";

                if(game.world.selected_files.size()>1){
                    text+="s";
                }

                text+=" selected";
            }
        }
        else if(special_info=="rename_file"){
            if(game.world.selected_files.size()==1){
                text=game.world.selected_files[0];
            }
        }
        else if(special_info=="create_directory"){
            text="Creating new directory in '"+game.world.get_current_path()+"':";
        }
        else if(special_info=="confirm_delete_selected"){
            text="Are you sure you want to delete ";
            if(game.world.selected_files.size()==1){
                text+="this file?";
            }
            else{
                text+="these "+string_stuff.num_to_string(game.world.selected_files.size())+" files?";
            }
        }
        else{
            message_log.add_error("Invalid special info text: '"+special_info+"'");
        }
    }

    return text;
}

string Special_Info::get_special_info_sprite(string special_info){
    string str_sprite_name="";

    if(special_info.length()>0){
        if(special_info=="example"){
            str_sprite_name="";
        }
        else{
            message_log.add_error("Invalid special info sprite: '"+special_info+"'");
        }
    }

    return str_sprite_name;
}
