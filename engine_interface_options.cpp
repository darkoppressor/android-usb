/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"
#include "version.h"

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/crc.hpp>

#ifdef GAME_OS_OSX
    #include <CoreServices/CoreServices.h>
#endif

using namespace std;

Version_Series::Version_Series(int get_major_1,int get_minor_1,int get_micro_1,int get_major_2,int get_minor_2,int get_micro_2){
    major_1=get_major_1;
    minor_1=get_minor_1;
    micro_1=get_micro_1;

    major_2=get_major_2;
    minor_2=get_minor_2;
    micro_2=get_micro_2;
}

string Engine_Interface::get_version(){
    return string_stuff.num_to_string(Version::MAJOR)+"."+string_stuff.num_to_string(Version::MINOR)+"."+string_stuff.num_to_string(Version::MICRO);
}

string Engine_Interface::get_build_date(){
    string year=Version::YEAR;
    string month=Version::MONTH;
    string day=Version::DAY;

    return year+"-"+month+"-"+day;
}

string Engine_Interface::get_checksum(){
    string checksum="";
    string checksum_data="";

    for(File_IO_Directory_Iterator it("data");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_path=it.get_full_path();

            File_IO_Load load(file_path);

            if(load.file_loaded()){
                checksum_data+=load.get_data();
            }
            else{
                message_log.add_error("Error loading file for checksum calculation: '"+file_path+"'");
            }
        }
    }

    if(checksum_data.length()>0){
        boost::crc_32_type result;
        result.process_bytes(checksum_data.data(),checksum_data.length());

        checksum=string_stuff.num_to_string((uint32_t)result.checksum());
    }

    return checksum;
}

string Engine_Interface::get_timestamp(bool include_date,bool include_time,bool filename){
    string str_start="[";
    string str_end="]";
    string date_separator="-";
    string time_separator=":";
    string section_separator=" ";
    if(filename){
        str_start="";
        str_end="";
        date_separator="-";
        time_separator=".";
        section_separator="_";
    }

    string date_string="%Y"+date_separator+"%m"+date_separator+"%d";
    string time_string="%H"+time_separator+"%M"+time_separator+"%S";

    string time_stamp=str_start;
    if(!include_date && include_time){
        time_stamp+=time_string;
    }
    else if(include_date && !include_time){
        time_stamp+=date_string;
    }
    else{
        time_stamp+=date_string+section_separator+time_string;
    }
    time_stamp+=str_end;

    char buff[BUFSIZ];
    time_t now=time(NULL);
    struct tm* tm_now=localtime(&now);
    strftime(buff,sizeof buff,time_stamp.c_str(),tm_now);

    return buff;
}

