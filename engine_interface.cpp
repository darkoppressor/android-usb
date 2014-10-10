#include "engine_interface.h"
#include "world.h"
#include "render.h"
#include "sorting.h"

#include <fstream>
#include <cmath>

#include <SDL_image.h>

#include <boost/algorithm/string.hpp>

using namespace std;

GUI_Object::GUI_Object(string get_type,int get_index,int get_x,int get_y){
    type=get_type;
    index=get_index;
    x=get_x;
    y=get_y;
}

GUI_Selector_Chaser::GUI_Selector_Chaser(){
    x=-1.0;
    y=-1.0;
}

Controller::Controller(SDL_GameController* get_controller){
    controller=get_controller;
    instance_id=-1;
    haptic=0;
}

Engine_Interface::Engine_Interface(){
    window_under_mouse=0;

    controller_text_entry_small=false;

    cursor_render_always=false;

    cursor="";
    cursor_mouse_over="";
    color_theme="";
    toast_font="";
    default_font="";

    toast_length_short=0;
    toast_length_medium=0;
    toast_length_long=0;

    logical_screen_width=0;
    logical_screen_height=0;

    resolution_mode="";

    axis_scroll_rate=0;

    scrolling_buttons_offset=0;

    cursor_width=0;
    cursor_height=0;

    console_height=0;
    chat_height=0;

    sound_falloff=0.0;

    window_border_thickness=0.0;
    gui_border_thickness=0.0;

    touch_finger_size=0.0;
    touch_controller_shoulders=false;
    touch_controller_guide=false;
    touch_controller_xy=false;

    game_title="";
    home_directory="";
    developer="";

    option_save_location="";

    option_version=get_version();

    option_screen_width=0;
    option_screen_height=0;
    option_fullscreen=false;
    option_fullscreen_mode="windowed";

    option_vsync=false;
    option_accelerometer_controller=false;
    option_touch_controller=false;
    option_touch_controller_opacity=0.0;
    option_font_shadows=false;
    option_screen_keyboard=false;
	option_hw_cursor=false;
	option_bind_cursor=false;

    option_fps=false;
    option_dev=false;
    option_volume_global=1.0;
    option_volume_sound=1.0;
    option_volume_music=1.0;
    option_mute_global=false;
    option_mute_sound=false;
    option_mute_music=false;

    hide_gui=false;

    need_to_reinit=false;

    mouse_over=false;

    touch_controls=false;

    gui_mode="mouse";
    gui_selected_object=-1;
    gui_selector_style="";
    for(int i=0;i<2;i++){
        gui_selector_chasers.push_back(GUI_Selector_Chaser());
    }

    gui_axis_nav_last_direction="none";

    controller_dead_zone=0;

    counter_gui_scroll_axis=0;

    configure_command=-1;

    editing_server=0;

    counter_cursor=0;
    cursor_fade_direction=false;
    cursor_opacity=10;

    ptr_mutable_info=0;
}

void Engine_Interface::reload(){
    need_to_reinit=true;
}

void Engine_Interface::quit(){
    game.stop();

    unload_world();

    main_window.cleanup_video();

    for(int i=0;i<controllers.size();i++){
        if(controllers[i].haptic!=0 && SDL_HapticOpened(SDL_HapticIndex(controllers[i].haptic))){
            SDL_HapticClose(controllers[i].haptic);
        }

        SDL_GameControllerClose(controllers[i].controller);
    }
    controllers.clear();

    IMG_Quit();

    Mix_CloseAudio();
    Mix_Quit();

    SDL_Quit();

    exit(EXIT_SUCCESS);
}

void Engine_Interface::build_text_input_characters(){
    characters_symbols.clear();

    characters_lower.clear();
    for(char i='a';i<='z';i++){
        characters_lower.push_back(string(1,i));
    }

    if(!controller_text_entry_small){
        characters_lower.push_back(",");
        characters_lower.push_back(".");
        characters_lower.push_back(":");
        characters_lower.push_back("/");
        characters_lower.push_back("@");
        characters_lower.push_back("-");
    }
    else{
        characters_symbols.push_back(",");
        characters_symbols.push_back(".");
        characters_symbols.push_back(":");
        characters_symbols.push_back("/");
        characters_symbols.push_back("@");
        characters_symbols.push_back("-");
    }

    characters_upper.clear();
    for(char i='A';i<='Z';i++){
        characters_upper.push_back(string(1,i));
    }

    if(!controller_text_entry_small){
        characters_upper.push_back("?");
        characters_upper.push_back("!");
        characters_upper.push_back(";");
        characters_upper.push_back("\\");
        characters_upper.push_back("&");
        characters_upper.push_back("_");
    }
    else{
        characters_symbols.push_back("?");
        characters_symbols.push_back("!");
        characters_symbols.push_back(";");
        characters_symbols.push_back("\\");
        characters_symbols.push_back("&");
        characters_symbols.push_back("_");
    }

    characters_numbers.clear();
    characters_numbers.push_back("1");
    characters_numbers.push_back("2");
    characters_numbers.push_back("3");
    characters_numbers.push_back("4");
    characters_numbers.push_back("5");
    characters_numbers.push_back("6");
    characters_numbers.push_back("7");
    characters_numbers.push_back("8");
    characters_numbers.push_back("9");
    characters_numbers.push_back("0");

    if(!controller_text_entry_small){
        characters_numbers.push_back("*");
        characters_numbers.push_back("+");
        characters_numbers.push_back(string(1,(unsigned char)156));
        characters_numbers.push_back(string(1,(unsigned char)155));
        characters_numbers.push_back("$");
        characters_numbers.push_back("`");
        characters_numbers.push_back("'");
        characters_numbers.push_back("\"");
        characters_numbers.push_back("~");
        characters_numbers.push_back("|");
        characters_numbers.push_back("=");
        characters_numbers.push_back("#");
        characters_numbers.push_back("%");
        characters_numbers.push_back("^");
        characters_numbers.push_back("<");
        characters_numbers.push_back(">");
        characters_numbers.push_back("[");
        characters_numbers.push_back("]");
        characters_numbers.push_back("{");
        characters_numbers.push_back("}");
        characters_numbers.push_back("(");
        characters_numbers.push_back(")");
    }
    else{
        characters_numbers.push_back(" ");
        characters_numbers.push_back(" ");
        characters_numbers.push_back(" ");

        characters_symbols.push_back("*");
        characters_symbols.push_back("+");
        characters_symbols.push_back(string(1,(unsigned char)156));
        characters_symbols.push_back(string(1,(unsigned char)155));
        characters_symbols.push_back("$");
        characters_symbols.push_back("`");
        characters_symbols.push_back("'");
        characters_symbols.push_back("\"");
        characters_symbols.push_back("~");
        characters_symbols.push_back("|");
        characters_symbols.push_back("=");
        characters_symbols.push_back("#");
        characters_symbols.push_back("%");
        characters_symbols.push_back("^");
        characters_symbols.push_back("<");
        characters_symbols.push_back(">");
        characters_symbols.push_back("[");
        characters_symbols.push_back("]");
        characters_symbols.push_back("{");
        characters_symbols.push_back("}");
        characters_symbols.push_back("(");
        characters_symbols.push_back(")");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(string(1,(unsigned char)1));
        characters_symbols.push_back(string(1,(unsigned char)2));
    }
}

void Engine_Interface::set_logic_update_rate(double frame_rate){
    UPDATE_RATE=frame_rate;
    SKIP_TICKS=1000.0/UPDATE_RATE;
}

void Engine_Interface::set_render_update_rate(double frame_rate){
    UPDATE_RATE_RENDER=frame_rate;
    SKIP_TICKS_RENDER=1000.0/UPDATE_RATE_RENDER;
}

bool Engine_Interface::load_data_engine(){
    bool load_result=load_data("engine");

    build_text_input_characters();

    return load_result;
}

void Engine_Interface::load_data_main(){
    load_data("font");
    load_data("cursor");
    load_data("color");
    load_data("color_theme");
    load_data("animation");
    load_data("window");
    load_data("game_command");
    load_data("game_constant");

    load_data_game();

    for(int i=0;i<windows.size();i++){
        windows[i].create_close_button();
        windows[i].set_last_normal_button();
    }

    for(int i=0;i<cursors.size();i++){
        cursors[i].load_hw_cursor();
    }

    text_selector.set_name("text_selector");
}

void Engine_Interface::load_data_game_options(){
    load_data("game_option");
}

bool Engine_Interface::load_data(string script){
    //Look through all of the files in the directory.
    for(File_IO_Directory_Iterator it("data");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_path=it.get_full_path();

            File_IO_Load load(file_path);

            if(load.file_loaded()){
                bool multi_line_comment=false;

                //As long as we haven't reached the end of the file.
                while(!load.eof()){
                    string line="";

                    //Grab the next line of the file.
                    load.getline(&line);

                    //Clear initial whitespace from the line.
                    boost::algorithm::trim(line);

                    //If the line ends a multi-line comment.
                    if(boost::algorithm::contains(line,"*/")){
                        multi_line_comment=false;
                    }
                    //If the line starts a multi-line comment.
                    if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
                        multi_line_comment=true;
                    }
                    //If the line is a comment.
                    else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
                        //Ignore this line.
                    }

                    //If the line begins an instance of the passed script type.
                    else if(!multi_line_comment && boost::algorithm::starts_with(line,"<"+script+">")){
                        if(script=="engine"){
                            load_engine_data(&load);
                        }
                        else if(script=="font"){
                            load_font(&load);
                        }
                        else if(script=="cursor"){
                            load_cursor(&load);
                        }
                        else if(script=="color"){
                            load_color(&load);
                        }
                        else if(script=="color_theme"){
                            load_color_theme(&load);
                        }
                        else if(script=="animation"){
                            load_animation(&load);
                        }
                        else if(script=="window"){
                            load_window(&load);
                        }
                        else if(script=="game_command"){
                            load_game_command(&load);
                        }
                        else if(script=="game_option"){
                            load_game_option(&load);
                        }
                        else if(script=="game_constant"){
                            load_game_constant(&load);
                        }
                        else{
                            load_data_script_game(script,&load);
                        }
                    }
                }
            }
            else{
                message_log.add_error("Error loading script data: '"+script+"'");

                return false;
            }
        }
    }

    return true;
}

void Engine_Interface::unload_data(){
    for(int i=0;i<cursors.size();i++){
        cursors[i].free_hw_cursor();
    }

    clear_mutable_info();

    fonts.clear();
    cursors.clear();
    colors.clear();
    color_themes.clear();
    animations.clear();
    windows.clear();

    window_z_order.clear();
    window_under_mouse=0;

    game_commands.clear();
    game_options.clear();

    unload_data_game();
}

