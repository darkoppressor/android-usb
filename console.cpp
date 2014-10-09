#include "console.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

Console::Console(){
    chat=false;

    y=0;

    on=false;

    move_speed=0;

    max_command_length=0;

    max_log_recall=0;

    max_command_recall=0;

    current_recalled_command_string=0;

    font="";
    font_color="";

    background_opacity=1.0;

    last_tab_complete_command="";

    line_timeout=0;
}

void Console::setup(bool get_chat){
    chat=get_chat;

    if(!chat){
        setup_commands();
    }

    info_display.font=font;
    info_display.font_color=font_color;
    info_display.background_type="standard";
    info_display.background_opacity=background_opacity;
    info_display.scrolling=true;
    info_display.scroll_width=main_window.SCREEN_WIDTH/engine_interface.get_font(info_display.font)->spacing_x-1;
    if(!chat){
        info_display.scroll_height=engine_interface.console_height;
    }
    else{
        info_display.scroll_height=engine_interface.chat_height;
    }
    info_display.x=-1;
    info_display.start_x=info_display.x;
    info_display.start_y=info_display.y;
    info_display.set_dimensions();
    info_display.center_in_window(main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT);

    if(!chat){
        info_display.y=-info_display.h;
    }
    else{
        info_display.y=main_window.SCREEN_HEIGHT;
    }

    y=info_display.y;

    info_input.text_mutable=true;
    info_input.max_text_length=max_command_length;
    info_input.allowed_input.push_back("backspace");
    info_input.allowed_input.push_back("letters");
    info_input.allowed_input.push_back("numbers");
    info_input.allowed_input.push_back("symbols");
    info_input.allowed_input.push_back("space");
    info_input.font=font;
    info_input.font_color=font_color;
    info_input.background_type="standard";
    info_input.background_opacity=background_opacity;
    info_input.scroll_width=main_window.SCREEN_WIDTH/engine_interface.get_font(info_input.font)->spacing_x-1;
    info_input.scroll_height=1;
    info_input.x=-1;
    info_input.y=y+info_display.h;
    info_input.start_x=info_input.x;
    info_input.start_y=info_input.y;
    info_input.set_dimensions();
    info_input.center_in_window(main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT);

    //If the command strings vector is larger or smaller than it is allowed to be, resize it.
    while(recalled_command_strings.size()<max_command_recall){
        recalled_command_strings.push_back("\x1F");
    }
    while(recalled_command_strings.size()>max_command_recall){
        recalled_command_strings.erase(recalled_command_strings.begin());
    }

    if(recalled_command_strings[recalled_command_strings.size()-1]=="\x1F"){
        recalled_command_strings[recalled_command_strings.size()-1]="";
    }

    reset_current_recalled_command_string();

    if(chat){
        reset_text_timers();
    }
}

void Console::reset_current_recalled_command_string(){
    current_recalled_command_string=recalled_command_strings.size();
}

void Console::reset_text_timers(){
    if(chat){
        text_timers.clear();

        for(int i=0;i<string_stuff.newline_count(info_display.text)+1;i++){
            text_timers.push_back(Timer());
        }
    }
}

void Console::clear_text(){
    if(!chat){
        info_display.text="";
    }
    else{
        reset_text_timers();
    }
}

void Console::toggle_on(){
    on=!on;

    if(!on){
        //If the mutable information is in the console, it is no longer selected.
        if(engine_interface.mutable_info_selected()){
            if(engine_interface.mutable_info_this(&info_input)){
                engine_interface.clear_mutable_info();
            }
        }
    }
    else{
        engine_interface.set_mutable_info(&info_input);
    }
}

void Console::add_text(string text){
    if(chat && game.option_chat_timestamps){
        text=engine_interface.get_timestamp(false)+" "+text;
    }

    //If the text extends beyond the text box, wrap it around.
    for(int i=0,j=0;i<text.size();i++,j++){
        if(text[i]=='\n'){
            j=0;
        }
        else if(j>info_display.scroll_width){
            j=0;

            for(int k=i;k>=0;k--){
                if(text[k]==' '){
                    text.insert(k,"\n");
                    text.erase(text.begin()+k+1);
                    break;
                }
                else if(k==0){
                    text.insert(i,"\n");
                    i--;
                    break;
                }
            }
        }
    }

    info_display.text+="\n"+text;

    while(string_stuff.newline_count(info_display.text)+1>max_log_recall){
        info_display.text=string_stuff.erase_first_line(info_display.text);
    }

    info_display.scroll_offset=-string_stuff.newline_count(info_display.text);
    info_display.scroll_offset+=info_display.scroll_height-1;

    if(chat){
        for(int i=0;i<string_stuff.newline_count(text)+1;i++){
            //Create a timer for this line.
            text_timers.push_back(Timer());

            //Start this line's timer.
            text_timers[text_timers.size()-1].start();
        }

        while(text_timers.size()>max_log_recall){
            //Forget the oldest chat line's timer.
            text_timers.erase(text_timers.begin());
        }
    }
}