string Engine_Interface::get_cwd(){
    string cwd="./";

    char* base_path=SDL_GetBasePath();

    if(base_path!=0){
        cwd=SDL_strdup(base_path);

        SDL_free(base_path);
    }
    else{
        string msg="Error getting base path: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    correct_slashes(&cwd);

    return cwd;
}

string Engine_Interface::get_save_path(){
    string save_path=get_home_directory();

    if(save_path.length()==0){
        save_path=CURRENT_WORKING_DIRECTORY;
    }

    return save_path;
}

void Engine_Interface::correct_slashes(string* str_input){
    boost::algorithm::replace_all(*str_input,"\\","/");
}

string Engine_Interface::get_home_directory(){
    string str_home="./";

    if(option_save_location=="home"){
        string game_name=home_directory;
        #ifdef GAME_DEMO
            game_name+="-demo";
        #endif

        ///This creates a string pref_path to the location SDL thinks data should be saved for this platform.
        /**string pref_path="./";
        char* base_path=SDL_GetPrefPath("cheese-and-bacon-games",game_name.c_str());
        if(base_path!=0){
            pref_path=SDL_strdup(base_path);

            SDL_free(base_path);
        }
        else{
            string msg="Error getting pref path: ";
            msg+=SDL_GetError();
            message_log.add_error(msg);
        }
        correct_slashes(&pref_path);*/

        #ifdef GAME_OS_WINDOWS
            str_home=getenv("USERPROFILE");
            str_home+="/My Documents/My Games/";
            str_home+=game_name;
            str_home+="/";
        #endif

        #ifdef GAME_OS_LINUX
            str_home=getenv("HOME");
            str_home+="/.";
            str_home+=game_name;
            str_home+="/";
        #endif

        #ifdef GAME_OS_OSX
            FSRef fsref;
            OSType folder_type=kApplicationSupportFolderType;
            char path[PATH_MAX];
            FSFindFolder(kUserDomain,folder_type,kCreateFolder,&fsref);
            FSRefMakePath(&fsref,(uint8_t*)&path,PATH_MAX);

            str_home=path;
            str_home+="/";
            str_home+=game_name;
            str_home+="/";
        #endif

        #ifdef GAME_OS_ANDROID
            bool using_external_storage=false;

            if(file_io.external_storage_available()){
                const char* external_path=SDL_AndroidGetExternalStoragePath();

                if(external_path!=0){
                    str_home=external_path;
                    str_home+="/";

                    using_external_storage=true;
                }
                else{
                    string msg="Error getting external storage path: ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);
                }
            }

            /**if(!using_external_storage){
                const char* internal_path=SDL_AndroidGetInternalStoragePath();

                if(internal_path!=0){
                    str_home=internal_path;
                    str_home+="/";
                }
                else{
                    string msg="Error getting internal storage path: ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);
                }
            }*/
        #endif

        correct_slashes(&str_home);
    }

    return str_home;
}

void Engine_Interface::make_home_directory(){
    if(option_save_location=="home"){
        #ifdef GAME_OS_WINDOWS
            string str_my_games=getenv("USERPROFILE");
            str_my_games+="/My Documents/My Games";

            correct_slashes(&str_my_games);

            file_io.create_directory(str_my_games);
        #endif

        string str_home=get_home_directory();

        //Remove the ending slash.
        str_home.erase(str_home.length()-1,1);

        correct_slashes(&str_home);

        file_io.create_directory(str_home);
    }
}

void Engine_Interface::make_directories(){
    make_home_directory();

    file_io.create_directory(get_home_directory()+"screenshots");

    file_io.create_directory(get_home_directory()+"files");
}

bool Engine_Interface::save_save_location(){
    stringstream save("");

    save<<"<save_location>\n";

    save<<"\tsave_location:"<<option_save_location<<"\n";

    save<<"</save_location>\n";

    if(file_io.save_file("save_location.cfg",save.str())){
        save_location_loaded=true;

        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_save_location(){
    File_IO_Load load("save_location.cfg");

    if(load.file_loaded()){
        bool multi_line_comment=false;

        //As long as we haven't reached the end of the file.
        while(!load.eof()){
            string line="";

            //The option strings used in the file.

            string str_save_location="save_location:";

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

            //Load data based on the line.

            //save_location
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_save_location)){
                //Clear the data name.
                line.erase(0,str_save_location.length());

                option_save_location=line;

                save_location_loaded=true;
            }

            //If the line ends the save location data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</save_location>")){
                break;
            }
        }
    }
    else{
        if(!save_save_location()){
            return false;
        }
    }

    return true;
}

int Engine_Interface::version_compare(int major_1,int minor_1,int micro_1,int major_2,int minor_2,int micro_2){
    if(major_1==major_2){
        if(minor_1==minor_2){
            if(micro_1==micro_2){
                return 0;
            }
            else if(micro_1<micro_2){
                return -1;
            }
            else if(micro_1>micro_2){
                return 1;
            }
        }
        else if(minor_1<minor_2){
            return -1;
        }
        else if(minor_1>minor_2){
            return 1;
        }
    }
    else if(major_1<major_2){
        return -1;
    }
    else if(major_1>major_2){
        return 1;
    }
}

int Engine_Interface::which_version_series(vector<Version_Series>* version_series,int major,int minor,int micro){
    for(int i=0;i<version_series->size();i++){
        if(version_compare(major,minor,micro,version_series->at(i).major_1,version_series->at(i).minor_1,version_series->at(i).micro_1)>=0 &&
           version_compare(major,minor,micro,version_series->at(i).major_2,version_series->at(i).minor_2,version_series->at(i).micro_2)<=0){
            return i;
        }
    }

    return -1;
}