void Engine_Interface::load_engine_data(File_IO_Load* load){
    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_home_directory="home_directory:";
        string str_developer="developer:";
        string str_starting_windows="starting_windows:";
        string str_logical_screen_width="logical_screen_width:";
        string str_logical_screen_height="logical_screen_height:";
        string str_resolution_mode="resolution_mode:";
        string str_frame_rate="logic_update_rate:";
        string str_frame_rate_render="frame_rate_max:";
        string str_axis_scroll_rate="axis_scroll_rate:";
        string str_scrolling_buttons_offset="scrolling_buttons_offset:";
        string str_cursor_width="cursor_width:";
        string str_cursor_height="cursor_height:";
        string str_console_height="console_height:";
        string str_chat_height="chat_height:";
        string str_sound_falloff="sound_falloff:";
        string str_window_border_thickness="window_border_thickness:";
        string str_gui_border_thickness="gui_border_thickness:";
        string str_touch_finger_size="touch_finger_size:";
        string str_touch_controller_shoulders="touch_controller_shoulders:";
        string str_touch_controller_guide="touch_controller_guide:";
        string str_touch_controller_xy="touch_controller_xy:";
        string str_tooltip_font="tooltip_font:";
        string str_toast_font="toast_font:";
        string str_default_font="default_font:";
        string str_toast_length_short="toast_length_short:";
        string str_toast_length_medium="toast_length_medium:";
        string str_toast_length_long="toast_length_long:";
        string str_console_move_speed="console_move_speed:";
        string str_console_max_command_length="console_max_command_length:";
        string str_console_max_log_recall="console_max_log_recall:";
        string str_console_max_command_recall="console_max_command_recall:";
        string str_console_font="console_font:";
        string str_console_font_color="console_font_color:";
        string str_console_opacity="console_opacity:";
        string str_chat_move_speed="chat_move_speed:";
        string str_chat_max_command_length="chat_max_command_length:";
        string str_chat_max_log_recall="chat_max_log_recall:";
        string str_chat_max_command_recall="chat_max_command_recall:";
        string str_chat_font="chat_font:";
        string str_chat_font_color="chat_font_color:";
        string str_chat_opacity="chat_opacity:";
        string str_chat_line_timeout="chat_line_timeout:";
        string str_gui_selector_style="gui_selector_style:";
        string str_controller_dead_zone="controller_dead_zone:";
        string str_color_theme="color_theme:";
        string str_controller_text_entry_small="controller_text_entry_small:";
        string str_cursor_render_always="cursor_render_always:";
        string str_cursor="cursor:";
        string str_cursor_mouse_over="cursor_mouse_over:";
        string str_default_save_location="default_save_location:";

        string str_default_screen_width="default_screen_width:";
        string str_default_screen_height="default_screen_height:";
        string str_default_fullscreen="default_fullscreen_state:";
        string str_default_fullscreen_mode="default_fullscreen_mode:";

        string str_default_vsync="default_vsync:";
        string str_default_accelerometer_controller="default_accelerometer_controller:";
        string str_default_touch_controller="default_touch_controller:";
        string str_default_touch_controller_opacity="default_touch_controller_opacity:";
        string str_default_font_shadows="default_font_shadows:";
        string str_default_screen_keyboard="default_screen_keyboard:";
        string str_default_hw_cursor="default_hw_cursor:";
        string str_default_bind_cursor="default_bind_cursor:";

        string str_default_fps="default_fps:";
        string str_default_dev="default_dev:";
        string str_default_volume_global="default_volume_global:";
        string str_default_volume_sound="default_volume_sound:";
        string str_default_volume_music="default_volume_music:";
        string str_default_mute_global="default_mute_global:";
        string str_default_mute_sound="default_mute_sound:";
        string str_default_mute_music="default_mute_music:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            game_title=line;
        }
        //Home directory
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_home_directory)){
            //Clear the data name.
            line.erase(0,str_home_directory.length());

            home_directory=line;
        }
        //Developer
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_developer)){
            //Clear the data name.
            line.erase(0,str_developer.length());

            developer=line;
        }
        //Starting windows
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_windows)){
            //Clear the data name.
            line.erase(0,str_starting_windows.length());

            starting_windows.clear();
            boost::algorithm::split(starting_windows,line,boost::algorithm::is_any_of(","));

            for(int i=0;i<starting_windows.size();i++){
                if(starting_windows[i].length()==0){
                    starting_windows.erase(starting_windows.begin()+i);
                    i--;
                }
            }
        }
        //Logical screen width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_logical_screen_width)){
            //Clear the data name.
            line.erase(0,str_logical_screen_width.length());

            logical_screen_width=string_stuff.string_to_long(line);
        }
        //Logical screen height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_logical_screen_height)){
            //Clear the data name.
            line.erase(0,str_logical_screen_height.length());

            logical_screen_height=string_stuff.string_to_long(line);
        }
        //Resolution mode
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_resolution_mode)){
            //Clear the data name.
            line.erase(0,str_resolution_mode.length());

            resolution_mode=line;
        }
        //Frame rate
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_rate)){
            //Clear the data name.
            line.erase(0,str_frame_rate.length());

            set_logic_update_rate(string_stuff.string_to_double(line));
        }
        //Frame rate render
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_rate_render)){
            //Clear the data name.
            line.erase(0,str_frame_rate_render.length());

            set_render_update_rate(string_stuff.string_to_double(line));
        }
        //Axis scroll rate
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_axis_scroll_rate)){
            //Clear the data name.
            line.erase(0,str_axis_scroll_rate.length());

            axis_scroll_rate=string_stuff.string_to_long(line);
        }
        //Scrolling buttons offset
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling_buttons_offset)){
            //Clear the data name.
            line.erase(0,str_scrolling_buttons_offset.length());

            scrolling_buttons_offset=string_stuff.string_to_long(line);
        }
        //Cursor width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_width)){
            //Clear the data name.
            line.erase(0,str_cursor_width.length());

            cursor_width=string_stuff.string_to_long(line);
        }
        //Cursor height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_height)){
            //Clear the data name.
            line.erase(0,str_cursor_height.length());

            cursor_height=string_stuff.string_to_long(line);
        }
        //Console height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_height)){
            //Clear the data name.
            line.erase(0,str_console_height.length());

            console_height=string_stuff.string_to_long(line);
        }
        //Chat height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_height)){
            //Clear the data name.
            line.erase(0,str_chat_height.length());

            chat_height=string_stuff.string_to_long(line);
        }
        //Sound falloff
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sound_falloff)){
            //Clear the data name.
            line.erase(0,str_sound_falloff.length());

            sound_falloff=string_stuff.string_to_double(line);
        }
        //Window border thickness
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_border_thickness)){
            //Clear the data name.
            line.erase(0,str_window_border_thickness.length());

            window_border_thickness=string_stuff.string_to_double(line);
        }
        //GUI border thickness
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_border_thickness)){
            //Clear the data name.
            line.erase(0,str_gui_border_thickness.length());

            gui_border_thickness=string_stuff.string_to_double(line);
        }
        //Touch finger size
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_finger_size)){
            //Clear the data name.
            line.erase(0,str_touch_finger_size.length());

            touch_finger_size=string_stuff.string_to_double(line);
        }
        //Touch controller shoulders
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_shoulders)){
            //Clear the data name.
            line.erase(0,str_touch_controller_shoulders.length());

            touch_controller_shoulders=string_stuff.string_to_bool(line);
        }
        //Touch controller guide
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_guide)){
            //Clear the data name.
            line.erase(0,str_touch_controller_guide.length());

            touch_controller_guide=string_stuff.string_to_bool(line);
        }
        //Touch controller xy
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_xy)){
            //Clear the data name.
            line.erase(0,str_touch_controller_xy.length());

            touch_controller_xy=string_stuff.string_to_bool(line);
        }
        //Tooltip font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_font)){
            //Clear the data name.
            line.erase(0,str_tooltip_font.length());

            tooltip.font=line;
        }
        //Toast font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_font)){
            //Clear the data name.
            line.erase(0,str_toast_font.length());

            toast_font=line;
        }
        //Default font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_font)){
            //Clear the data name.
            line.erase(0,str_default_font.length());

            default_font=line;
        }
        //Toast length short
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_short)){
            //Clear the data name.
            line.erase(0,str_toast_length_short.length());

            toast_length_short=string_stuff.string_to_long(line);
        }
        //Toast length medium
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_medium)){
            //Clear the data name.
            line.erase(0,str_toast_length_medium.length());

            toast_length_medium=string_stuff.string_to_long(line);
        }
        //Toast length long
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_long)){
            //Clear the data name.
            line.erase(0,str_toast_length_long.length());

            toast_length_long=string_stuff.string_to_long(line);
        }
        //Console move speed
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_move_speed)){
            //Clear the data name.
            line.erase(0,str_console_move_speed.length());

            console.move_speed=string_stuff.string_to_long(line);
        }
        //Console max command length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_command_length)){
            //Clear the data name.
            line.erase(0,str_console_max_command_length.length());

            console.max_command_length=string_stuff.string_to_long(line);
        }
        //Console max log recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_log_recall)){
            //Clear the data name.
            line.erase(0,str_console_max_log_recall.length());

            console.max_log_recall=string_stuff.string_to_long(line);
        }
        //Console max command recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_command_recall)){
            //Clear the data name.
            line.erase(0,str_console_max_command_recall.length());

            console.max_command_recall=string_stuff.string_to_long(line);
        }
        //Console font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_font)){
            //Clear the data name.
            line.erase(0,str_console_font.length());

            console.font=line;
        }
        //Console font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_font_color)){
            //Clear the data name.
            line.erase(0,str_console_font_color.length());

            console.font_color=line;
        }
        //Console opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_opacity)){
            //Clear the data name.
            line.erase(0,str_console_opacity.length());

            console.background_opacity=string_stuff.string_to_double(line);
        }
        //Chat move speed
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_move_speed)){
            //Clear the data name.
            line.erase(0,str_chat_move_speed.length());

            chat.move_speed=string_stuff.string_to_long(line);
        }
        //Chat max command length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_command_length)){
            //Clear the data name.
            line.erase(0,str_chat_max_command_length.length());

            chat.max_command_length=string_stuff.string_to_long(line);
        }
        //Chat max log recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_log_recall)){
            //Clear the data name.
            line.erase(0,str_chat_max_log_recall.length());

            chat.max_log_recall=string_stuff.string_to_long(line);
        }
        //Chat max command recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_command_recall)){
            //Clear the data name.
            line.erase(0,str_chat_max_command_recall.length());

            chat.max_command_recall=string_stuff.string_to_long(line);
        }
        //Chat font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_font)){
            //Clear the data name.
            line.erase(0,str_chat_font.length());

            chat.font=line;
        }
        //Chat font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_font_color)){
            //Clear the data name.
            line.erase(0,str_chat_font_color.length());

            chat.font_color=line;
        }
        //Chat opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_opacity)){
            //Clear the data name.
            line.erase(0,str_chat_opacity.length());

            chat.background_opacity=string_stuff.string_to_double(line);
        }
        //Chat line timeout
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_line_timeout)){
            //Clear the data name.
            line.erase(0,str_chat_line_timeout.length());

            chat.line_timeout=string_stuff.string_to_long(line);
        }
        //GUI selector style
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_selector_style)){
            //Clear the data name.
            line.erase(0,str_gui_selector_style.length());

            gui_selector_style=line;
        }
        //Controller dead zone
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_dead_zone)){
            //Clear the data name.
            line.erase(0,str_controller_dead_zone.length());

            controller_dead_zone=string_stuff.string_to_long(line);
        }
        //Color theme
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_color_theme)){
            //Clear the data name.
            line.erase(0,str_color_theme.length());

            color_theme=line;
        }
        //Controller text entry small
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_text_entry_small)){
            //Clear the data name.
            line.erase(0,str_controller_text_entry_small.length());

            controller_text_entry_small=string_stuff.string_to_bool(line);
        }
        //Cursor render always
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_render_always)){
            //Clear the data name.
            line.erase(0,str_cursor_render_always.length());

            cursor_render_always=string_stuff.string_to_bool(line);
        }
        //Cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor)){
            //Clear the data name.
            line.erase(0,str_cursor.length());

            cursor=line;
        }
        //Cursor Mouse Over
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_mouse_over)){
            //Clear the data name.
            line.erase(0,str_cursor_mouse_over.length());

            cursor_mouse_over=line;
        }
        //default_save_location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_save_location)){
            //Clear the data name.
            line.erase(0,str_default_save_location.length());

            option_save_location=line;
        }

        //default_screen_width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_width)){
            //Clear the data name.
            line.erase(0,str_default_screen_width.length());

            option_screen_width=string_stuff.string_to_long(line);
        }
        //default_screen_height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_height)){
            //Clear the data name.
            line.erase(0,str_default_screen_height.length());

            option_screen_height=string_stuff.string_to_long(line);
        }
        //default_fullscreen
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fullscreen)){
            //Clear the data name.
            line.erase(0,str_default_fullscreen.length());

            option_fullscreen=string_stuff.string_to_bool(line);
        }
        //default_fullscreen_mode
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fullscreen_mode)){
            //Clear the data name.
            line.erase(0,str_default_fullscreen_mode.length());

            option_fullscreen_mode=line;
        }

        //default_vsync
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_vsync)){
            //Clear the data name.
            line.erase(0,str_default_vsync.length());

            option_vsync=string_stuff.string_to_bool(line);
        }
        //default_accelerometer_controller
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_accelerometer_controller)){
            //Clear the data name.
            line.erase(0,str_default_accelerometer_controller.length());

            option_accelerometer_controller=string_stuff.string_to_bool(line);
        }
        //default_touch_controller
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_touch_controller)){
            //Clear the data name.
            line.erase(0,str_default_touch_controller.length());

            option_touch_controller=string_stuff.string_to_bool(line);
        }
        //default_touch_controller_opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_touch_controller_opacity)){
            //Clear the data name.
            line.erase(0,str_default_touch_controller_opacity.length());

            option_touch_controller_opacity=string_stuff.string_to_double(line);
        }
        //default_font_shadows
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_font_shadows)){
            //Clear the data name.
            line.erase(0,str_default_font_shadows.length());

            option_font_shadows=string_stuff.string_to_bool(line);
        }
        //default_screen_keyboard
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_keyboard)){
            //Clear the data name.
            line.erase(0,str_default_screen_keyboard.length());

            option_screen_keyboard=string_stuff.string_to_bool(line);
        }
        //default_hw_cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_hw_cursor)){
            //Clear the data name.
            line.erase(0,str_default_hw_cursor.length());

            option_hw_cursor=string_stuff.string_to_bool(line);
        }
        //default_bind_cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_bind_cursor)){
            //Clear the data name.
            line.erase(0,str_default_bind_cursor.length());

            option_bind_cursor=string_stuff.string_to_bool(line);
        }

        //default_fps
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fps)){
            //Clear the data name.
            line.erase(0,str_default_fps.length());

            option_fps=string_stuff.string_to_bool(line);
        }
        //default_dev
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_dev)){
            //Clear the data name.
            line.erase(0,str_default_dev.length());

            option_dev=string_stuff.string_to_bool(line);
        }
        //default_volume_global
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_global)){
            //Clear the data name.
            line.erase(0,str_default_volume_global.length());

            option_volume_global=string_stuff.string_to_double(line);
        }
        //default_volume_sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_sound)){
            //Clear the data name.
            line.erase(0,str_default_volume_sound.length());

            option_volume_sound=string_stuff.string_to_double(line);
        }
        //default_volume_music
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_music)){
            //Clear the data name.
            line.erase(0,str_default_volume_music.length());

            option_volume_music=string_stuff.string_to_double(line);
        }
        //default_mute_global
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_global)){
            //Clear the data name.
            line.erase(0,str_default_mute_global.length());

            option_mute_global=string_stuff.string_to_bool(line);
        }
        //default_mute_sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_sound)){
            //Clear the data name.
            line.erase(0,str_default_mute_sound.length());

            option_mute_sound=string_stuff.string_to_bool(line);
        }
        //default_mute_music
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_music)){
            //Clear the data name.
            line.erase(0,str_default_mute_music.length());

            option_mute_music=string_stuff.string_to_bool(line);
        }

        //If the line ends the engine data.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</engine>")){
            return;
        }
    }
}

void Engine_Interface::load_font(File_IO_Load* load){
    fonts.push_back(Bitmap_Font());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_sprite="sprite:";
        string str_spacing_x="spacing_x:";
        string str_spacing_y="spacing_y:";
        string str_gui_padding_x="gui_padding_x:";
        string str_gui_padding_y="gui_padding_y:";
        string str_shadow_distance="shadow_distance:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            fonts[fonts.size()-1].name=line;
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            fonts[fonts.size()-1].sprite.name=line;
        }
        //Spacing x
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_spacing_x)){
            //Clear the data name.
            line.erase(0,str_spacing_x.length());

            fonts[fonts.size()-1].spacing_x=string_stuff.string_to_long(line);
        }
        //Spacing y
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_spacing_y)){
            //Clear the data name.
            line.erase(0,str_spacing_y.length());

            fonts[fonts.size()-1].spacing_y=string_stuff.string_to_long(line);
        }
        //GUI padding x
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_padding_x)){
            //Clear the data name.
            line.erase(0,str_gui_padding_x.length());

            fonts[fonts.size()-1].gui_padding_x=string_stuff.string_to_long(line);
        }
        //GUI padding y
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_padding_y)){
            //Clear the data name.
            line.erase(0,str_gui_padding_y.length());

            fonts[fonts.size()-1].gui_padding_y=string_stuff.string_to_long(line);
        }
        //Shadow distance
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_shadow_distance)){
            //Clear the data name.
            line.erase(0,str_shadow_distance.length());

            fonts[fonts.size()-1].shadow_distance=string_stuff.string_to_long(line);
        }

        //If the line ends the font.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</font>")){
            fonts[fonts.size()-1].build_font();

            return;
        }
    }
}

