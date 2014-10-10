#include "game.h"
#include "world.h"

using namespace std;

void Game::prepare_for_input(){
    if(in_progress){
        command_states.clear();

        display_scoreboard=false;
    }
}

void Game::handle_command_states_multiplayer(){
    if(in_progress){
        if(network.status=="server"){
            network.prepare_server_input_states();

            for(int i=0;i<network.clients.size();i++){
                if(!paused){
                    //Example multiplayer command state
                    /**if(network.clients[i].game_command_state("some_command")){
                        ///Deal with command state here
                    }*/
                }
            }
        }
    }
}

void Game::handle_game_commands_multiplayer(){
    if(in_progress){
        if(network.status=="server"){
            for(int i=0;i<network.clients.size();i++){
                for(int j=0;j<network.clients[i].command_buffer.size();j++){
                    string command_name=network.clients[i].command_buffer[j];

                    if(!paused){
                        //Example multiplayer command
                        /**if(command_name=="some_command"){
                            ///Deal with command here
                        }*/
                    }
                }

                network.clients[i].command_buffer.clear();
            }
        }
    }
}

void Game::handle_input_states_gui(){
    int mouse_x=0;
    int mouse_y=0;
    engine_interface.get_mouse_state(&mouse_x,&mouse_y);

    if(in_progress){
        if(engine_interface.game_command_state("scoreboard")){
            display_scoreboard=true;
        }
    }
}

void Game::handle_input_states(){
    int mouse_x=0;
    int mouse_y=0;
    engine_interface.get_mouse_state(&mouse_x,&mouse_y);

    if(in_progress){
        if(!paused){
            //Example multiplayer command state
            /**if(engine_interface.game_command_state("some_command")){
                command_states.push_back("some_command");
            }*/
        }
    }
}

bool Game::handle_game_command_gui(string command_name){
    //Example multiplayer pause
    /**if(command_name=="pause"){
        if(network.status=="server"){
            toggle_pause();

            network.send_paused();
        }

        return true;
    }*/

    //Toggle chat box
    /**else if(command_name=="chat"){
        engine_interface.chat.toggle_on();

        return true;
    }*/

    //Help
    if(command_name=="help"){
        Window* notice=engine_interface.get_window("notice");
        if(!notice->on){
            string help="Scroll the file list up and down using the mouse scroll wheel, the arrow keys, or a gamepad.\n";
            help+="Click on a file/directory by clicking on it with the left mouse button (or press Enter or A on a gamepad).\n";
            help+="Clicking on a directory navigates to it.\n\n";
            help+="Drag files/directories onto the program window to copy them from your computer to the current directory.\n\n";
            help+="Hold Control (or the Left Shoulder button on a gamepad) when clicking on a file/directory to select it.\n";
            help+="Press A (or the Right Shoulder button on a gamepad) to select all files in the current directory.\n\n";
            help+="Press C (or the X button on a gamepad) to copy all selected files/directories to your computer.\n";
            help+="Files copied to your computer are located in '"+engine_interface.get_home_directory()+"files/'\n";
            help+="WARNING: Copying files in either direction has no prompt and simply overwrites any identically\nnamed files already located in the same place. Use wisely.\n\n";
            help+="Press Delete (or the Y button on a gamepad) to delete all selected files/directories.\n";
            help+="WARNING: Directories are deleted recursively. Use wisely.\n\n";
            help+="Press H (or the Left Stick on a gamepad) to toggle the display of hidden files and directories.\n";
            help+="Press F5 (or the Right Stick on a gamepad) to refresh the file list.\n";
            help+="Press Backspace (or click mouse button 4 or B on a gamepad) to move up a directory.\n";

            engine_interface.make_notice(help);
        }
        else{
            if(!engine_interface.is_window_on_top(notice)){
                engine_interface.bring_window_to_top(notice);
            }
            else{
                notice->toggle_on(true,false);
            }
        }

        return true;
    }

    //Toggle hidden files
    else if(command_name=="toggle_hidden_files"){
        option_hidden_files=!option_hidden_files;

        engine_interface.get_window("browser")->rebuild_scrolling_buttons();

        return true;
    }

    //Refresh
    else if(command_name=="refresh"){
        engine_interface.get_window("browser")->rebuild_scrolling_buttons();

        return true;
    }

    //Up
    else if(command_name=="up"){
        world.change_directory("..");

        return true;
    }

    //Select all
    else if(command_name=="select_all"){
        world.select_all();

        return true;
    }

    //Copy
    else if(command_name=="copy"){
        world.copy_selected();

        return true;
    }

    //Delete
    else if(command_name=="delete"){
        if(world.selected_files.size()>0){
            Window* window=engine_interface.get_window("confirm_delete_selected");

            if(world.selected_files.size()==1){
                window->informations[1].text=world.selected_files[0];
            }
            else{
                window->informations[1].text="";
            }

            window->toggle_on(true,true);
        }

        return true;
    }

    return false;
}

bool Game::handle_game_command(string command_name){
    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    ///DEV COMMANDS
    if(engine_interface.option_dev && keystates[SDL_SCANCODE_F1]){
        //Example dev command
        /**if(command_name=="some_dev_command"){
            ///Dev command here.

            return true;
        }*/
    }
    ///END OF DEV COMMANDS

    if(!paused){
        //Example command
        /**if(command_name=="some_command"){
            ///Command here

            return true;
        }*/

        //Example multiplayer command input
        /**if(command_name=="some_command"){
            network.add_command(command_name);

            return true;
        }*/
    }

    return false;
}

bool Game::handle_input_events_gui(){
    bool event_consumed=false;

    if(in_progress){
        for(int i=0;i<engine_interface.game_commands.size() && !event_consumed;i++){
            if((event.type==SDL_CONTROLLERBUTTONDOWN && engine_interface.game_commands[i].button==event.cbutton.button) ||
               (event.type==SDL_KEYDOWN && event.key.repeat==0 && engine_interface.game_commands[i].key==event.key.keysym.scancode)){
                event_consumed=handle_game_command_gui(engine_interface.game_commands[i].name);
            }
        }

        if(event.type==SDL_MOUSEBUTTONDOWN){
            if(!event_consumed && event.button.button==SDL_BUTTON_X1){
                world.change_directory("..");

                event_consumed=true;
            }
        }
    }

    return event_consumed;
}

bool Game::handle_input_events(){
    bool event_consumed=false;

    if(in_progress){
        for(int i=0;i<engine_interface.game_commands.size() && !event_consumed;i++){
            if((event.type==SDL_CONTROLLERBUTTONDOWN && engine_interface.game_commands[i].button==event.cbutton.button) ||
               (event.type==SDL_KEYDOWN && event.key.repeat==0 && engine_interface.game_commands[i].key==event.key.keysym.scancode)){
                event_consumed=handle_game_command(engine_interface.game_commands[i].name);
            }
        }
    }

    return event_consumed;
}