bool Engine_Interface::version_compatible(string name_to_check){
    int current_major=Version::MAJOR;
    int current_minor=Version::MINOR;
    int current_micro=Version::MICRO;

    int major=0;
    int minor=0;
    int micro=0;

    vector<string> version_strings;
    boost::algorithm::split(version_strings,option_version,boost::algorithm::is_any_of("."));

    major=string_stuff.string_to_long(version_strings[0]);
    minor=string_stuff.string_to_long(version_strings[1]);
    micro=string_stuff.string_to_long(version_strings[2]);

    //Version series are defined by a start version and an end version.
    //The start version must be less than or equal to the end version.
    vector<Version_Series> version_series;

    version_series.push_back(Version_Series(0,0,0,0,0,1));

    if(which_version_series(&version_series,major,minor,micro)!=which_version_series(&version_series,current_major,current_minor,current_micro)){
        string error_message="Version incompatibility! Save data was created with version "+option_version;
        error_message+=". Current version is "+get_version()+".";

        message_log.add_error(error_message);

        return false;
    }

    return true;
}

string Engine_Interface::get_option_value(string option){
    if(option=="cl_logic_update_rate"){
        return string_stuff.num_to_string(UPDATE_RATE);
    }
    else if(option=="cl_frame_rate_max"){
        return string_stuff.num_to_string(UPDATE_RATE_RENDER);
    }

    else if(option=="cl_screen_width"){
        return string_stuff.num_to_string(option_screen_width);
    }
    else if(option=="cl_screen_height"){
        return string_stuff.num_to_string(option_screen_height);
    }
    else if(option=="cl_fullscreen_state"){
        return string_stuff.bool_to_string(option_fullscreen);
    }
    else if(option=="cl_fullscreen_mode"){
        return option_fullscreen_mode;
    }

    else if(option=="cl_vsync"){
        return string_stuff.bool_to_string(option_vsync);
    }
    else if(option=="cl_accelerometer_controller"){
        return string_stuff.bool_to_string(option_accelerometer_controller);
    }
    else if(option=="cl_touch_controller"){
        return string_stuff.bool_to_string(option_touch_controller);
    }
    else if(option=="cl_touch_controller_opacity"){
        return string_stuff.num_to_string(option_touch_controller_opacity);
    }
    else if(option=="cl_font_shadows"){
        return string_stuff.bool_to_string(option_font_shadows);
    }
    else if(option=="cl_screen_keyboard"){
        return string_stuff.bool_to_string(option_screen_keyboard);
    }
    else if(option=="cl_hw_cursor"){
        return string_stuff.bool_to_string(option_hw_cursor);
    }
    else if(option=="cl_bind_cursor"){
        return string_stuff.bool_to_string(option_bind_cursor);
    }

    else if(option=="cl_fps"){
        return string_stuff.bool_to_string(option_fps);
    }
    else if(option=="cl_dev"){
        return string_stuff.bool_to_string(option_dev);
    }
    else if(option=="cl_volume_global"){
        return string_stuff.num_to_string(option_volume_global);
    }
    else if(option=="cl_volume_sound"){
        return string_stuff.num_to_string(option_volume_sound);
    }
    else if(option=="cl_volume_music"){
        return string_stuff.num_to_string(option_volume_music);
    }
    else if(option=="cl_mute_global"){
        return string_stuff.bool_to_string(option_mute_global);
    }
    else if(option=="cl_mute_sound"){
        return string_stuff.bool_to_string(option_mute_sound);
    }
    else if(option=="cl_mute_music"){
        return string_stuff.bool_to_string(option_mute_music);
    }

    for(int i=0;i<game_options.size();i++){
        if(option==game_options[i].name){
            return game_options[i].get_value();
        }
    }

    return "";
}