void Engine_Interface::load_cursor(File_IO_Load* load){
    cursors.push_back(Cursor());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_sprite="sprite:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            cursors[cursors.size()-1].name=line;
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            cursors[cursors.size()-1].sprite.name=line;
        }

        //If the line ends the cursor.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</cursor>")){
            return;
        }
    }
}

void Engine_Interface::load_color(File_IO_Load* load){
    colors.push_back(Color());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_rgb="rgb:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            colors[colors.size()-1].name=line;
        }
        //Rgb
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_rgb)){
            //Clear the data name.
            line.erase(0,str_rgb.length());

            vector<string> rgb_values;
            boost::algorithm::split(rgb_values,line,boost::algorithm::is_any_of(","));

            if(rgb_values.size()==3){
                colors[colors.size()-1].set_rgb(string_stuff.string_to_long(rgb_values[0]),string_stuff.string_to_long(rgb_values[1]),string_stuff.string_to_long(rgb_values[2]));
            }
        }

        //If the line ends the color.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</color>")){
            return;
        }
    }
}

void Engine_Interface::load_color_theme(File_IO_Load* load){
    color_themes.push_back(Color_Theme());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_window_font="window_font:";
        string str_window_title_bar="window_title_bar:";
        string str_window_background="window_background:";
        string str_window_border="window_border:";
        string str_button_font="button_font:";
        string str_button_background="button_background:";
        string str_button_background_moused="button_background_moused:";
        string str_button_background_click="button_background_click:";
        string str_button_border="button_border:";
        string str_tooltip_font="tooltip_font:";
        string str_tooltip_background="tooltip_background:";
        string str_tooltip_border="tooltip_border:";
        string str_toast_font="toast_font:";
        string str_toast_background="toast_background:";
        string str_toast_border="toast_border:";
        string str_information_font="information_font:";
        string str_information_background="information_background:";
        string str_information_border="information_border:";
        string str_gui_selector_1="gui_selector_1:";
        string str_gui_selector_2="gui_selector_2:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            color_themes[color_themes.size()-1].name=line;
        }
        //Window font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_font)){
            //Clear the data name.
            line.erase(0,str_window_font.length());

            color_themes[color_themes.size()-1].window_font=line;
        }
        //Window title bar
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_title_bar)){
            //Clear the data name.
            line.erase(0,str_window_title_bar.length());

            color_themes[color_themes.size()-1].window_title_bar=line;
        }
        //Window background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_background)){
            //Clear the data name.
            line.erase(0,str_window_background.length());

            color_themes[color_themes.size()-1].window_background=line;
        }
        //Window border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_border)){
            //Clear the data name.
            line.erase(0,str_window_border.length());

            color_themes[color_themes.size()-1].window_border=line;
        }
        //Button font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_button_font)){
            //Clear the data name.
            line.erase(0,str_button_font.length());

            color_themes[color_themes.size()-1].button_font=line;
        }
        //Button background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_button_background)){
            //Clear the data name.
            line.erase(0,str_button_background.length());

            color_themes[color_themes.size()-1].button_background=line;
        }
        //Button background moused
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_button_background_moused)){
            //Clear the data name.
            line.erase(0,str_button_background_moused.length());

            color_themes[color_themes.size()-1].button_background_moused=line;
        }
        //Button background click
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_button_background_click)){
            //Clear the data name.
            line.erase(0,str_button_background_click.length());

            color_themes[color_themes.size()-1].button_background_click=line;
        }
        //Button border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_button_border)){
            //Clear the data name.
            line.erase(0,str_button_border.length());

            color_themes[color_themes.size()-1].button_border=line;
        }
        //Tooltip font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_font)){
            //Clear the data name.
            line.erase(0,str_tooltip_font.length());

            color_themes[color_themes.size()-1].tooltip_font=line;
        }
        //Tooltip background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_background)){
            //Clear the data name.
            line.erase(0,str_tooltip_background.length());

            color_themes[color_themes.size()-1].tooltip_background=line;
        }
        //Tooltip border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_border)){
            //Clear the data name.
            line.erase(0,str_tooltip_border.length());

            color_themes[color_themes.size()-1].tooltip_border=line;
        }
        //Toast font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_font)){
            //Clear the data name.
            line.erase(0,str_toast_font.length());

            color_themes[color_themes.size()-1].toast_font=line;
        }
        //Toast background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_background)){
            //Clear the data name.
            line.erase(0,str_toast_background.length());

            color_themes[color_themes.size()-1].toast_background=line;
        }
        //Toast border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_border)){
            //Clear the data name.
            line.erase(0,str_toast_border.length());

            color_themes[color_themes.size()-1].toast_border=line;
        }
        //Information font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_information_font)){
            //Clear the data name.
            line.erase(0,str_information_font.length());

            color_themes[color_themes.size()-1].information_font=line;
        }
        //Information background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_information_background)){
            //Clear the data name.
            line.erase(0,str_information_background.length());

            color_themes[color_themes.size()-1].information_background=line;
        }
        //Information border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_information_border)){
            //Clear the data name.
            line.erase(0,str_information_border.length());

            color_themes[color_themes.size()-1].information_border=line;
        }
        //GUI selector 1
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_selector_1)){
            //Clear the data name.
            line.erase(0,str_gui_selector_1.length());

            color_themes[color_themes.size()-1].gui_selector_1=line;
        }
        //GUI selector 2
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_selector_2)){
            //Clear the data name.
            line.erase(0,str_gui_selector_2.length());

            color_themes[color_themes.size()-1].gui_selector_2=line;
        }

        //If the line ends the color theme.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</color_theme>")){
            return;
        }
    }
}

void Engine_Interface::load_animation(File_IO_Load* load){
    animations.push_back(Animation());

    double frame_width=0.0;

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_frame_count="frame_count:";
        string str_animation_speed="animation_speed:";
        string str_frame_width="frame_width:";
        string str_end_behavior="end_behavior:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            animations[animations.size()-1].name=line;
        }
        //Frame count
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_count)){
            //Clear the data name.
            line.erase(0,str_frame_count.length());

            animations[animations.size()-1].frame_count=string_stuff.string_to_long(line);
        }
        //Animation speed
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_animation_speed)){
            //Clear the data name.
            line.erase(0,str_animation_speed.length());

            animations[animations.size()-1].animation_speed=string_stuff.string_to_long(line);
        }
        //Frame width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_width)){
            //Clear the data name.
            line.erase(0,str_frame_width.length());

            frame_width=string_stuff.string_to_double(line);
        }
        //End behavior
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_end_behavior)){
            //Clear the data name.
            line.erase(0,str_end_behavior.length());

            animations[animations.size()-1].end_behavior=line;
        }

        //If the line ends the animation.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</animation>")){
            animations[animations.size()-1].setup(frame_width);

            return;
        }
    }
}

void Engine_Interface::load_window(File_IO_Load* load){
    windows.push_back(Window());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_starting_position="starting_position:";
        string str_starting_size="starting_size:";
        string str_width="width:";
        string str_height="height:";
        string str_fit_content="fit_content:";
        string str_title="title:";
        string str_font="font:";
        string str_font_color="font_color:";
        string str_border="border:";
        string str_background="background:";
        string str_scrolling_buttons="scrolling_buttons:";
        string str_close_function="close_function:";
        string str_disallow_closing="disallow_closing:";
        string str_exclusive="exclusive:";
        string str_no_button_sort="no_button_sort:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            windows[windows.size()-1].name=line;
        }
        //Starting position
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_position)){
            //Clear the data name.
            line.erase(0,str_starting_position.length());

            vector<string> start_coords;
            boost::algorithm::split(start_coords,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].x=string_stuff.string_to_long(start_coords[0]);
            windows[windows.size()-1].y=string_stuff.string_to_long(start_coords[1]);

            windows[windows.size()-1].start_x=windows[windows.size()-1].x;
            windows[windows.size()-1].start_y=windows[windows.size()-1].y;
        }
        //Starting size
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_size)){
            //Clear the data name.
            line.erase(0,str_starting_size.length());

            vector<string> start_dimensions;
            boost::algorithm::split(start_dimensions,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].start_width=string_stuff.string_to_long(start_dimensions[0]);
            windows[windows.size()-1].start_height=string_stuff.string_to_long(start_dimensions[1]);
        }
        //Width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_width)){
            //Clear the data name.
            line.erase(0,str_width.length());

            windows[windows.size()-1].w=string_stuff.string_to_long(line);
        }
        //Height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_height)){
            //Clear the data name.
            line.erase(0,str_height.length());

            windows[windows.size()-1].h=string_stuff.string_to_long(line);
        }
        //Fit Content
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fit_content)){
            //Clear the data name.
            line.erase(0,str_fit_content.length());

            windows[windows.size()-1].fit_content=string_stuff.string_to_bool(line);
        }
        //Title
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_title)){
            //Clear the data name.
            line.erase(0,str_title.length());

            windows[windows.size()-1].title=string_stuff.process_newlines(line);
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].font=line;
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].font_color=line;
        }
        //Border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_border)){
            //Clear the data name.
            line.erase(0,str_border.length());

            windows[windows.size()-1].border.name=line;
        }
        //Background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background)){
            //Clear the data name.
            line.erase(0,str_background.length());

            windows[windows.size()-1].background.name=line;
        }
        //Scrolling Buttons
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling_buttons)){
            //Clear the data name.
            line.erase(0,str_scrolling_buttons.length());

            windows[windows.size()-1].scrolling_buttons=line;
        }
        //Close Function
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_close_function)){
            //Clear the data name.
            line.erase(0,str_close_function.length());

            windows[windows.size()-1].close_function=line;
        }
        //Disallow Closing
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_disallow_closing)){
            //Clear the data name.
            line.erase(0,str_disallow_closing.length());

            windows[windows.size()-1].disallow_closing=string_stuff.string_to_bool(line);
        }
        //Exclusive
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_exclusive)){
            //Clear the data name.
            line.erase(0,str_exclusive.length());

            windows[windows.size()-1].exclusive=string_stuff.string_to_bool(line);
        }
        //No Button Sort
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_no_button_sort)){
            //Clear the data name.
            line.erase(0,str_no_button_sort.length());

            windows[windows.size()-1].no_button_sort=string_stuff.string_to_bool(line);
        }

        //If the line begins an information.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"<info>")){
            load_information(load);
        }
        //If the line begins a button.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"<button>")){
            load_button(load);
        }

        //If the line ends the window.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</window>")){
            windows[windows.size()-1].set_dimensions();
            windows[windows.size()-1].center_in_game_window();

            return;
        }
    }
}

void Engine_Interface::load_information(File_IO_Load* load){
    windows[windows.size()-1].informations.push_back(Information());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_location="location:";
        string str_text="text:";
        string str_tooltip_text="tooltip:";
        string str_text_mutable="mutable:";
        string str_max_text_length="max_text_length:";
        string str_allowed_input="allowed_input:";
        string str_scrolling="scrolling:";
        string str_scroll_dimensions="box:";
        string str_sprite="sprite:";
        string str_background_type="background_type:";
        string str_background_opacity="background_opacity:";
        string str_font_color="font_color:";
        string str_special_info_text="special_text:";
        string str_special_info_sprite="special_sprite:";
        string str_font="font:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_location)){
            //Clear the data name.
            line.erase(0,str_location.length());

            vector<string> location;
            boost::algorithm::split(location,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].x=string_stuff.string_to_long(location[0]);
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].y=string_stuff.string_to_long(location[1]);

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].start_x=windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].x;
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].start_y=windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].y;
        }
        //Text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text)){
            //Clear the data name.
            line.erase(0,str_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].text=string_stuff.process_newlines(line);
        }
        //Tooltip text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_text)){
            //Clear the data name.
            line.erase(0,str_tooltip_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].tooltip_text=string_stuff.process_newlines(line);
        }
        //Text mutable
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text_mutable)){
            //Clear the data name.
            line.erase(0,str_text_mutable.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].text_mutable=string_stuff.string_to_bool(line);
        }
        //Max text length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_max_text_length)){
            //Clear the data name.
            line.erase(0,str_max_text_length.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].max_text_length=string_stuff.string_to_long(line);
        }
        //Allowed input
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_allowed_input)){
            //Clear the data name.
            line.erase(0,str_allowed_input.length());

            vector<string> inputs;
            boost::algorithm::split(inputs,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].allowed_input=inputs;
        }
        //Scrolling
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling)){
            //Clear the data name.
            line.erase(0,str_scrolling.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scrolling=string_stuff.string_to_bool(line);
        }
        //Scroll dimensions
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scroll_dimensions)){
            //Clear the data name.
            line.erase(0,str_scroll_dimensions.length());

            vector<string> scroll_dimensions;
            boost::algorithm::split(scroll_dimensions,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scroll_width=string_stuff.string_to_long(scroll_dimensions[0]);
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scroll_height=string_stuff.string_to_long(scroll_dimensions[1]);
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].sprite.name=line;
        }
        //Background type
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background_type)){
            //Clear the data name.
            line.erase(0,str_background_type.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].background_type=line;
        }
        //Background opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background_opacity)){
            //Clear the data name.
            line.erase(0,str_background_opacity.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].background_opacity=string_stuff.string_to_double(line);
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].font_color=line;
        }
        //Special info text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_special_info_text)){
            //Clear the data name.
            line.erase(0,str_special_info_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].special_info_text=line;
        }
        //Special info sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_special_info_sprite)){
            //Clear the data name.
            line.erase(0,str_special_info_sprite.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].special_info_sprite=line;
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].font=line;
        }

        //If the line ends the information.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</info>")){
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].set_dimensions();
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].center_in_window(windows[windows.size()-1].w,windows[windows.size()-1].h);

            return;
        }
    }
}

