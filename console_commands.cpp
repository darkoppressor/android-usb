#include "console.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Console::setup_commands(){
    commands.clear();

    commands.push_back("commands");
    commands.push_back("options");
    commands.push_back("echo");
    commands.push_back("clear");
    commands.push_back("reload");
    commands.push_back("about");
    commands.push_back("quit");
    commands.push_back("play");
    commands.push_back("toast");

    commands.push_back("cl_logic_update_rate");
    commands.push_back("cl_frame_rate_max");

    commands.push_back("cl_screen_width");
    commands.push_back("cl_screen_height");
    commands.push_back("cl_fullscreen_state");
    commands.push_back("cl_fullscreen_mode");

    commands.push_back("cl_vsync");
    commands.push_back("cl_accelerometer_controller");
    commands.push_back("cl_touch_controller");
    commands.push_back("cl_touch_controller_opacity");
    commands.push_back("cl_font_shadows");
    commands.push_back("cl_screen_keyboard");
    commands.push_back("cl_hw_cursor");
    commands.push_back("cl_bind_cursor");

    commands.push_back("cl_fps");
    commands.push_back("cl_dev");
    commands.push_back("cl_volume_global");
    commands.push_back("cl_volume_sound");
    commands.push_back("cl_volume_music");
    commands.push_back("cl_mute_global");
    commands.push_back("cl_mute_sound");
    commands.push_back("cl_mute_music");

    for(int i=0;i<engine_interface.game_options.size();i++){
        commands.push_back(engine_interface.game_options[i].name);
    }
}

vector<string> Console::parse_input(string str_input){
    vector<string> input_list;

    //Are we reading options?
    bool options=false;

    //Are we reading a string in quotes?
    bool quote=false;

    //The first string is always the options, even if there aren't any.
    input_list.push_back("");

    //The second string is always the first input, even if there isn't any.
    input_list.push_back("");

    for(int i=0;i<str_input.length();i++){
        if(!quote && !options && str_input[i]=='-'){
            options=true;

            continue;
        }
        else if(!quote && str_input[i]==' '){
            options=false;

            input_list.push_back("");

            continue;
        }
        else if(!options && str_input[i]=='\"'){
            quote=!quote;

            continue;
        }

        if(options){
            input_list[0]+=str_input[i];
        }
        else{
            input_list[input_list.size()-1]+=str_input[i];
        }
    }

    //Remove any empty strings, ignoring the first two.
    for(int i=2;i<input_list.size();i++){
        if(input_list[i].length()==0){
            input_list.erase(input_list.begin()+i);

            i--;
        }
    }

    if(input_list[1].length()==0){
        if(input_list.size()>2){
            input_list[1]=input_list[2];

            input_list.erase(input_list.begin()+2);
        }
    }

    return input_list;
}

bool Console::input_has_option(string option,string options){
    if(option.length()==0){
        option=" ";
    }

    for(int i=0;i<options.length();i++){
        if(option[0]==options[i]){
            return true;
        }
    }

    return false;
}