string Engine_Interface::get_option_description(string option){
    if(option=="cl_logic_update_rate"){
        return "The game logic update rate.\n - Changes to this option won't persist through a game restart.\n - This option cannot be changed during networked play.";
    }
    else if(option=="cl_frame_rate_max"){
        return "The maximum frame rate.\n - Changes to this option won't persist through a game restart.";
    }

    else if(option=="cl_screen_width"){
        return "The width of the screen in pixels.\n - Changes to this option are only applied when the UI is reloaded with \"reload\".";
    }
    else if(option=="cl_screen_height"){
        return "The height of the screen in pixels.\n - Changes to this option are only applied when the UI is reloaded with \"reload\".";
    }
    else if(option=="cl_fullscreen_state"){
        return "Enable or disable the fullscreen status of the window.\n - Changes to this option are only applied when the UI is reloaded with \"reload\".";
    }
    else if(option=="cl_fullscreen_mode"){
        return "The mode used when the window is made fullscreen.\n - Changes to this option are only applied when the UI is reloaded with \"reload\".\n - Valid values: \"standard\", \"windowed\"";
    }

    else if(option=="cl_vsync"){
        return "Enable vertical synchronization.\n - Changes to this option are only applied when the game is restarted.";
    }
    else if(option=="cl_accelerometer_controller"){
        return "Allow the Android accelerometer to be used as a game controller.\n - Changes to this option are only applied when the game is restarted.";
    }
    else if(option=="cl_touch_controller"){
        return "Enable the virtual touchscreen controller.";
    }
    else if(option=="cl_touch_controller_opacity"){
        return "The opacity of the touchscreen controller.\n - Valid values: 0.0 - 1.0";
    }
    else if(option=="cl_font_shadows"){
        return "Enable font shadows.";
    }
    else if(option=="cl_screen_keyboard"){
        return "Enable the screen keyboard.";
    }
    else if(option=="cl_hw_cursor"){
        return "Enable hardware cursor.";
    }
    else if(option=="cl_bind_cursor"){
        return "Bind the cursor to the window.";
    }

    else if(option=="cl_fps"){
        return "Show FPS in-game and in the window's titlebar.";
    }
    else if(option=="cl_dev"){
        return "Enable developer mode.";
    }
    else if(option=="cl_volume_global"){
        return "The global volume modifier.\n - Valid values: 0.0 - 1.0";
    }
    else if(option=="cl_volume_sound"){
        return "The volume modifier for sound effects.\n - Valid values: 0.0 - 1.0";
    }
    else if(option=="cl_volume_music"){
        return "The volume modifier for music.\n - Valid values: 0.0 - 1.0";
    }
    else if(option=="cl_mute_global"){
        return "Mute all sound.";
    }
    else if(option=="cl_mute_sound"){
        return "Mute sound effects.";
    }
    else if(option=="cl_mute_music"){
        return "Mute music.";
    }

    for(int i=0;i<game_options.size();i++){
        if(option==game_options[i].name){
            return game_options[i].description;
        }
    }
}

void Engine_Interface::change_option(string option,string new_value){
    if(option=="cl_logic_update_rate"){
        if(network.status=="off"){
            set_logic_update_rate(string_stuff.string_to_double(new_value));
        }
    }
    else if(option=="cl_frame_rate_max"){
        set_render_update_rate(string_stuff.string_to_double(new_value));
    }

    else if(option=="cl_screen_width"){
        option_screen_width=string_stuff.string_to_long(new_value);
    }
    else if(option=="cl_screen_height"){
        option_screen_height=string_stuff.string_to_long(new_value);
    }
    else if(option=="cl_fullscreen_state"){
        option_fullscreen=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_fullscreen_mode"){
        option_fullscreen_mode=new_value;
    }

    else if(option=="cl_vsync"){
        option_vsync=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_accelerometer_controller"){
        option_accelerometer_controller=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_touch_controller"){
        option_touch_controller=string_stuff.string_to_bool(new_value);

        if(option_touch_controller && SDL_GetNumTouchDevices()>0){
            touch_controls=true;
        }
    }
    else if(option=="cl_touch_controller_opacity"){
        option_touch_controller_opacity=string_stuff.string_to_double(new_value);
    }
    else if(option=="cl_font_shadows"){
        option_font_shadows=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_screen_keyboard"){
        option_screen_keyboard=string_stuff.string_to_bool(new_value);

        if(!option_screen_keyboard && SDL_IsScreenKeyboardShown(main_window.screen)){
            SDL_StopTextInput();
        }
    }
    else if(option=="cl_hw_cursor"){
        option_hw_cursor=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_bind_cursor"){
        option_bind_cursor=string_stuff.string_to_bool(new_value);

        if(option_bind_cursor){
            SDL_SetWindowGrab(main_window.screen,SDL_TRUE);
        }
        else{
            SDL_SetWindowGrab(main_window.screen,SDL_FALSE);
        }
    }

    else if(option=="cl_fps"){
        option_fps=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_dev"){
        option_dev=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_volume_global"){
        option_volume_global=string_stuff.string_to_double(new_value);

        music.set_track_volumes();
    }
    else if(option=="cl_volume_sound"){
        option_volume_sound=string_stuff.string_to_double(new_value);
    }
    else if(option=="cl_volume_music"){
        option_volume_music=string_stuff.string_to_double(new_value);

        music.set_track_volumes();
    }
    else if(option=="cl_mute_global"){
        option_mute_global=string_stuff.string_to_bool(new_value);

        music.set_track_volumes();
    }
    else if(option=="cl_mute_sound"){
        option_mute_sound=string_stuff.string_to_bool(new_value);
    }
    else if(option=="cl_mute_music"){
        option_mute_music=string_stuff.string_to_bool(new_value);

        music.set_track_volumes();
    }

    else{
        bool game_option=false;

        for(int i=0;i<game_options.size();i++){
            if(option==game_options[i].name){
                game_options[i].set_value(new_value);

                game_option=true;

                break;
            }
        }

        if(!game_option){
            return;
        }
    }

    save_options();
}