void Engine_Interface::load_button(File_IO_Load* load){
    windows[windows.size()-1].buttons.push_back(Button());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_location="location:";
        string str_text="text:";
        string str_tooltip_text="tooltip:";
        string str_font="font:";
        string str_font_color="font_color:";
        string str_event_function="event:";
        string str_alt_function1="alt_event1:";
        string str_alt_function2="alt_event2:";
        string str_alt_function3="alt_event3:";
        string str_mouse_over_sound="sound_moused:";
        string str_event_fire_sound="sound_click:";
        string str_sprite="sprite:";
        string str_sprite_moused="sprite_moused:";
        string str_sprite_click="sprite_click:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_location)){
            //Clear the data name.
            line.erase(0,str_location.length());

            vector<string> location;
            boost::algorithm::split(location,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].x=string_stuff.string_to_long(location[0]);
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].y=string_stuff.string_to_long(location[1]);

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].start_x=windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].x;
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].start_y=windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].y;
        }
        //Text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text)){
            //Clear the data name.
            line.erase(0,str_text.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].text=string_stuff.process_newlines(line);
        }
        //Tooltip text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_text)){
            //Clear the data name.
            line.erase(0,str_tooltip_text.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].tooltip_text=string_stuff.process_newlines(line);
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].font=line;
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].font_color=line;
        }
        //Event function
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_event_function)){
            //Clear the data name.
            line.erase(0,str_event_function.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].event_function=line;
        }
        //Alt function 1
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function1)){
            //Clear the data name.
            line.erase(0,str_alt_function1.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function1=line;
        }
        //Alt function 2
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function2)){
            //Clear the data name.
            line.erase(0,str_alt_function2.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function2=line;
        }
        //Alt function 3
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function3)){
            //Clear the data name.
            line.erase(0,str_alt_function3.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function3=line;
        }
        //Mouse over sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mouse_over_sound)){
            //Clear the data name.
            line.erase(0,str_mouse_over_sound.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].mouse_over_sound=line;
        }
        //Event fire sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_event_fire_sound)){
            //Clear the data name.
            line.erase(0,str_event_fire_sound.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].event_fire_sound=line;
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite.name=line;
        }
        //Sprite moused
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite_moused)){
            //Clear the data name.
            line.erase(0,str_sprite_moused.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite_moused.name=line;
        }
        //Sprite click
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite_click)){
            //Clear the data name.
            line.erase(0,str_sprite_click.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite_click.name=line;
        }

        //If the line ends the button.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</button>")){
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].set_dimensions();
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].center_in_window(windows[windows.size()-1].w,windows[windows.size()-1].h);

            return;
        }
    }
}

void Engine_Interface::load_game_command(File_IO_Load* load){
    game_commands.push_back(Game_Command());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_title="title:";
        string str_description="description:";
        string str_developer="developer:";
        string str_key="key:";
        string str_controller_button="controller_button:";
        string str_controller_axis="controller_axis:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            game_commands[game_commands.size()-1].name=line;
        }
        //Title
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_title)){
            //Clear the data name.
            line.erase(0,str_title.length());

            game_commands[game_commands.size()-1].title=line;
        }
        //Description
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_description)){
            //Clear the data name.
            line.erase(0,str_description.length());

            game_commands[game_commands.size()-1].description=string_stuff.process_newlines(line);
        }
        //Developer
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_developer)){
            //Clear the data name.
            line.erase(0,str_developer.length());

            game_commands[game_commands.size()-1].dev=string_stuff.string_to_bool(line);
        }
        //Key
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_key)){
            //Clear the data name.
            line.erase(0,str_key.length());

            game_commands[game_commands.size()-1].key=SDL_GetScancodeFromName(line.c_str());
        }
        //Controller Button
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_button)){
            //Clear the data name.
            line.erase(0,str_controller_button.length());

            game_commands[game_commands.size()-1].button=SDL_GameControllerGetButtonFromString(line.c_str());
        }
        //Controller Axis
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_axis)){
            //Clear the data name.
            line.erase(0,str_controller_axis.length());

            game_commands[game_commands.size()-1].axis=SDL_GameControllerGetAxisFromString(line.c_str());
        }

        //If the line ends the game command.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</game_command>")){
            return;
        }
    }
}

void Engine_Interface::load_game_option(File_IO_Load* load){
    game_options.push_back(Game_Option());

    string option_default="";

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_description="description:";
        string str_default="default:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            game_options[game_options.size()-1].name=line;
        }
        //Description
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_description)){
            //Clear the data name.
            line.erase(0,str_description.length());

            game_options[game_options.size()-1].description=string_stuff.process_newlines(line);
        }
        //Default
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default)){
            //Clear the data name.
            line.erase(0,str_default.length());

            option_default=line;
        }

        //If the line ends the game option.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</game_option>")){
            if(option_default.length()>0){
                game_options[game_options.size()-1].set_value(option_default);
            }

            return;
        }
    }
}

void Engine_Interface::load_game_constant(File_IO_Load* load){
    string constant_name="";
    string constant_value="";

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_value="value:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            constant_name=line;
        }
        //Value
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_value)){
            //Clear the data name.
            line.erase(0,str_value.length());

            constant_value=line;
        }

        //If the line ends the game constant.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</game_constant>")){
            set_game_constant(constant_name,constant_value);

            return;
        }
    }
}

Bitmap_Font* Engine_Interface::get_font(std::string name){
    Bitmap_Font* ptr_font=0;

    for(int i=0;i<fonts.size();i++){
        if(fonts[i].name==name){
            ptr_font=&fonts[i];

            break;
        }
    }

    if(ptr_font==0){
        message_log.add_error("Error accessing font '"+name+"'");
    }

    return ptr_font;
}

Cursor* Engine_Interface::get_cursor(string name){
    Cursor* ptr_cursor=0;

    for(int i=0;i<cursors.size();i++){
        if(cursors[i].name==name){
            ptr_cursor=&cursors[i];

            break;
        }
    }

    if(ptr_cursor==0){
        message_log.add_error("Error accessing cursor '"+name+"'");
    }

    return ptr_cursor;
}

Color* Engine_Interface::get_color(string name){
    Color* ptr_color=0;

    for(int i=0;i<colors.size();i++){
        if(colors[i].name==name){
            ptr_color=&colors[i];

            break;
        }
    }

    if(ptr_color==0){
        message_log.add_error("Error accessing color '"+name+"'");
    }

    return ptr_color;
}

Color_Theme* Engine_Interface::get_color_theme(string name){
    Color_Theme* ptr_color_theme=0;

    for(int i=0;i<color_themes.size();i++){
        if(color_themes[i].name==name){
            ptr_color_theme=&color_themes[i];

            break;
        }
    }

    if(ptr_color_theme==0){
        message_log.add_error("Error accessing color theme '"+name+"'");
    }

    return ptr_color_theme;
}

Animation* Engine_Interface::get_animation(string name){
    Animation* ptr_animation=0;

    for(int i=0;i<animations.size();i++){
        if(animations[i].name==name){
            ptr_animation=&animations[i];

            break;
        }
    }

    if(ptr_animation==0){
        message_log.add_error("Error accessing animation '"+name+"'");
    }

    return ptr_animation;
}

Window* Engine_Interface::get_window(string name){
    Window* ptr_window=0;

    for(int i=0;i<windows.size();i++){
        if(windows[i].name==name){
            ptr_window=&windows[i];

            break;
        }
    }

    if(ptr_window==0){
        message_log.add_error("Error accessing window '"+name+"'");
    }

    return ptr_window;
}

Game_Command* Engine_Interface::get_game_command(string name){
    Game_Command* ptr_object=0;

    for(int i=0;i<game_commands.size();i++){
        if(game_commands[i].name==name){
            ptr_object=&game_commands[i];

            break;
        }
    }

    if(ptr_object==0){
        message_log.add_error("Error accessing game command '"+name+"'");
    }

    return ptr_object;
}

Game_Option* Engine_Interface::get_game_option(string name){
    Game_Option* ptr_object=0;

    for(int i=0;i<game_options.size();i++){
        if(game_options[i].name==name){
            ptr_object=&game_options[i];

            break;
        }
    }

    if(ptr_object==0){
        message_log.add_error("Error accessing game option '"+name+"'");
    }

    return ptr_object;
}

Color_Theme* Engine_Interface::current_color_theme(){
    return get_color_theme(color_theme);
}

bool Engine_Interface::animation_exists(std::string animation_name){
    Animation* ptr_animation=0;

    for(int i=0;i<animations.size();i++){
        if(animations[i].name==animation_name){
            ptr_animation=&animations[i];

            break;
        }
    }

    if(ptr_animation==0){
        return false;
    }
    else{
        return true;
    }
}

void Engine_Interface::rebuild_window_data(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].need_to_rebuild_scrolling_buttons){
            windows[i].need_to_rebuild_scrolling_buttons=false;

            windows[i].build_scrolling_buttons();
        }
    }
}

bool Engine_Interface::is_window_open(Window* window){
    for(int i=0;i<window_z_order.size();i++){
        if(window_z_order[i]==window){
            return true;
        }
    }

    return false;
}

bool Engine_Interface::is_window_on_top(Window* window){
    if(window_z_order.size()>0 && window_z_order[0]==window){
        return true;
    }

    return false;
}

Window* Engine_Interface::get_top_window(){
    if(window_z_order.size()>0){
        return window_z_order[0];
    }

    return 0;
}

void Engine_Interface::bring_window_to_top(Window* window){
    if(is_window_open(window)){
        close_window(window);

        open_window(window);
    }
}

bool Engine_Interface::is_exclusive_window_open(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].on && windows[i].exclusive){
            return true;
        }
    }

    return false;
}

bool Engine_Interface::is_any_window_open(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].on){
            return true;
        }
    }

    return false;
}

int Engine_Interface::open_window_count(){
    int open_windows=0;

    for(int i=0;i<windows.size();i++){
        if(windows[i].on){
            open_windows++;
        }
    }

    return open_windows;
}

void Engine_Interface::close_all_windows(Window* ignore){
    for(int i=0;i<windows.size();i++){
        if(&windows[i]!=ignore){
            windows[i].toggle_on(true,false);
        }
    }
}

void Engine_Interface::close_top_window(){
    if(is_any_window_open()){
        Window* window=window_z_order[0];

        if(!window->disallow_closing){
            window->toggle_on(true,false);
        }
    }
}

bool Engine_Interface::is_mouse_over_window(){
    if(window_under_mouse!=0){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::is_mouse_over_open_window(){
    if(window_under_mouse!=0 && is_window_open(window_under_mouse)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::poll_event(SDL_Event* event_storage){
    if(SDL_PollEvent(event_storage)){
        return true;
    }
    else if(touch_controller_events.size()>0){
        *event_storage=touch_controller_events[0];

        touch_controller_events.erase(touch_controller_events.begin());

        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::mouse_allowed(){
    if(!touch_controls){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::gui_check_scrolling_button(){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                if(top_window->is_button_scrolling(object.index)){
                    for(int i=0;i<10000 && !top_window->is_scrolling_button_on_screen(object.index);i++){
                        if(top_window->off_screen_scrolling_button_direction(object.index)){
                            top_window->scroll_up();
                        }
                        else{
                            top_window->scroll_down();
                        }
                    }
                }
            }
        }
    }
}

void Engine_Interface::reset_gui_selector_chasers(){
    for(int i=0;i<gui_selector_chasers.size();i++){
        gui_selector_chasers[i].x=-1.0;
        gui_selector_chasers[i].y=-1.0;
    }
}

void Engine_Interface::reset_gui_selected_object(){
    gui_selected_object=-1;

    if(gui_mode=="keyboard" || gui_mode=="controller"){
        if(get_gui_object_count()>0){
            gui_selected_object=0;
        }
    }

    gui_check_scrolling_button();

    reset_gui_selector_chasers();
}

void Engine_Interface::set_gui_mode(string new_gui_mode){
    string previous_gui_mode=gui_mode;

    gui_mode=new_gui_mode;

    if(previous_gui_mode!=gui_mode){
        //Only reset selected object if we are switching to or from mouse mode.
        if(previous_gui_mode=="mouse" || gui_mode=="mouse"){
            bool was_console=is_console_selected();
            bool was_chat=is_chat_selected();

            clear_mutable_info();

            reset_gui_selected_object();

            if(was_console){
                console.toggle_on();
            }
            else if(was_chat){
                chat.toggle_on();
            }
        }

        if(gui_mode!="mouse" && gui_mode!="keyboard" && gui_mode!="controller"){
            message_log.add_error("Error setting GUI mode '"+gui_mode+"'");
        }
    }
}

int Engine_Interface::get_gui_object_count(){
    Window* top_window=get_top_window();

    if(top_window!=0){
        int button_count=0;
        int info_count=0;

        for(int i=0;i<top_window->buttons.size();i++){
            if(!top_window->buttons[i].is_x && !top_window->buttons[i].is_divider){
                button_count++;
            }
        }

        for(int i=0;i<top_window->informations.size();i++){
            if(top_window->informations[i].text_mutable || top_window->informations[i].scrolling){
                info_count++;
            }
        }

        return button_count+info_count;
    }
    else{
        return 0;
    }
}

void Engine_Interface::change_gui_selected_object(string direction){
    int object_count=get_gui_object_count();
    int fast_nav=(int)ceil((double)object_count*0.1);

    if(object_count>0){
        clear_mutable_info();

        if(direction=="up"){
            if(--gui_selected_object<0){
                gui_selected_object=object_count-1;
            }
        }
        else if(direction=="down"){
            if(++gui_selected_object>object_count-1){
                gui_selected_object=0;
            }
        }
        else if(direction=="left"){
            gui_selected_object-=fast_nav;

            if(gui_selected_object<0){
                gui_selected_object=object_count-1;
            }
        }
        else if(direction=="right"){
            gui_selected_object+=fast_nav;

            if(gui_selected_object>object_count-1){
                gui_selected_object=0;
            }
        }
        else{
            message_log.add_error("Error navigating GUI in direction '"+direction+"'");
        }

        gui_check_scrolling_button();

        reset_gui_selector_chasers();
    }
    else{
        reset_gui_selected_object();
    }
}

GUI_Object Engine_Interface::get_gui_selected_object(){
    GUI_Object object("none",-1,-1,-1);

    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            vector<GUI_Object> objects;

            for(int i=0;i<top_window->buttons.size();i++){
                if(!top_window->buttons[i].is_x && !top_window->buttons[i].is_divider){
                    int real_y=top_window->buttons[i].y;

                    //If this button is non-scrolling and above the scrolling button area, add it normally.
                    if(!top_window->is_button_scrolling(i) && top_window->buttons[i].y<scrolling_buttons_offset){
                    }
                    //If this button is scrolling, place it at the top of the scrolling button area, offset by its position in the scrolling buttons list.
                    else if(top_window->is_button_scrolling(i)){
                        real_y=scrolling_buttons_offset+top_window->get_scrolling_button_position(i);
                    }
                    //If this button is non-scrolling and below the scrolling button area, offset it by the bottommost scrolling button's height.
                    else{
                        if(top_window->get_scrolling_button_count()>0){
                            real_y+=scrolling_buttons_offset+top_window->get_scrolling_button_count()-1;
                        }
                    }

                    objects.push_back(GUI_Object("button",i,top_window->buttons[i].x,real_y));
                }
            }

            for(int i=0;i<top_window->informations.size();i++){
                if(top_window->informations[i].text_mutable || top_window->informations[i].scrolling){
                    int real_y=top_window->informations[i].y;

                    //If there are scrolling buttons and this info is below the scrolling button area, offset it by the bottommost scrolling button's height.
                    if(top_window->get_scrolling_button_count()>0 && top_window->informations[i].y>=scrolling_buttons_offset){
                        real_y+=scrolling_buttons_offset+top_window->get_scrolling_button_count()-1;
                    }

                    objects.push_back(GUI_Object("information",i,top_window->informations[i].x,real_y));
                }
            }

            if(objects.size()>gui_selected_object){
                if(!top_window->no_button_sort){
                    objects=quick_sort(objects,true);

                    //Build a list of all y values that have at least one duplicate
                    vector<int> y_duplicates_to_sort;
                    vector<int> y_values;
                    for(int i=0;i<objects.size();i++){
                        if(y_values.size()==0){
                            y_values.push_back(objects[i].y);
                        }
                        else{
                            for(int j=0;j<y_values.size();j++){
                                if(objects[i].y==y_values[j]){
                                    if(y_duplicates_to_sort.size()==0){
                                        y_duplicates_to_sort.push_back(y_values[j]);
                                    }
                                    else{
                                        for(int k=0;k<y_duplicates_to_sort.size();k++){
                                            if(y_values[j]==y_duplicates_to_sort[k]){
                                                break;
                                            }

                                            if(k==y_duplicates_to_sort.size()-1){
                                                y_duplicates_to_sort.push_back(y_values[j]);

                                                break;
                                            }
                                        }
                                    }

                                    break;
                                }

                                if(j==y_values.size()-1){
                                    y_values.push_back(objects[i].y);

                                    break;
                                }
                            }
                        }
                    }

                    for(int i=0;i<y_duplicates_to_sort.size();i++){
                        vector<GUI_Object> replacements;
                        int initial_object=-1;

                        for(int j=0;j<objects.size();j++){
                            if(objects[j].y==y_duplicates_to_sort[i]){
                                if(initial_object==-1){
                                    initial_object=j;
                                }

                                replacements.push_back(objects[j]);
                            }
                        }

                        replacements=quick_sort(replacements,false);

                        for(int j=0;j<replacements.size();j++){
                            objects[initial_object+j]=replacements[j];
                        }
                    }
                }

                object=objects[gui_selected_object];
            }
        }

        if(object.type=="none"){
            reset_gui_selected_object();
        }
    }

    return object;
}

Engine_Rect Engine_Interface::get_gui_selected_object_pos(){
    Engine_Rect object_pos;

    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                object_pos.x=top_window->x+button->x;
                object_pos.y=top_window->y+button->y;
                object_pos.w=button->w;
                object_pos.h=button->h;

                if(top_window->is_button_scrolling(object.index)){
                    object_pos.y=top_window->get_scrolling_button_offset(object.index)+button->y;
                }
            }
            else if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                object_pos.x=top_window->x+info->x;
                object_pos.y=top_window->y+info->y;
                object_pos.w=info->w;
                object_pos.h=info->h;
            }
        }
    }

    return object_pos;
}