void Console::send_chat(){
    if(chat){
        if(info_input.text.length()>0){
            //Forget the oldest command string.
            recalled_command_strings.erase(recalled_command_strings.begin());

            //Remember this string.
            recalled_command_strings.push_back(info_input.text);

            reset_current_recalled_command_string();

            string msg=game.option_name;
            msg+=": ";
            msg+=info_input.text;

            info_input.text="";

            if(network.status=="server"){
                network.send_chat_message(msg,RakNet::UNASSIGNED_RAKNET_GUID,true);
            }
            else if(network.status=="client"){
                network.send_chat_message(msg,network.server_id,false);
            }

            add_text(msg);

            toggle_on();
        }
        else{
            toggle_on();
        }
    }
}

void Console::move(){
    if(!chat){
        if(move_speed>0){
            if(on && y<0){
                y+=(int)ceil((double)move_speed/UPDATE_RATE);

                if(y>0){
                    y=0;
                }

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
            else if(!on && y>-(info_display.h+info_input.h)){
                y-=(int)ceil((double)move_speed/UPDATE_RATE);

                if(y<-(info_display.h+info_input.h)){
                    y=-(info_display.h+info_input.h);
                }

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
        }
        else{
            if(on){
                y=0;

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
            else{
                y=-(info_display.h+info_input.h);

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
        }
    }
    else{
        if(move_speed>0){
            if(on && y>main_window.SCREEN_HEIGHT-info_display.h-info_input.h){
                y-=(int)ceil((double)move_speed/UPDATE_RATE);

                if(y<main_window.SCREEN_HEIGHT-info_display.h-info_input.h){
                    y=main_window.SCREEN_HEIGHT-info_display.h-info_input.h;
                }

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
            else if(!on && y<main_window.SCREEN_HEIGHT){
                y+=(int)ceil((double)move_speed/UPDATE_RATE);

                if(y>main_window.SCREEN_HEIGHT){
                    y=main_window.SCREEN_HEIGHT;
                }

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
        }
        else{
            if(on){
                y=main_window.SCREEN_HEIGHT-info_display.h-info_input.h;

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
            else{
                y=main_window.SCREEN_HEIGHT;

                info_display.y=y;
                info_input.y=y+info_display.h;
            }
        }
    }
}

void Console::recall_up(){
    if(current_recalled_command_string>0 && recalled_command_strings[current_recalled_command_string-1]!="\x1F"){
        current_recalled_command_string--;
    }

    info_input.text=recalled_command_strings[current_recalled_command_string];
}

void Console::recall_down(){
    if(current_recalled_command_string<recalled_command_strings.size()-1){
        current_recalled_command_string++;

        info_input.text=recalled_command_strings[current_recalled_command_string];
    }
    else{
        reset_current_recalled_command_string();

        info_input.text.clear();
    }
}

void Console::tab_complete(){
    if(!chat){
        if(info_input.text.length()>0){
            vector<string> valid_commands;

            for(int i=0;i<commands.size();i++){
                if(boost::algorithm::starts_with(commands[i],info_input.text)){
                    valid_commands.push_back(commands[i]);
                }
            }

            if(valid_commands.size()>0){
                if(valid_commands.size()==1){
                    info_input.text=valid_commands[0];
                }
                else{
                    string shared_characters=info_input.text;
                    bool mismatch_found=false;
                    while(!mismatch_found){
                        if(valid_commands[0].length()>=shared_characters.length()+1){
                            shared_characters+=valid_commands[0][shared_characters.length()];
                        }
                        else{
                            break;
                        }

                        for(int i=0;i<valid_commands.size();i++){
                            if(valid_commands[i].length()<shared_characters.length() || !boost::algorithm::starts_with(valid_commands[i],shared_characters)){
                                shared_characters.erase(shared_characters.end()-1);

                                mismatch_found=true;
                                break;
                            }
                        }
                    }

                    if(info_input.text.length()==shared_characters.length() && last_tab_complete_command.length()==0){
                        last_tab_complete_command=shared_characters;
                    }
                    else if(info_input.text.length()==shared_characters.length() && last_tab_complete_command.length()>0){
                        int max_valid_length=0;
                        for(int i=0;i<valid_commands.size();i++){
                            if(valid_commands[i].length()>max_valid_length){
                                max_valid_length=valid_commands[i].length();
                            }
                        }
                        max_valid_length+=2;

                        vector<string> valid_command_display;
                        valid_command_display.push_back("");
                        for(int i=0;i<valid_commands.size();i++){
                            string this_line=valid_commands[i];
                            for(int n=0;n<max_valid_length-valid_commands[i].length();n++){
                                this_line+=" ";
                            }

                            if(valid_command_display[valid_command_display.size()-1].length()+this_line.length()>info_display.w/engine_interface.get_font(info_display.font)->spacing_x){
                                valid_command_display.push_back("");
                            }

                            valid_command_display[valid_command_display.size()-1]+=this_line;
                        }

                        add_text("");
                        for(int i=0;i<valid_command_display.size();i++){
                            add_text(valid_command_display[i]);
                        }
                        add_text("");
                    }
                    else{
                        info_input.text=shared_characters;
                    }
                }
            }
        }
        else{
            info_input.text=commands[0];
        }
    }
}

void Console::handle_input_states(){
    if(on){
        int mouse_x=0;
        int mouse_y=0;
        engine_interface.get_mouse_state(&mouse_x,&mouse_y);

        info_display.handle_input_states(mouse_x,mouse_y,0,0);
        info_input.handle_input_states(mouse_x,mouse_y,0,0);
    }
}

bool Console::handle_input_events(){
    bool event_consumed=false;

    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    if(!chat){
        if(last_tab_complete_command.length()>0 && last_tab_complete_command!=info_input.text){
            last_tab_complete_command="";
        }

        switch(event.type){
            case SDL_CONTROLLERBUTTONDOWN:
                if(on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                    recall_up();
                }
                else if(on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                    recall_down();
                }
                else if(on && event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                    tab_complete();
                }
                break;

            case SDL_KEYDOWN:
                if(event.key.repeat==0){
                    if((event.key.keysym.scancode==SDL_SCANCODE_GRAVE && !keystates[SDL_SCANCODE_LSHIFT] && !keystates[SDL_SCANCODE_RSHIFT]) || event.key.keysym.scancode==SDL_SCANCODE_AC_SEARCH){
                        if(engine_interface.gui_mode=="controller"){
                            engine_interface.set_gui_mode("mouse");
                        }

                        toggle_on();

                        event_consumed=true;
                    }
                    else if(on && event.key.keysym.scancode==SDL_SCANCODE_UP){
                        recall_up();
                    }
                    else if(on && event.key.keysym.scancode==SDL_SCANCODE_DOWN){
                        recall_down();
                    }
                    else if(on && event.key.keysym.scancode==SDL_SCANCODE_TAB){
                        tab_complete();
                    }
                }
                break;
        }
    }
    else{
        switch(event.type){
            case SDL_CONTROLLERBUTTONDOWN:
                if(!event_consumed && on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                    recall_up();

                    event_consumed=true;
                }
                if(!event_consumed && on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                    recall_down();

                    event_consumed=true;
                }
                break;

            case SDL_KEYDOWN:
                if(event.key.repeat==0){
                    if(!event_consumed && on && event.key.keysym.scancode==SDL_SCANCODE_UP){
                        recall_up();

                        event_consumed=true;
                    }
                    if(!event_consumed && on && event.key.keysym.scancode==SDL_SCANCODE_DOWN){
                        recall_down();

                        event_consumed=true;
                    }
                }
                break;
        }
    }

    if(on && engine_interface.mouse_allowed()){
        int mouse_x=0;
        int mouse_y=0;
        engine_interface.get_mouse_state(&mouse_x,&mouse_y);

        if(!event_consumed){
            event_consumed=info_display.handle_input_events(mouse_x,mouse_y,0,0);
        }
        if(!event_consumed){
            event_consumed=info_input.handle_input_events(mouse_x,mouse_y,0,0);
        }
    }

    return event_consumed;
}

void Console::animate(){
    move();

    if(on){
        info_display.animate();
        info_input.animate();
    }
}

void Console::render(){
    if(!chat){
        if(y+info_display.h+info_input.h>0){
            info_display.render(0,0);
            info_input.render(0,0);
        }
    }
    else{
        if(game.in_progress){
            if(y<main_window.SCREEN_HEIGHT){
                info_display.render(0,0);
                info_input.render(0,0);
            }
            else{
                Information info_temp_display=info_display;
                info_temp_display.y=main_window.SCREEN_HEIGHT-info_display.h-info_input.h;
                info_temp_display.background_type="none";

                string newline="\xA";
                int line=0;

                for(int i=0;i<info_temp_display.text.length();i++){
                    if(info_temp_display.text[i]==newline[0]){
                        line++;
                    }
                    else{
                        if(!text_timers[line].is_started() || (text_timers[line].is_started() && text_timers[line].get_ticks()>=line_timeout)){
                            info_temp_display.text.erase(info_temp_display.text.begin()+i);
                            i--;
                        }
                    }
                }

                info_temp_display.render(0,0);
            }
        }
    }
}