bool Engine_Interface::save_options(){
    stringstream save("");

    save<<"<options>\n";

    save<<"\tversion:"<<option_version<<"\n";

    save<<"\n";

    save<<"\tscreen_width:"<<option_screen_width<<"\n";
    save<<"\tscreen_height:"<<option_screen_height<<"\n";
    save<<"\tfullscreen_state:"<<string_stuff.bool_to_string(option_fullscreen)<<"\n";
    save<<"\tfullscreen_mode:"<<option_fullscreen_mode<<"\n";

    save<<"\n";

    save<<"\tvsync:"<<string_stuff.bool_to_string(option_vsync)<<"\n";
    save<<"\taccelerometer_controller:"<<string_stuff.bool_to_string(option_accelerometer_controller)<<"\n";
    save<<"\ttouch_controller:"<<string_stuff.bool_to_string(option_touch_controller)<<"\n";
    save<<"\ttouch_controller_opacity:"<<string_stuff.num_to_string(option_touch_controller_opacity)<<"\n";
    save<<"\tfont_shadows:"<<string_stuff.bool_to_string(option_font_shadows)<<"\n";
    save<<"\tscreen_keyboard:"<<string_stuff.bool_to_string(option_screen_keyboard)<<"\n";
    save<<"\thw_cursor:"<<string_stuff.bool_to_string(option_hw_cursor)<<"\n";
    save<<"\tbind_cursor:"<<string_stuff.bool_to_string(option_bind_cursor)<<"\n";

    save<<"\n";

    save<<"\tfps:"<<string_stuff.bool_to_string(option_fps)<<"\n";
    save<<"\tdev:"<<string_stuff.bool_to_string(option_dev)<<"\n";
    save<<"\tvolume_global:"<<option_volume_global<<"\n";
    save<<"\tvolume_sound:"<<option_volume_sound<<"\n";
    save<<"\tvolume_music:"<<option_volume_music<<"\n";
    save<<"\tmute_global:"<<string_stuff.bool_to_string(option_mute_global)<<"\n";
    save<<"\tmute_sound:"<<string_stuff.bool_to_string(option_mute_sound)<<"\n";
    save<<"\tmute_music:"<<string_stuff.bool_to_string(option_mute_music)<<"\n";

    save<<"\n";

    for(int i=0;i<game_options.size();i++){
        save<<"\t"<<game_options[i].name<<":"<<game_options[i].get_value()<<"\n";
    }

    save<<"</options>\n";

    if(file_io.save_file(get_home_directory()+"options.cfg",save.str())){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_options(){
    File_IO_Load load(get_home_directory()+"options.cfg");

    if(load.file_loaded()){
        bool multi_line_comment=false;

        //As long as we haven't reached the end of the file.
        while(!load.eof()){
            string line="";

            //The option strings used in the file.

            string str_version="version:";
            string str_screen_width="screen_width:";
            string str_screen_height="screen_height:";
            string str_fullscreen="fullscreen_state:";
            string str_fullscreen_mode="fullscreen_mode:";

            string str_vsync="vsync:";
            string str_accelerometer_controller="accelerometer_controller:";
            string str_touch_controller="touch_controller:";
            string str_touch_controller_opacity="touch_controller_opacity:";
            string str_font_shadows="font_shadows:";
            string str_screen_keyboard="screen_keyboard:";
            string str_hw_cursor="hw_cursor:";
            string str_bind_cursor="bind_cursor:";

            string str_fps="fps:";
            string str_dev="dev:";
            string str_volume_global="volume_global:";
            string str_volume_sound="volume_sound:";
            string str_volume_music="volume_music:";
            string str_mute_global="mute_global:";
            string str_mute_sound="mute_sound:";
            string str_mute_music="mute_music:";

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

            //Load data based on the line.

            //version
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_version)){
                //Clear the data name.
                line.erase(0,str_version.length());

                option_version=line;
            }
            //screen_width
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_width)){
                //Clear the data name.
                line.erase(0,str_screen_width.length());

                option_screen_width=string_stuff.string_to_long(line);
            }
            //screen_height
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_height)){
                //Clear the data name.
                line.erase(0,str_screen_height.length());

                option_screen_height=string_stuff.string_to_long(line);
            }
            //fullscreen
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fullscreen)){
                //Clear the data name.
                line.erase(0,str_fullscreen.length());

                option_fullscreen=string_stuff.string_to_bool(line);
            }
            //fullscreen_mode
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fullscreen_mode)){
                //Clear the data name.
                line.erase(0,str_fullscreen_mode.length());

                option_fullscreen_mode=line;
            }

            //vsync
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_vsync)){
                //Clear the data name.
                line.erase(0,str_vsync.length());

                option_vsync=string_stuff.string_to_bool(line);
            }
            //accelerometer_controller
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_accelerometer_controller)){
                //Clear the data name.
                line.erase(0,str_accelerometer_controller.length());

                option_accelerometer_controller=string_stuff.string_to_bool(line);
            }
            //touch_controller
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller)){
                //Clear the data name.
                line.erase(0,str_touch_controller.length());

                option_touch_controller=string_stuff.string_to_bool(line);
            }
            //touch_controller_opacity
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_opacity)){
                //Clear the data name.
                line.erase(0,str_touch_controller_opacity.length());

                option_touch_controller_opacity=string_stuff.string_to_double(line);
            }
            //font_shadows
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_shadows)){
                //Clear the data name.
                line.erase(0,str_font_shadows.length());

                option_font_shadows=string_stuff.string_to_bool(line);
            }
            //screen_keyboard
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_keyboard)){
                //Clear the data name.
                line.erase(0,str_screen_keyboard.length());

                option_screen_keyboard=string_stuff.string_to_bool(line);
            }
            //hw_cursor
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_hw_cursor)){
                //Clear the data name.
                line.erase(0,str_hw_cursor.length());

                option_hw_cursor=string_stuff.string_to_bool(line);
            }
            //bind_cursor
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_bind_cursor)){
                //Clear the data name.
                line.erase(0,str_bind_cursor.length());

                option_bind_cursor=string_stuff.string_to_bool(line);
            }

            //fps
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fps)){
                //Clear the data name.
                line.erase(0,str_fps.length());

                option_fps=string_stuff.string_to_bool(line);
            }
            //dev
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_dev)){
                //Clear the data name.
                line.erase(0,str_dev.length());

                option_dev=string_stuff.string_to_bool(line);
            }
            //volume_global
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_global)){
                //Clear the data name.
                line.erase(0,str_volume_global.length());

                option_volume_global=string_stuff.string_to_double(line);
            }
            //volume_sound
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_sound)){
                //Clear the data name.
                line.erase(0,str_volume_sound.length());

                option_volume_sound=string_stuff.string_to_double(line);
            }
            //volume_music
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_music)){
                //Clear the data name.
                line.erase(0,str_volume_music.length());

                option_volume_music=string_stuff.string_to_double(line);
            }
            //mute_global
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_global)){
                //Clear the data name.
                line.erase(0,str_mute_global.length());

                option_mute_global=string_stuff.string_to_bool(line);
            }
            //mute_sound
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_sound)){
                //Clear the data name.
                line.erase(0,str_mute_sound.length());

                option_mute_sound=string_stuff.string_to_bool(line);
            }
            //mute_music
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_music)){
                //Clear the data name.
                line.erase(0,str_mute_music.length());

                option_mute_music=string_stuff.string_to_bool(line);
            }

            //If the line ends the options data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</options>")){
                break;
            }

            //Game Option
            else if(!multi_line_comment){
                for(int i=0;i<game_options.size();i++){
                    string str_game_option=game_options[i].name+":";

                    if(boost::algorithm::starts_with(line,str_game_option)){
                        //Clear the data name.
                        line.erase(0,str_game_option.length());

                        game_options[i].set_value(line);
                    }
                }
            }
        }
    }

    if(!save_options()){
        return false;
    }

    return true;
}