bool Engine_Interface::is_gui_object_selected(Button* button_to_check){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                if(button_to_check==button){
                    return true;
                }
            }
        }
    }

    return false;
}

bool Engine_Interface::is_gui_object_selected(Information* info_to_check){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                if(info_to_check==info){
                    return true;
                }
            }
        }
    }

    return false;
}

void Engine_Interface::confirm_gui_object(){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                button->mouse_button_down();
                button->mouse_button_up(top_window);
                button->reset_clicked();
            }
            else if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                if(info->text_mutable){
                    info->begin_editing();
                }
            }
        }
    }
}

void Engine_Interface::scroll_gui_object_up(){
    if(is_console_selected()){
        console.info_display.scroll_up(0);
    }
    else if(is_chat_selected()){
        chat.info_display.scroll_up(0);
    }
    else if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                info->scroll_up(top_window->y);
            }
        }
    }
}

void Engine_Interface::scroll_gui_object_down(){
    if(is_console_selected()){
        console.info_display.scroll_down(0);
    }
    else if(is_chat_selected()){
        chat.info_display.scroll_down(0);
    }
    else if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                info->scroll_down(top_window->y);
            }
        }
    }
}

void Engine_Interface::get_mouse_state(int* mouse_x,int* mouse_y){
    SDL_Rect rect;
    SDL_RenderGetViewport(main_window.renderer,&rect);

    float scale_x=0.0f;
    float scale_y=0.0f;
    SDL_RenderGetScale(main_window.renderer,&scale_x,&scale_y);

    SDL_GetMouseState(mouse_x,mouse_y);

    float offset_x=(float)rect.x*scale_x;
    float offset_y=(float)rect.y*scale_y;

    *mouse_x=(int)ceil(((float)*mouse_x-offset_x)/scale_x);
    *mouse_y=(int)ceil(((float)*mouse_y-offset_y)/scale_y);
}

void Engine_Interface::get_rgba_masks(uint32_t* rmask,uint32_t* gmask,uint32_t* bmask,uint32_t* amask){
    if(SDL_BYTEORDER==SDL_BIG_ENDIAN){
        *rmask=0xff000000;
        *gmask=0x00ff0000;
        *bmask=0x0000ff00;
        *amask=0x000000ff;
    }
    else{
        *rmask=0x000000ff;
        *gmask=0x0000ff00;
        *bmask=0x00ff0000;
        *amask=0xff000000;
    }
}

void Engine_Interface::update_window_caption(int render_rate,double ms_per_frame,int logic_frame_rate){
    string msg="";

    //Set the window caption.
    if(option_dev){
        msg=game_title+" (DEV Mode)";
    }
    else{
        msg=game_title;
    }

    msg+=get_game_window_caption();

    if(option_fps){
        msg+="  FPS: "+string_stuff.num_to_string(render_rate);

        msg+="  LUPS: "+string_stuff.num_to_string(logic_frame_rate);

        msg+="  MS/Frame: "+string_stuff.num_to_string(ms_per_frame);
    }

    SDL_SetWindowTitle(main_window.screen,msg.c_str());
}

void Engine_Interface::set_mutable_info(Information* ptr_info){
    ptr_mutable_info=ptr_info;

    if(allow_screen_keyboard()){
        SDL_StartTextInput();
    }
    else if(controller_text_entry_small && gui_mode=="controller"){
        text_entry_small_selector.x=0;
        text_entry_small_selector.y=0;
    }
}

void Engine_Interface::clear_mutable_info(){
    ptr_mutable_info=0;

    if(SDL_IsScreenKeyboardShown(main_window.screen)){
        SDL_StopTextInput();
    }

    if(console.on){
        console.toggle_on();
    }
    if(chat.on){
        chat.toggle_on();
    }
}