void Console::do_command(){
    if(!chat && info_input.text.length()>0){
        //Forget the oldest command string.
        recalled_command_strings.erase(recalled_command_strings.begin());

        //Remember this string.
        recalled_command_strings.push_back(info_input.text);

        reset_current_recalled_command_string();

        vector<string> command_list;
        boost::algorithm::split(command_list,info_input.text,boost::algorithm::is_any_of(";"));

        for(int n=0;n<command_list.size();n++){
            info_input.text=command_list[n];

            boost::algorithm::trim(info_input.text);

            string full_command=info_input.text;

            //The command itself.
            string command="";

            //Any input given to this command.
            vector<string> command_input;

            for(int i=0;i<commands.size();i++){
                if(boost::algorithm::starts_with(info_input.text,commands[i])){
                    command=commands[i];

                    boost::algorithm::erase_first(info_input.text,commands[i]);
                    boost::algorithm::trim(info_input.text);

                    command_input=parse_input(info_input.text);

                    break;
                }
            }

            if(command.length()>0){
                if(command=="commands"){
                    for(int i=0;i<commands.size();i++){
                        if(!boost::algorithm::starts_with(commands[i],"cl_")){
                            add_text(commands[i]);
                        }
                    }
                }
                else if(command=="options"){
                    for(int i=0;i<commands.size();i++){
                        if(boost::algorithm::starts_with(commands[i],"cl_")){
                            add_text(commands[i]);
                        }
                    }
                }
                else if(command=="echo"){
                    string echo_text="";

                    for(int i=1;i<command_input.size();i++){
                        echo_text+=command_input[i]+" ";
                    }

                    boost::algorithm::trim(echo_text);

                    add_text(echo_text);
                }
                else if(command=="clear"){
                    clear_text();
                    engine_interface.chat.clear_text();
                }
                else if(command=="reload"){
                    add_text("Reloading UI...");

                    engine_interface.reload();
                }
                else if(command=="about"){
                    add_text(engine_interface.game_title+"\nDeveloped by: "+engine_interface.developer+"\nVersion: "+engine_interface.get_version()+"\nChecksum: "+CHECKSUM+"\nBuilt on: "+engine_interface.get_build_date());
                }
                else if(command=="quit"){
                    add_text("Be seeing you...");

                    engine_interface.quit();
                }
                else if(command=="play"){
                    if(command_input[1].length()>0){
                        if(sound_system.get_sound(command_input[1])!=0){
                            add_text("Playing sound '"+command_input[1]+"'");

                            sound_system.play_sound(command_input[1]);
                        }
                        else{
                            add_text("Unknown sound '"+command_input[1]+"'");
                        }
                    }
                }
                else if(command=="toast"){
                    string toast_length="";
                    int custom_toast_length=-1;

                    if(command_input.size()>2){
                        custom_toast_length=string_stuff.string_to_long(command_input[2]);
                    }

                    if(input_has_option("s",command_input[0])){
                        toast_length="short";
                    }
                    else if(input_has_option("m",command_input[0])){
                        toast_length="medium";
                    }
                    else if(input_has_option("l",command_input[0])){
                        toast_length="long";
                    }

                    engine_interface.make_toast(command_input[1],toast_length,custom_toast_length);
                }

                //If the command is a valid one but is not handled above.
                else{
                    for(int i=0;i<commands.size();i++){
                        if(command==commands[i]){
                            if(command_input[1].length()==0){
                                add_text("\""+commands[i]+"\" = \""+engine_interface.get_option_value(commands[i])+"\"");

                                add_text(" - "+engine_interface.get_option_description(commands[i]));
                            }
                            else{
                                engine_interface.change_option(commands[i],command_input[1]);

                                add_text("\""+commands[i]+"\" set to \""+engine_interface.get_option_value(commands[i])+"\"");
                            }

                            break;
                        }
                    }
                }
            }
            else if(info_input.text.length()>=2 && (isdigit(info_input.text[0]) || info_input.text[0]=='-' || info_input.text[0]=='d')){
                vector<string> dice_command;

                if(info_input.text[0]=='d'){
                    dice_command.push_back("1");
                    string ds=info_input.text;
                    ds.erase(ds.begin());
                    dice_command.push_back(ds);
                }
                else{
                    boost::algorithm::split(dice_command,info_input.text,boost::algorithm::is_any_of("d"));
                }

                if(dice_command.size()==2 && string_stuff.is_number(dice_command[0]) && string_stuff.is_number(dice_command[1])){
                    int dice_count=string_stuff.string_to_long(dice_command[0]);
                    int dice_sides=string_stuff.string_to_long(dice_command[1]);

                    if(dice_count>0 && dice_sides>0){
                        int total=0;

                        for(int i=0;i<dice_count;i++){
                            total+=engine_interface.rng.random_range(1,dice_sides);
                        }

                        add_text("Rolling "+info_input.text+": "+string_stuff.num_to_string(total));
                    }
                    else if(dice_count<=0){
                        add_text("You cannot roll "+string_stuff.num_to_string(dice_count)+" dice.");
                    }
                    else if(dice_sides<=0){
                        add_text("A "+string_stuff.num_to_string(dice_sides)+"-sided die?");
                    }
                }
                else{
                    add_text("Unknown command: \""+info_input.text+"\"");
                }
            }
            else{
                add_text("Unknown command: \""+info_input.text+"\"");
            }

            info_input.text="";
        }
    }
}