bool Engine_Interface::save_game_commands(){
    stringstream save("");

    save<<"<game_commands>\n";

    for(int i=0;i<game_commands.size();i++){
        const char* ckey=SDL_GetScancodeName(game_commands[i].key);
        const char* cbutton=SDL_GameControllerGetStringForButton(game_commands[i].button);
        const char* caxis=SDL_GameControllerGetStringForAxis(game_commands[i].axis);

        string key="";
        if(ckey!=0){
            key=ckey;
        }

        string button="";
        if(cbutton!=0){
            button=cbutton;
        }

        string axis="";
        if(caxis!=0){
            axis=caxis;
        }

        save<<"\t"<<game_commands[i].name<<","<<game_commands[i].title<<","<<string_stuff.add_newlines(game_commands[i].description)<<","<<string_stuff.bool_to_string(game_commands[i].dev)<<","<<key<<","<<button<<","<<axis<<"\n";
    }

    save<<"</game_commands>\n";

    if(file_io.save_file(get_home_directory()+"game_commands.cfg",save.str())){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_game_commands(){
    File_IO_Load load(get_home_directory()+"game_commands.cfg");

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

            //Load data based on the line.

            else if(!multi_line_comment && !boost::algorithm::starts_with(line,"</game_commands>")){
                vector<string> command_parts;
                boost::algorithm::split(command_parts,line,boost::algorithm::is_any_of(","));

                if(command_parts.size()==7){
                    Game_Command* game_command=0;

                    string name=command_parts[0];

                    for(int i=0;i<game_commands.size();i++){
                        if(name==game_commands[i].name){
                            game_command=&game_commands[i];

                            break;
                        }

                        if(i==game_commands.size()-1){
                            game_commands.push_back(Game_Command());
                            game_command=&game_commands[game_commands.size()-1];

                            break;
                        }
                    }

                    if(game_command!=0){
                        SDL_Scancode key=SDL_SCANCODE_UNKNOWN;
                        if(command_parts[4].length()>0){
                            key=SDL_GetScancodeFromName(command_parts[4].c_str());
                        }

                        SDL_GameControllerButton button=SDL_CONTROLLER_BUTTON_INVALID;
                        if(command_parts[5].length()>0){
                            button=SDL_GameControllerGetButtonFromString(command_parts[5].c_str());
                        }

                        SDL_GameControllerAxis axis=SDL_CONTROLLER_AXIS_INVALID;
                        if(command_parts[6].length()>0){
                            axis=SDL_GameControllerGetAxisFromString(command_parts[6].c_str());
                        }

                        game_command->name=name;
                        game_command->title=command_parts[1];
                        game_command->description=string_stuff.process_newlines(command_parts[2]);
                        game_command->dev=string_stuff.string_to_bool(command_parts[3]);
                        game_command->key=key;
                        game_command->button=button;
                        game_command->axis=axis;
                    }
                }
            }

            //If the line ends the game commands data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</game_commands>")){
                break;
            }
        }
    }

    if(!save_game_commands()){
        return false;
    }

    return true;
}