bool Engine_Interface::mutable_info_selected(){
    if(ptr_mutable_info!=0){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::mutable_info_this(Information* ptr_info){
    if(ptr_mutable_info==ptr_info){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::add_chat(string message){
    chat.add_text(message);
}

void Engine_Interface::make_notice(string message){
    Window* window=get_window("notice");

    window->informations[0].set_text(message);

    window->set_dimensions();

    window->toggle_on(true,true);
}

void Engine_Interface::make_toast(string message,string length,int custom_length){
    if(message.length()>0){
        int real_length=0;

        if(custom_length>-1){
            real_length=custom_length;
        }
        else{
            if(length=="short"){
                real_length=toast_length_short;
            }
            else if(length=="medium"){
                real_length=toast_length_medium;
            }
            else if(length=="long"){
                real_length=toast_length_long;
            }
            else{
                real_length=toast_length_medium;
            }
        }

        //Real length was in terms of seconds. Now we translate it to frames.
        real_length=(int)ceil((double)real_length*UPDATE_RATE);
        if(real_length<1){
            real_length=1;
        }

        toasts.push_back(Toast(message,1.0/(double)real_length));
    }
}

void Engine_Interface::make_rumble(int controller_number,float strength,uint32_t length){
    if(controller_number==CONTROLLER_ID_ALL || controller_number==CONTROLLER_ID_TOUCH){
        ///Play the rumble on the device with the touch controller, if possible.
        ///I don't think there is a way to do this currently.
    }

    if(controller_number!=CONTROLLER_ID_TOUCH){
        for(int i=0;i<controllers.size();i++){
            if(SDL_GameControllerGetAttached(controllers[i].controller) && controllers[i].haptic!=0 && SDL_HapticOpened(SDL_HapticIndex(controllers[i].haptic))){
                if(controller_number==CONTROLLER_ID_ALL || controller_number==i){
                    SDL_HapticRumblePlay(controllers[i].haptic,strength,length);
                }
            }
        }
    }
}

void Engine_Interface::handle_text_input(string text){
    if(mutable_info_selected()){
        const uint8_t* keystates=SDL_GetKeyboardState(NULL);

        //Remove characters that are not allowed.
        for(int i=0;i<text.length();i++){
            bool keep_char=false;

            if((ptr_mutable_info->allows_input("letters") && ((text[i]>='A' && text[i]<='Z') || (text[i]>='a' && text[i]<='z'))) ||
               (ptr_mutable_info->allows_input("numbers") && text[i]>='0' && text[i]<='9') ||
               (ptr_mutable_info->allows_input("symbols") && ((text[i]>='!' && text[i]<='/') || (text[i]>=':' && text[i]<='@') || (text[i]>='[' && text[i]<='`') || ((unsigned char)text[i]>='{'))) ||
               (ptr_mutable_info->allows_input("space") && text[i]==' ')){
                //Don't allow the console toggle key to be typed.
                if(!(text[i]=='`' && (gui_mode=="mouse" || gui_mode=="keyboard") && !keystates[SDL_SCANCODE_LSHIFT] && !keystates[SDL_SCANCODE_RSHIFT])){
                    keep_char=true;
                }
            }

            if(!keep_char){
                text.erase(text.begin()+i);
                i--;
            }
        }

        //Don't allow the mutable string to exceed its maximum size.
        for(int i=text.length()-1;i>=0 && ptr_mutable_info->text.length()+text.length()>ptr_mutable_info->max_text_length;i--){
            text.erase(text.begin()+i);
        }

        if(text.length()>0){
            ptr_mutable_info->text+=text;
        }
    }
}

bool Engine_Interface::controller_state(int controller_number,SDL_GameControllerButton button){
    if(controller_number==CONTROLLER_ID_ALL || controller_number==CONTROLLER_ID_TOUCH){
        if(touch_controls && touch_controller.check_button_state(button)){
            return true;
        }
    }

    for(int i=0;i<controllers.size();i++){
        if(SDL_GameControllerGetAttached(controllers[i].controller)){
            if(controller_number==CONTROLLER_ID_ALL || controller_number==i){
                if(SDL_GameControllerGetButton(controllers[i].controller,button)){
                    return true;
                }
            }
        }
    }

    return false;
}

int Engine_Interface::controller_state(int controller_number,SDL_GameControllerAxis axis){
    if(controller_number==CONTROLLER_ID_ALL || controller_number==CONTROLLER_ID_TOUCH){
        ///Check the touch controller for the passed axis state.
        ///... but the touch controller doesn't currently have any axes.
    }

    for(int i=0;i<controllers.size();i++){
        if(SDL_GameControllerGetAttached(controllers[i].controller)){
            if(controller_number==CONTROLLER_ID_ALL || controller_number==i){
                int state=SDL_GameControllerGetAxis(controllers[i].controller,axis);

                if(state!=0){
                    return state;
                }
            }
        }
    }

    return 0;
}

int Engine_Interface::game_command_state(string name){
    for(int i=0;i<game_commands.size();i++){
        if(game_commands[i].name==name){
            const uint8_t* keystates=SDL_GetKeyboardState(NULL);

            if(game_commands[i].key_valid() && keystates[game_commands[i].key]){
                return true;
            }
            else if(game_commands[i].button_valid() && controller_state(-1,game_commands[i].button)){
                return true;
            }
            else if(game_commands[i].axis_valid()){
                int state=controller_state(-1,game_commands[i].axis);

                if(state!=0){
                    return state;
                }
            }
        }
    }

    return false;
}

void Engine_Interface::gui_nav_back_android(){
    string window_name="main_menu";
    Window* window=get_window(window_name);

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        if(!game.in_progress && is_window_open(window) && open_window_count()==1){
            quit();
        }
        else{
            close_top_window();
        }
    }
    else{
        quit();
    }
}

void Engine_Interface::gui_nav_toggle_menu_android(){
    string window_name="main_menu";
    if(game.in_progress){
        window_name="ingame_menu";
    }

    Window* window=get_window(window_name);

    if(is_window_open(window) && !is_window_on_top(window)){
        bring_window_to_top(window);
    }
    else{
        window->toggle_on();
    }
}

void Engine_Interface::gui_nav_back_controller(){
    set_gui_mode("controller");

    //I've added this for the file browser
    if(open_window_count()==1){
        game.world.change_directory("..");
    }

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        close_top_window();
    }
}

void Engine_Interface::gui_nav_toggle_menu_controller(){
    set_gui_mode("controller");

    //I've overridden the usual behavior here because there should be no main menu
    quit();

    /**string window_name="main_menu";
    if(game.in_progress){
        window_name="ingame_menu";
    }

    Window* window=get_window(window_name);

    if(is_window_open(window) && !is_window_on_top(window)){
        bring_window_to_top(window);
    }
    else{
        window->toggle_on();
    }*/
}

void Engine_Interface::gui_nav_back_keyboard_and_mouse(){
    if(gui_mode=="controller"){
        set_gui_mode("mouse");
    }

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        close_top_window();
    }
    else{
        if(game.in_progress){
            get_window("ingame_menu")->toggle_on();
        }
        else{
            get_window("main_menu")->toggle_on();
        }
    }
}

void Engine_Interface::gui_nav_confirm(string gui_type){
    set_gui_mode(gui_type);

    confirm_gui_object();
}

void Engine_Interface::gui_nav_up(string gui_type){
    set_gui_mode(gui_type);

    change_gui_selected_object("up");
}

void Engine_Interface::gui_nav_down(string gui_type){
    set_gui_mode(gui_type);

    change_gui_selected_object("down");
}

void Engine_Interface::gui_scroll_up(string gui_type){
    set_gui_mode(gui_type);

    scroll_gui_object_up();
}

void Engine_Interface::gui_scroll_down(string gui_type){
    set_gui_mode(gui_type);

    scroll_gui_object_down();
}

bool Engine_Interface::is_console_selected(){
    if(mutable_info_selected() && mutable_info_this(&console.info_input)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::is_chat_selected(){
    if(mutable_info_selected() && mutable_info_this(&chat.info_input)){
        return true;
    }
    else{
        return false;
    }
}

int Engine_Interface::get_text_input_selected_chunk(){
    int selected_chunk=-1;

    int axis_x=controller_state(-1,SDL_CONTROLLER_AXIS_LEFTX);
    if(axis_x>-controller_dead_zone && axis_x<controller_dead_zone){
        axis_x=0;
    }
    int axis_y=controller_state(-1,SDL_CONTROLLER_AXIS_LEFTY);
    if(axis_y>-controller_dead_zone && axis_y<controller_dead_zone){
        axis_y=0;
    }

    if(axis_x!=0 || axis_y!=0){
        Collision_Rect box_a(0,0,1,1);
        Collision_Rect box_b(axis_x,axis_y,1,1);

        double axis_angle=get_angle_to_rect(box_a,box_b,Collision_Rect(0.0,0.0,0.0,0.0));

        if(axis_angle>=74 && axis_angle<118){
            selected_chunk=0;
        }
        else if(axis_angle>=30 && axis_angle<74){
            selected_chunk=1;
        }
        else if(axis_angle>=338 || axis_angle<30){
            selected_chunk=2;
        }
        else if(axis_angle>=294 && axis_angle<338){
            selected_chunk=3;
        }
        else if(axis_angle>=250 && axis_angle<294){
            selected_chunk=4;
        }
        else if(axis_angle>=206 && axis_angle<250){
            selected_chunk=5;
        }
        else if(axis_angle>=162 && axis_angle<206){
            selected_chunk=6;
        }
        else if(axis_angle>=118 && axis_angle<162){
            selected_chunk=7;
        }
    }

    return selected_chunk;
}

vector<string>* Engine_Interface::get_text_input_character_set(){
    vector<string>* characters=&characters_lower;
    if(controller_state(-1,SDL_CONTROLLER_AXIS_TRIGGERLEFT)>controller_dead_zone){
        characters=&characters_upper;
    }
    else if(controller_state(-1,SDL_CONTROLLER_AXIS_TRIGGERRIGHT)>controller_dead_zone){
        characters=&characters_numbers;
    }

    return characters;
}

void Engine_Interface::input_backspace(){
    if(mutable_info_selected()){
        string deleted_char="";
        deleted_char+=ptr_mutable_info->text[ptr_mutable_info->text.size()-1];

        ptr_mutable_info->text.erase(ptr_mutable_info->text.length()-1);

        if(deleted_char=="\xA"){
            ptr_mutable_info->scroll_offset=-string_stuff.newline_count(ptr_mutable_info->text);
        }
    }
}

void Engine_Interface::input_newline(){
    if(mutable_info_selected()){
        ptr_mutable_info->text+="\\";
        ptr_mutable_info->text+="n";
        ptr_mutable_info->text=string_stuff.process_newlines(ptr_mutable_info->text);

        ptr_mutable_info->scroll_offset=-string_stuff.newline_count(ptr_mutable_info->text);
        ptr_mutable_info->scroll_offset+=ptr_mutable_info->scroll_height-1;
    }
}

bool Engine_Interface::allow_screen_keyboard(){
    if(option_screen_keyboard && SDL_HasScreenKeyboardSupport()){
        return true;
    }
    else{
        return false;
    }
}

string Engine_Interface::get_text_entry_small_character(){
    if(text_entry_small_selector.x<0){
        text_entry_small_selector.x=0;
    }
    else if(text_entry_small_selector.x>12){
        text_entry_small_selector.x=12;
    }

    if(text_entry_small_selector.y<0){
        text_entry_small_selector.y=0;
    }
    else if(text_entry_small_selector.y>7){
        text_entry_small_selector.y=7;
    }

    string choices[13][8];

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][0]=characters_upper[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][1]=characters_upper[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][2]=characters_lower[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][3]=characters_lower[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][4]=characters_numbers[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][5]=characters_symbols[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][6]=characters_symbols[i];
    }
    for(int i=26,n=0;i<39;i++,n++){
        choices[n][7]=characters_symbols[i];
    }

    return choices[text_entry_small_selector.x][text_entry_small_selector.y];
}

void Engine_Interface::prepare_for_input(){
    //Start each frame of updates with the tooltip off.
    tooltip.on=false;

    mouse_over=false;

    if(counter_gui_scroll_axis>0){
        counter_gui_scroll_axis--;
    }

    window_under_mouse=0;

    if(allow_screen_keyboard() && !SDL_IsScreenKeyboardShown(main_window.screen) && mutable_info_selected()){
        clear_mutable_info();
    }

    vector<int> closed_windows;
    for(int i=0;i<windows.size();i++){
        if(!is_window_open(&windows[i])){
            closed_windows.push_back(i);
        }
    }

    int mouse_x=0;
    int mouse_y=0;
    get_mouse_state(&mouse_x,&mouse_y);

    for(int i=0;i<window_z_order.size() && window_under_mouse==0;i++){
        Collision_Rect box_a(mouse_x,mouse_y,cursor_width,cursor_height);
        Collision_Rect box_b(window_z_order[i]->x,window_z_order[i]->y,window_z_order[i]->w,window_z_order[i]->h);

        if(collision_check_rect(box_a,box_b)){
            window_under_mouse=window_z_order[i];
        }
    }

    for(int i=0;i<closed_windows.size() && window_under_mouse==0;i++){
        Collision_Rect box_a(mouse_x,mouse_y,cursor_width,cursor_height);
        Collision_Rect box_b(windows[closed_windows[i]].x,windows[closed_windows[i]].y,windows[closed_windows[i]].w,windows[closed_windows[i]].h);

        if(collision_check_rect(box_a,box_b)){
            window_under_mouse=&windows[closed_windows[i]];
        }
    }
}

bool Engine_Interface::handle_input_events_drag_and_drop(){
    bool event_consumed=false;

    switch(event.type){
    case SDL_DROPFILE:
        if(!event_consumed){
            string file=event.drop.file;
            SDL_free(event.drop.file);

            game.world.adb_push(file);
            get_window("browser")->rebuild_scrolling_buttons();

            event_consumed=true;
        }
        break;
    }

    return event_consumed;
}

bool Engine_Interface::handle_input_events_touch(){
    bool event_consumed=false;

    switch(event.type){
    case SDL_FINGERDOWN:
        if(!event_consumed){
            vector<SDL_GameControllerButton> touch_buttons=touch_controller.check_for_button_press(event.tfinger.x,event.tfinger.y);

            for(int i=0;i<touch_buttons.size();i++){
                SDL_Event touch_controller_event;
                touch_controller_event.type=SDL_CONTROLLERBUTTONDOWN;
                touch_controller_event.common.type=touch_controller_event.type;
                touch_controller_event.common.timestamp=SDL_GetTicks();
                touch_controller_event.cbutton.type=touch_controller_event.type;
                touch_controller_event.cbutton.timestamp=touch_controller_event.common.timestamp;
                touch_controller_event.cbutton.which=CONTROLLER_ID_TOUCH;
                touch_controller_event.cbutton.button=touch_buttons[i];
                touch_controller_event.cbutton.state=SDL_PRESSED;

                touch_controller_events.push_back(touch_controller_event);
            }

            event_consumed=true;
        }
        break;
    }

    return event_consumed;
}

bool Engine_Interface::handle_input_events_command_set(){
    bool event_consumed=false;

    if(configure_command!=-1){
        const uint8_t* keystates=SDL_GetKeyboardState(NULL);

        bool allow_keys_and_buttons=true;
        bool allow_axes=true;

        if(configure_command<game_commands.size()){
            const char* ckey=SDL_GetScancodeName(game_commands[configure_command].key);
            const char* cbutton=SDL_GameControllerGetStringForButton(game_commands[configure_command].button);
            const char* caxis=SDL_GameControllerGetStringForAxis(game_commands[configure_command].axis);

            if(caxis!=0 && game_commands[configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                allow_keys_and_buttons=false;
            }
            else{
                allow_axes=false;
            }
        }

        switch(event.type){
            case SDL_CONTROLLERBUTTONDOWN:
                if(!event_consumed && (event.cbutton.button==SDL_CONTROLLER_BUTTON_START || allow_keys_and_buttons)){
                    if(event.cbutton.button!=SDL_CONTROLLER_BUTTON_START){
                        if(configure_command<game_commands.size()){
                            game_commands[configure_command].button=(SDL_GameControllerButton)event.cbutton.button;
                        }

                        save_game_commands();
                    }

                    engine_interface.get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;

            case SDL_KEYDOWN:
                if(!event_consumed && event.key.repeat==0 &&
                   (event.key.keysym.scancode==SDL_SCANCODE_ESCAPE || event.key.keysym.scancode==SDL_SCANCODE_AC_BACK || allow_keys_and_buttons)){
                    if(event.key.keysym.scancode!=SDL_SCANCODE_ESCAPE && event.key.keysym.scancode!=SDL_SCANCODE_AC_BACK && event.key.keysym.scancode!=SDL_SCANCODE_MENU){
                        if(configure_command<game_commands.size()){
                            game_commands[configure_command].key=event.key.keysym.scancode;
                        }

                        save_game_commands();
                    }

                    engine_interface.get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;

            case SDL_CONTROLLERAXISMOTION:
                if(!event_consumed && allow_axes){
                    if(configure_command<game_commands.size()){
                        game_commands[configure_command].axis=(SDL_GameControllerAxis)event.caxis.axis;
                    }

                    save_game_commands();
                    engine_interface.get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;
        }
    }

    return event_consumed;
}

bool Engine_Interface::handle_input_events(bool event_ignore_command_set){
    bool event_consumed=false;

    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    switch(event.type){
        case SDL_CONTROLLERDEVICEADDED:
            if(SDL_IsGameController(event.cdevice.which)){
                controllers.push_back(Controller(SDL_GameControllerOpen(event.cdevice.which)));

                Controller* controller_object=&controllers[controllers.size()-1];
                SDL_GameController* controller=controller_object->controller;

                if(controller!=0){
                    controller_object->instance_id=SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));

                    if(SDL_JoystickIsHaptic(SDL_GameControllerGetJoystick(controller))){
                        controller_object->haptic=SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));

                        if(controller_object->haptic!=0){
                            if(SDL_HapticRumbleSupported(controller_object->haptic)){
                                if(SDL_HapticRumbleInit(controller_object->haptic)!=0){
                                    string msg="Error initializing rumble for haptic on controller "+string_stuff.num_to_string(event.cdevice.which)+": ";
                                    msg+=SDL_GetError();
                                    message_log.add_error(msg);

                                    SDL_HapticClose(controller_object->haptic);
                                    controller_object->haptic=0;
                                }
                            }
                        }
                        else{
                            string msg="Error opening haptic for controller "+string_stuff.num_to_string(event.cdevice.which)+": ";
                            msg+=SDL_GetError();
                            message_log.add_error(msg);
                        }
                    }
                }
                else{
                    string msg="Error opening controller "+string_stuff.num_to_string(event.cdevice.which)+": ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);

                    controllers.pop_back();
                }
            }
            else{
                string joystick_name=SDL_JoystickNameForIndex(event.cdevice.which);
                message_log.add_error("Joystick \""+joystick_name+"\" detected, but not supported by the game controller interface.");
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            for(int i=0;i<controllers.size();i++){
                if(event.cdevice.which==controllers[i].instance_id){
                    if(controllers[i].haptic!=0 && SDL_HapticOpened(SDL_HapticIndex(controllers[i].haptic))){
                        SDL_HapticClose(controllers[i].haptic);
                    }

                    SDL_GameControllerClose(controllers[i].controller);

                    controllers.erase(controllers.begin()+i);

                    break;
                }
            }
            break;

        case SDL_TEXTINPUT:
            if(mutable_info_selected()){
                if(!event_consumed){
                    string text=event.text.text;

                    handle_text_input(text);

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERAXISMOTION:
            if(!event_ignore_command_set){
                //GUI nav controller axis up
                if(!event_consumed && is_any_window_open() && !console.on && !mutable_info_selected() && gui_axis_nav_last_direction!="up" &&
                   event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && event.caxis.value<-controller_dead_zone){
                    gui_axis_nav_last_direction="up";

                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller axis down
                if(!event_consumed && is_any_window_open() && !console.on && !mutable_info_selected() && gui_axis_nav_last_direction!="down" &&
                   event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && event.caxis.value>controller_dead_zone){
                    gui_axis_nav_last_direction="down";

                    gui_nav_down("controller");

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            if(!event_ignore_command_set){
                if(mutable_info_selected() && !allow_screen_keyboard()){
                    if(!controller_text_entry_small){
                        if(!event_consumed && ptr_mutable_info->allows_input("newline") && ptr_mutable_info->text.length()<ptr_mutable_info->max_text_length &&
                           event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            input_newline();

                            event_consumed=true;
                        }

                        if(!event_consumed && is_console_selected() && event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            console.do_command();

                            event_consumed=true;
                        }
                        else if(!event_consumed && is_chat_selected() && event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            chat.send_chat();

                            event_consumed=true;
                        }

                        if(!event_consumed && ptr_mutable_info->allows_input("backspace") && event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSHOULDER && ptr_mutable_info->text.length()>0){
                            input_backspace();

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_RIGHTSHOULDER){
                            handle_text_input(" ");

                            event_consumed=true;
                        }
                        if(!event_consumed && (event.cbutton.button==SDL_CONTROLLER_BUTTON_A || event.cbutton.button==SDL_CONTROLLER_BUTTON_B ||
                                               event.cbutton.button==SDL_CONTROLLER_BUTTON_X || event.cbutton.button==SDL_CONTROLLER_BUTTON_Y)){
                            vector<string>* characters=get_text_input_character_set();
                            int selected_chunk=get_text_input_selected_chunk();

                            if(selected_chunk>=0 && selected_chunk<8){
                                int character_offset=0;
                                if(event.cbutton.button==SDL_CONTROLLER_BUTTON_X){
                                    character_offset=0;
                                }
                                else if(event.cbutton.button==SDL_CONTROLLER_BUTTON_Y){
                                    character_offset=1;
                                }
                                else if(event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                                    character_offset=2;
                                }
                                else if(event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                                    character_offset=3;
                                }

                                handle_text_input(characters->at((selected_chunk*4)+character_offset));

                                event_consumed=true;
                            }
                        }
                    }
                    else{
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){
                            if(--text_entry_small_selector.x<0){
                                text_entry_small_selector.x=12;
                            }

                            sound_system.play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
                            if(++text_entry_small_selector.x>=13){
                                text_entry_small_selector.x=0;
                            }

                            sound_system.play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                            if(--text_entry_small_selector.y<0){
                                text_entry_small_selector.y=7;
                            }

                            sound_system.play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                            if(++text_entry_small_selector.y>=8){
                                text_entry_small_selector.y=0;
                            }

                            sound_system.play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                            if(ptr_mutable_info->allows_input("backspace") && ptr_mutable_info->text.length()>0){
                                input_backspace();
                            }
                            else{
                                gui_nav_back_controller();
                            }

                            sound_system.play_sound("text_input_small_select");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_START){
                            text_entry_small_selector.x=12;
                            text_entry_small_selector.y=7;

                            sound_system.play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                            string text_entry_char=get_text_entry_small_character();

                            if((int)text_entry_char[0]==1){
                                if(ptr_mutable_info->allows_input("backspace") && ptr_mutable_info->text.length()>0){
                                    input_backspace();
                                }
                            }
                            else if((int)text_entry_char[0]==2){
                                if(is_console_selected()){
                                    console.do_command();
                                }
                                else if(is_chat_selected()){
                                    chat.send_chat();
                                }
                                else{
                                    clear_mutable_info();
                                }
                            }
                            else{
                                handle_text_input(text_entry_char);
                            }

                            sound_system.play_sound("text_input_small_select");

                            event_consumed=true;
                        }
                    }
                }

                //GUI nav controller back
                if(!event_consumed && (mutable_info_selected() || is_any_window_open()) && event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                    gui_nav_back_controller();

                    event_consumed=true;
                }
                //GUI nav controller toggle menu
                if(!event_consumed && event.cbutton.button==SDL_CONTROLLER_BUTTON_START){
                    gui_nav_toggle_menu_controller();

                    event_consumed=true;
                }
                //GUI nav controller confirm
                if(!event_consumed && !mutable_info_selected() && is_any_window_open() && event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                    gui_nav_confirm("controller");

                    event_consumed=true;
                }
                //GUI nav controller up
                if(!event_consumed && is_any_window_open() && !console.on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller down
                if(!event_consumed && is_any_window_open() && !console.on && event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                    gui_nav_down("controller");

                    event_consumed=true;
                }
            }
            break;

        case SDL_KEYDOWN:
            if(!event_ignore_command_set){
                if(mutable_info_selected()){
                    if(!event_consumed && ptr_mutable_info->allows_input("newline") && ptr_mutable_info->text.length()<ptr_mutable_info->max_text_length &&
                       (event.key.keysym.scancode==SDL_SCANCODE_RETURN || event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        input_newline();

                        event_consumed=true;
                    }

                    if(!event_consumed && event.key.repeat==0 && is_console_selected() && (event.key.keysym.scancode==SDL_SCANCODE_RETURN || event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        console.do_command();

                        event_consumed=true;
                    }
                    else if(!event_consumed && event.key.repeat==0 && is_chat_selected() && (event.key.keysym.scancode==SDL_SCANCODE_RETURN || event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        chat.send_chat();

                        event_consumed=true;
                    }

                    if(!event_consumed && ptr_mutable_info->allows_input("backspace") && event.key.keysym.scancode==SDL_SCANCODE_BACKSPACE && ptr_mutable_info->text.length()>0){
                        input_backspace();

                        event_consumed=true;
                    }
                }

                if(event.key.repeat==0){
                    //Screenshot
                    //I've changed this just for this program, because I really want F5 for refreshing the browser.
                    if(!event_consumed && event.key.keysym.scancode==SDL_SCANCODE_F12){
                        main_window.screenshot();

                        event_consumed=true;
                    }

                    //Toggle fullscreen
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && (event.key.keysym.scancode==SDL_SCANCODE_RETURN || event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        change_option("cl_fullscreen_state",string_stuff.bool_to_string(!option_fullscreen));
                        reload();

                        event_consumed=true;
                    }

                    //Quit game
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && event.key.keysym.scancode==SDL_SCANCODE_F4){
                        quit();

                        event_consumed=true;
                    }

                    //Toggle GUI display
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && event.key.keysym.scancode==SDL_SCANCODE_Z){
                        hide_gui=!hide_gui;

                        event_consumed=true;
                    }

                    //GUI nav Android back
                    if(!event_consumed && event.key.keysym.scancode==SDL_SCANCODE_AC_BACK){
                        gui_nav_back_android();

                        event_consumed=true;
                    }
                    //GUI nav Android toggle menu
                    if(!event_consumed && event.key.keysym.scancode==SDL_SCANCODE_MENU){
                        gui_nav_toggle_menu_android();

                        event_consumed=true;
                    }
                    //GUI nav keyboard back
                    if(!event_consumed && event.key.keysym.scancode==SDL_SCANCODE_ESCAPE){
                        gui_nav_back_keyboard_and_mouse();

                        event_consumed=true;
                    }
                    //GUI nav keyboard confirm
                    if(!event_consumed && !mutable_info_selected() && is_any_window_open() && (event.key.keysym.scancode==SDL_SCANCODE_RETURN || event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        gui_nav_confirm("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard up
                    if(!event_consumed && is_any_window_open() && !console.on && event.key.keysym.scancode==SDL_SCANCODE_UP){
                        gui_nav_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard down
                    if(!event_consumed && is_any_window_open() && !console.on && event.key.keysym.scancode==SDL_SCANCODE_DOWN){
                        gui_nav_down("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll up
                    if(!event_consumed && (is_any_window_open() || console.on || chat.on) && event.key.keysym.scancode==SDL_SCANCODE_PAGEUP){
                        gui_scroll_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll down
                    if(!event_consumed && (is_any_window_open() || console.on || chat.on) && event.key.keysym.scancode==SDL_SCANCODE_PAGEDOWN){
                        gui_scroll_down("keyboard");

                        event_consumed=true;
                    }
                }

                //Paste
                if(!event_consumed && (keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL]) && event.key.keysym.scancode==SDL_SCANCODE_V){
                    if(SDL_HasClipboardText()){
                        char* text=SDL_GetClipboardText();
                        string str_text=text;
                        SDL_free(text);

                        handle_text_input(str_text);
                    }

                    event_consumed=true;
                }
            }
            break;

        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: case SDL_MOUSEWHEEL:
            //GUI nav mouse
            if(!event_consumed && mouse_allowed()){
                set_gui_mode("mouse");
            }
            break;
    }

    if(!console.on && !event_ignore_command_set && mouse_allowed()){
        if(window_under_mouse!=0){
            if(!event_consumed){
                event_consumed=window_under_mouse->handle_input_events();
            }
        }

        if(!event_consumed){
            event_consumed=chat.handle_input_events();
        }
    }

    if(!event_consumed && !event_ignore_command_set){
        event_consumed=console.handle_input_events();
    }

    return event_consumed;
}

void Engine_Interface::handle_input_states(){
    if(!console.on){
        Window* top_window=get_top_window();

        for(int i=0;i<windows.size();i++){
            if(top_window==0 || top_window!=&windows[i]){
                windows[i].reset_buttons_moused_over();
            }
        }

        if(top_window!=0){
            top_window->handle_input_states();
        }

        chat.handle_input_states();
    }

    int axis_left_x=controller_state(-1,SDL_CONTROLLER_AXIS_LEFTX);
    int axis_left_y=controller_state(-1,SDL_CONTROLLER_AXIS_LEFTY);
    if(axis_left_x>=-controller_dead_zone && axis_left_x<=controller_dead_zone && axis_left_y>=-controller_dead_zone && axis_left_y<=controller_dead_zone){
        gui_axis_nav_last_direction="none";
    }

    if(is_any_window_open() || console.on || chat.on){
        int axis_right_y=controller_state(-1,SDL_CONTROLLER_AXIS_RIGHTY);
        if(counter_gui_scroll_axis==0 && axis_right_y<-controller_dead_zone){
            counter_gui_scroll_axis=axis_scroll_rate;

            gui_scroll_up("controller");
        }
        else if(counter_gui_scroll_axis==0 && axis_right_y>controller_dead_zone){
            counter_gui_scroll_axis=axis_scroll_rate;

            gui_scroll_down("controller");
        }
    }

    console.handle_input_states();
}

string Engine_Interface::get_system_info(){
    string msg="";

    int logical_width=0;
    int logical_height=0;
    SDL_RenderGetLogicalSize(main_window.renderer,&logical_width,&logical_height);

    SDL_Rect rect;
    SDL_RenderGetViewport(main_window.renderer,&rect);

    float scale_x=0.0;
    float scale_y=0.0;
    SDL_RenderGetScale(main_window.renderer,&scale_x,&scale_y);

    int actual_width=0;
    int actual_height=0;
    SDL_GetRendererOutputSize(main_window.renderer,&actual_width,&actual_height);

    SDL_RendererInfo info;
    SDL_GetRendererInfo(main_window.renderer,&info);
    string renderer_name=info.name;

    int mouse_x=0;
    int mouse_y=0;
    get_mouse_state(&mouse_x,&mouse_y);

    int mouse_real_x=0;
    int mouse_real_y=0;
    SDL_GetMouseState(&mouse_real_x,&mouse_real_y);

    int power_seconds=0;
    int power_percentage=0;
    SDL_PowerState power_state=SDL_GetPowerInfo(&power_seconds,&power_percentage);
    string str_power_seconds=string_stuff.time_string(power_seconds)+" remaining";
    string str_power_percentage=string_stuff.num_to_string(power_percentage)+"%";

    if(engine_interface.controllers.size()>0){
        msg+="Controllers ("+string_stuff.num_to_string(engine_interface.controllers.size())+"):\n\n";

        for(int i=0;i<engine_interface.controllers.size();i++){
            if(SDL_GameControllerGetAttached(engine_interface.controllers[i].controller)){
                bool haptic=false;
                if(engine_interface.controllers[i].haptic!=0 && SDL_HapticOpened(SDL_HapticIndex(engine_interface.controllers[i].haptic))){
                    haptic=true;
                }

                msg+=string_stuff.num_to_string(i)+".\n";
                msg+="Controller Name: ";
                msg+=SDL_GameControllerName(engine_interface.controllers[i].controller);
                msg+="\n";
                msg+="Joystick Name: ";
                msg+=SDL_JoystickName(SDL_GameControllerGetJoystick(engine_interface.controllers[i].controller));
                msg+="\n";
                msg+="Instance ID: ";
                msg+=string_stuff.num_to_string(engine_interface.controllers[i].instance_id);
                msg+="\n";
                msg+="Has Rumble: "+string_stuff.bool_to_string(haptic);
                msg+="\n\n";
            }
        }
    }

    msg+="Resolution (Logical): "+string_stuff.num_to_string(logical_width)+" x "+string_stuff.num_to_string(logical_height)+"\n";
    msg+="Logical Viewport: "+string_stuff.num_to_string(rect.x)+","+string_stuff.num_to_string(rect.y)+","+string_stuff.num_to_string(rect.w)+","+string_stuff.num_to_string(rect.h)+"\n";
    msg+="Render Scale: "+string_stuff.num_to_string(scale_x)+","+string_stuff.num_to_string(scale_y)+"\n";
    msg+="Resolution (Actual): "+string_stuff.num_to_string(actual_width)+" x "+string_stuff.num_to_string(actual_height)+"\n";
    msg+="Renderer: "+renderer_name+"\n";
    msg+="Max Texture Size: "+string_stuff.num_to_string(info.max_texture_width)+" x "+string_stuff.num_to_string(info.max_texture_height)+"\n";
    msg+="Current Gui Mode: "+string_stuff.first_letter_capital(engine_interface.gui_mode)+"\n";

    msg+="Mouse Position (Logical): "+string_stuff.num_to_string(mouse_x)+","+string_stuff.num_to_string(mouse_y)+"\n";
    msg+="Mouse Position (Actual): "+string_stuff.num_to_string(mouse_real_x)+","+string_stuff.num_to_string(mouse_real_y)+"\n";

    msg+="Power State: ";
    if(power_state==SDL_POWERSTATE_UNKNOWN){
        msg+="Unknown";
    }
    else if(power_state==SDL_POWERSTATE_ON_BATTERY){
        msg+="Unplugged, on battery";
    }
    else if(power_state==SDL_POWERSTATE_NO_BATTERY){
        msg+="Plugged in, no battery";
    }
    else if(power_state==SDL_POWERSTATE_CHARGING){
        msg+="Plugged in, charging";
    }
    else if(power_state==SDL_POWERSTATE_CHARGED){
        msg+="Plugged in, fully charged";
    }
    if(power_seconds!=-1 || power_percentage!=-1){
        msg+=" (";
    }
    if(power_percentage!=-1){
        msg+=str_power_percentage;
    }
    if(power_seconds!=-1){
        if(power_percentage!=-1){
            msg+=", ";
        }
        msg+=str_power_seconds;
    }
    if(power_seconds!=-1 || power_percentage!=-1){
        msg+=")";
    }

    return msg;
}

void Engine_Interface::animate(){
    for(int i=0;i<windows.size();i++){
        windows[i].animate();
    }

    console.animate();
    chat.animate();

    if(toasts.size()>0){
        toasts[0].animate();

        if(toasts[0].is_done()){
            toasts.erase(toasts.begin());
        }
    }

    for(int i=0;i<cursors.size();i++){
        cursors[i].animate();
    }

    animate_gui_selector_chasers();

    text_selector.animate();

    //Update the text cursor.
    if(++counter_cursor>=(int)ceil((48.0/1000.0)*UPDATE_RATE)){
        counter_cursor=0;

        if(cursor_opacity==10){
            cursor_fade_direction=false;
        }
        else if(cursor_opacity==0){
            cursor_fade_direction=true;
        }

        if(cursor_fade_direction && cursor_opacity<10){
            cursor_opacity+=1;
        }
        else if(!cursor_fade_direction && cursor_opacity>0){
            cursor_opacity-=1;
        }
    }
}

void Engine_Interface::animate_gui_selector_chasers(){
    Engine_Rect object_pos=get_gui_selected_object_pos();

    if(object_pos.x!=-1){
        double distance_to_cover=object_pos.w*2+object_pos.h*2;
        double move_speed=distance_to_cover/(5760.0/UPDATE_RATE);

        for(int i=0;i<gui_selector_chasers.size();i++){
            if(gui_selector_chasers[i].x>(double)object_pos.x && gui_selector_chasers[i].x<(double)object_pos.x+(double)object_pos.w && gui_selector_chasers[i].y==(double)object_pos.y){
                gui_selector_chasers[i].x+=move_speed;

                if(gui_selector_chasers[i].x>=(double)object_pos.x+(double)object_pos.w){
                    gui_selector_chasers[i].x=(double)object_pos.x+(double)object_pos.w;
                    gui_selector_chasers[i].y+=move_speed;
                }
            }
            else if(gui_selector_chasers[i].x>(double)object_pos.x && gui_selector_chasers[i].x<(double)object_pos.x+(double)object_pos.w && gui_selector_chasers[i].y==(double)object_pos.y+(double)object_pos.h){
                gui_selector_chasers[i].x-=move_speed;

                if(gui_selector_chasers[i].x<=(double)object_pos.x){
                    gui_selector_chasers[i].x=(double)object_pos.x;
                    gui_selector_chasers[i].y-=move_speed;
                }
            }
            else if(gui_selector_chasers[i].y>(double)object_pos.y && gui_selector_chasers[i].y<(double)object_pos.y+(double)object_pos.h && gui_selector_chasers[i].x==(double)object_pos.x){
                gui_selector_chasers[i].y-=move_speed;

                if(gui_selector_chasers[i].y<=(double)object_pos.y){
                    gui_selector_chasers[i].y=(double)object_pos.y;
                    gui_selector_chasers[i].x+=move_speed;
                }
            }
            else if(gui_selector_chasers[i].y>(double)object_pos.y && gui_selector_chasers[i].y<(double)object_pos.y+(double)object_pos.h && gui_selector_chasers[i].x==(double)object_pos.x+(double)object_pos.w){
                gui_selector_chasers[i].y+=move_speed;

                if(gui_selector_chasers[i].y>=(double)object_pos.y+(double)object_pos.h){
                    gui_selector_chasers[i].y=(double)object_pos.y+(double)object_pos.h;
                    gui_selector_chasers[i].x-=move_speed;
                }
            }
            else{
                if(i==0){
                    gui_selector_chasers[i].x=(double)object_pos.x+move_speed;
                    gui_selector_chasers[i].y=(double)object_pos.y;
                }
                else if(i==1){
                    gui_selector_chasers[i].x=(double)object_pos.x+(double)object_pos.w-move_speed;
                    gui_selector_chasers[i].y=(double)object_pos.y+(double)object_pos.h;
                }
            }
        }
    }
}

void Engine_Interface::render_gui_selector(){
    Engine_Rect object_pos=get_gui_selected_object_pos();

    if(object_pos.x!=-1){
        if(gui_selector_style=="standard" || gui_selector_style=="corners" || gui_selector_style=="chasers"){
            double thickness=gui_border_thickness;
            double offset=gui_border_thickness;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                thickness+=2.0;
                offset+=2.0;
            }

            render_rectangle_empty(object_pos.x-offset,object_pos.y-offset,object_pos.w+offset*2.0,object_pos.h+offset*2.0,1.0,current_color_theme()->gui_selector_1,thickness);
        }

        if(gui_selector_style=="corners"){
            double corner_size=gui_border_thickness+4.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                corner_size+=2.0;
            }

            render_rectangle(object_pos.x-corner_size/2.0+1,object_pos.y-corner_size/2.0+1,corner_size,corner_size,1.0,current_color_theme()->gui_selector_2);
            render_rectangle(object_pos.x+object_pos.w-corner_size/2.0-1,object_pos.y-corner_size/2.0+1,corner_size,corner_size,1.0,current_color_theme()->gui_selector_2);
            render_rectangle(object_pos.x-corner_size/2.0+1,object_pos.y+object_pos.h-corner_size/2.0-1,corner_size,corner_size,1.0,current_color_theme()->gui_selector_2);
            render_rectangle(object_pos.x+object_pos.w-corner_size/2.0-1,object_pos.y+object_pos.h-corner_size/2.0-1,corner_size,corner_size,1.0,current_color_theme()->gui_selector_2);
        }

        if(gui_selector_style=="underline"){
            double thickness=gui_border_thickness+1.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                thickness+=3.0;
            }

            for(int i=0;i<thickness;i++){
                render_line(object_pos.x,object_pos.y+object_pos.h+i,object_pos.x+object_pos.w-1,object_pos.y+object_pos.h+i,1.0,current_color_theme()->gui_selector_1);
            }
        }

        if(gui_selector_style=="chasers"){
            double chaser_size=gui_border_thickness+4.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                chaser_size+=2.0;
            }

            for(int i=0;i<gui_selector_chasers.size();i++){
                render_rectangle(gui_selector_chasers[i].x-chaser_size/2.0,gui_selector_chasers[i].y-chaser_size/2.0,chaser_size,chaser_size,1.0,current_color_theme()->gui_selector_2);
            }
        }
    }
}

void Engine_Interface::render_small_text_inputter(){
    render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,0.75,current_color_theme()->window_border);
    render_rectangle(window_border_thickness,window_border_thickness,main_window.SCREEN_WIDTH-window_border_thickness*2.0,main_window.SCREEN_HEIGHT-window_border_thickness*2.0,0.75,current_color_theme()->window_background);

    if(mutable_info_selected()){
        Bitmap_Font* font=get_font("small");

        font->show(window_border_thickness+2.0,window_border_thickness+2.0,ptr_mutable_info->text,current_color_theme()->window_font);
    }

    Bitmap_Font* font=get_font("standard");

    double buttons_start_y=window_border_thickness+2.0+font->spacing_y*2.0;

    double x_offset=((double)main_window.SCREEN_WIDTH-font->get_letter_width()*13.0)/2.0+window_border_thickness;
    double offset_y=buttons_start_y;

    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_upper[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<characters_upper.size();i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_upper[i],current_color_theme()->window_font);
    }

    offset_y+=font->get_letter_height();
    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_lower[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<characters_lower.size();i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_lower[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();

    for(int i=0;i<characters_numbers.size();i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_numbers[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();

    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_symbols[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<26;i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_symbols[i],current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=26;i<characters_symbols.size();i++){
        font->show(x_offset+(i-26)*font->get_letter_width(),offset_y,characters_symbols[i],current_color_theme()->window_font);
    }

    text_selector.render(x_offset+text_entry_small_selector.x*font->get_letter_width()-(text_selector.get_width()-font->get_letter_width())/2.0,buttons_start_y+text_entry_small_selector.y*font->get_letter_height()-(text_selector.get_height()-font->get_letter_height())/2.0);
}

void Engine_Interface::render_text_inputter(){
    vector<string>* characters=get_text_input_character_set();

    int selected_chunk=get_text_input_selected_chunk();

    double outer_radius=main_window.SCREEN_WIDTH/5;
    double outer_center_y=outer_radius+10;

    //The topmost instructions' y position.
    int words_y=outer_center_y+outer_radius+10;

    int x_adjust=0;
    if(mutable_info_selected()){
        Engine_Rect object_pos;

        if(is_console_selected()){
            object_pos.x=console.info_input.x;
            object_pos.y=console.info_input.y;
            object_pos.w=console.info_input.w;
            object_pos.h=console.info_input.h;
        }
        else if(is_chat_selected()){
            object_pos.x=chat.info_input.x;
            object_pos.y=chat.info_input.y;
            object_pos.w=chat.info_input.w;
            object_pos.h=chat.info_input.h;
        }
        else{
            object_pos=get_gui_selected_object_pos();
        }

        if(object_pos.x!=-1){
            if(object_pos.x<=main_window.SCREEN_WIDTH/2){
                x_adjust=main_window.SCREEN_WIDTH/2;
            }

            if(object_pos.y<=main_window.SCREEN_HEIGHT/2){
                outer_center_y=main_window.SCREEN_HEIGHT-outer_radius-10;
                words_y=outer_center_y-outer_radius-120;
            }
        }
    }

    double outer_center_x=x_adjust+main_window.SCREEN_WIDTH/4;

    Bitmap_Font* font=get_font("standard");

    render_rectangle(x_adjust,0,main_window.SCREEN_WIDTH/2,main_window.SCREEN_HEIGHT,0.75,current_color_theme()->window_background);
    render_rectangle_empty(x_adjust,0,main_window.SCREEN_WIDTH/2,main_window.SCREEN_HEIGHT,0.75,current_color_theme()->window_border,2.0);
    render_circle(outer_center_x+4,outer_center_y+4,outer_radius,1.0,"ui_black");
    render_circle(outer_center_x,outer_center_y,outer_radius,1.0,current_color_theme()->window_border);

    render_circle(outer_center_x,outer_center_y,16.0,1.0,"ui_gray");
    font->show(outer_center_x-12.0,outer_center_y-8.0,"LS","ui_white");

    string text="";

    text="(Press for Return)";
    font->show(x_adjust+(main_window.SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,outer_center_y-8.0+font->spacing_y,text,"ui_white");

    text="[LB] Backspace";
    font->show(x_adjust+100,words_y,text,current_color_theme()->window_font);
    text="Space [RB]";
    font->show(x_adjust+main_window.SCREEN_WIDTH/2-text.length()*font->spacing_x-100,words_y,text,current_color_theme()->window_font);

    text="(LT) Caps";
    font->show(x_adjust+100,words_y+30,text,current_color_theme()->window_font);
    text="Numbers (RT)";
    font->show(x_adjust+main_window.SCREEN_WIDTH/2-text.length()*font->spacing_x-100,words_y+30,text,current_color_theme()->window_font);

    if(selected_chunk==-1){
        if(is_console_selected()){
            text="(A) Auto-complete";
            font->show(x_adjust+(main_window.SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,words_y+60,text,current_color_theme()->window_font);
        }

        text="(B) Back/Done";
        font->show(x_adjust+(main_window.SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,words_y+90,text,current_color_theme()->window_font);
    }

    int character_chunk=0;
    int character=0;

    for(double angle=-90.0;angle<270.0;angle+=45.0){
        double mid_radius=outer_radius*0.75;
        double inner_radius=50.0;

        double inner_center_x=outer_center_x+mid_radius*cos(angle*(M_PI/180.0));
        double inner_center_y=outer_center_y+mid_radius*sin(angle*(M_PI/180.0));

        string outer_circle_color=current_color_theme()->window_background;
        if(character_chunk==selected_chunk){
            outer_circle_color=current_color_theme()->window_title_bar;
        }

        render_circle(inner_center_x,inner_center_y,inner_radius,1.0,outer_circle_color);

        for(double letter_angle=-180.0;letter_angle<180.0;letter_angle+=90.0){
            double letter_circle_radius=inner_radius*0.75;
            double letter_radius=16.0;

            string circle_color="text_input_blue";
            if(letter_angle==-180.0){
                circle_color="text_input_blue";
            }
            else if(letter_angle==-90.0){
                circle_color="text_input_yellow";
            }
            else if(letter_angle==0.0){
                circle_color="text_input_red";
            }
            else if(letter_angle==90.0){
                circle_color="text_input_green";
            }

            double letter_center_x=inner_center_x+letter_circle_radius*cos(letter_angle*(M_PI/180.0));
            double letter_center_y=inner_center_y+letter_circle_radius*sin(letter_angle*(M_PI/180.0));

            string font_color=current_color_theme()->window_font;

            if(character_chunk==selected_chunk){
                font_color="ui_white";

                render_circle(letter_center_x,letter_center_y,letter_radius,1.0,circle_color);
            }

            if(character<characters->size()){
                font->show(letter_center_x-8.0,letter_center_y-8.0,characters->at(character),font_color);
            }

            character++;
        }

        character_chunk++;
    }
}

void Engine_Interface::render(int render_rate,double ms_per_frame,int logic_frame_rate){
    if(!hide_gui){
        if(game.in_progress && game.paused){
            render_pause();
        }

        vector<int> closed_windows;
        for(int i=0;i<windows.size();i++){
            if(!is_window_open(&windows[i])){
                closed_windows.push_back(i);
            }
        }

        for(int i=0;i<closed_windows.size();i++){
            windows[closed_windows[i]].render();
        }

        for(int i=window_z_order.size()-1;i>=0;i--){
            window_z_order[i]->render();
        }

        render_gui_selector();

        console.render();
        chat.render();

        tooltip.render();

        if(toasts.size()>0){
            toasts[0].render();
        }

        if(mutable_info_selected() && gui_mode=="controller" && !allow_screen_keyboard()){
            if(!controller_text_entry_small){
                render_text_inputter();
            }
            else{
                render_small_text_inputter();
            }
        }

        if(touch_controls){
            touch_controller.render();
        }

        if(option_dev){
            render_dev_info();
        }

        if(option_fps){
            render_fps(render_rate,ms_per_frame,logic_frame_rate);
        }

        if(gui_mode=="mouse" && (cursor_render_always || is_any_window_open() || console.on)){
            int mouse_x=0;
            int mouse_y=0;
            get_mouse_state(&mouse_x,&mouse_y);

            if(!mouse_over){
                get_cursor(cursor)->render(mouse_x,mouse_y);
            }
            else{
                get_cursor(cursor_mouse_over)->render(mouse_x,mouse_y);
            }
        }
        else{
            if(option_hw_cursor){
                SDL_ShowCursor(SDL_DISABLE);
            }
        }
    }
    else{
        if(option_hw_cursor){
            SDL_ShowCursor(SDL_DISABLE);
        }
    }
}