bool Engine_Interface::save_servers(){
    stringstream save("");

    save<<"<servers>\n";

    for(int i=0;i<network.server_list.size();i++){
        save<<"\t"<<network.server_list[i].name<<","<<network.server_list[i].address<<","<<network.server_list[i].port<<","<<network.server_list[i].password<<"\n";
    }

    save<<"</servers>\n";

    if(file_io.save_file(get_home_directory()+"servers.txt",save.str())){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_servers(){
    File_IO_Load load(get_home_directory()+"servers.txt");

    if(load.file_loaded()){
        bool multi_line_comment=false;

        network.server_list.clear();

        while(!load.eof()){
            string line="";

            load.getline(&line);

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

            else if(!multi_line_comment && !boost::algorithm::starts_with(line,"</servers>")){
                vector<string> command_parts;
                boost::algorithm::split(command_parts,line,boost::algorithm::is_any_of(","));

                if(command_parts.size()==4){
                    network.server_list.push_back(Server());
                    network.server_list[network.server_list.size()-1].name=command_parts[0];
                    network.server_list[network.server_list.size()-1].address=command_parts[1];
                    network.server_list[network.server_list.size()-1].port=string_stuff.string_to_unsigned_long(command_parts[2]);
                    network.server_list[network.server_list.size()-1].password=command_parts[3];
                }
            }

            //If the line ends the data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</servers>")){
                break;
            }
        }
    }
    else{
        return false;
    }

    return true;
}
