#include "engine_interface.h"
#include "world.h"
#include "render.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Engine_Interface::load_data_game(){
    ///load_data("example_game_script");
}

void Engine_Interface::load_data_script_game(string script,File_IO_Load* load){
    /**if(script=="example_game_script"){
        load_example_game_script(load);
    }*/
}

void Engine_Interface::unload_data_game(){
    ///example_game_scripts.clear();
}

/**void Engine_Interface::load_example_game_script(File_IO_Load* load){
    example_game_scripts.push_back(Example_Game_Script());

    bool multi_line_comment=false;

    while(!load->eof()){
        string line="";

        string str_name="name:";

        load->getline(&line);
        boost::algorithm::trim(line);

        if(boost::algorithm::contains(line,"*/
                                      /**")){
            multi_line_comment=false;
        }
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
        }

        //name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            line.erase(0,str_name.length());

            example_game_scripts[example_game_scripts.size()-1].name=line;
        }

        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</example_game_script>")){
            return;
        }
    }
}*/

/**Example_Game_Script* Engine_Interface::get_example_game_script(string name){
    Example_Game_Script* ptr_object=0;

    for(int i=0;i<example_game_scripts.size();i++){
        if(example_game_scripts[i].name==name){
            ptr_object=&example_game_scripts[i];

            break;
        }
    }

    if(ptr_object==0){
        message_log.add_error("Error accessing example game script '"+name+"'");
    }

    return ptr_object;
}*/

void Engine_Interface::open_window(Window* window){
    if(!is_window_open(window)){
        window_z_order.insert(window_z_order.begin(),window);

        reset_gui_selected_object();
    }
}

void Engine_Interface::close_window(Window* window){
    for(int i=0;i<window_z_order.size();i++){
        if(window_z_order[i]==window){
            window_z_order.erase(window_z_order.begin()+i);

            reset_gui_selected_object();

            break;
        }
    }
}

void Engine_Interface::render_title_background(){
    Bitmap_Font* font=get_font("small");

    render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,1.0,"ui_black");

    font->show(0,main_window.SCREEN_HEIGHT-font->spacing_y*2.0,"Version "+get_version()+"\nChecksum "+CHECKSUM,"ui_white");

    Image_Data* logo=image.get_image("logo");

    double logo_scale_x=(double)main_window.SCREEN_WIDTH/(double)1280.0;
    double logo_scale_y=(double)main_window.SCREEN_HEIGHT/(double)720.0;

    render_texture(main_window.SCREEN_WIDTH-logo->w*logo_scale_x,main_window.SCREEN_HEIGHT-logo->h*logo_scale_y,*logo,1.0,logo_scale_x,logo_scale_y);
}

void Engine_Interface::render_pause(){
    Bitmap_Font* font=get_font("standard");

    string msg="Paused";
    font->show((main_window.SCREEN_WIDTH-(font->spacing_x*msg.length()))/2,(main_window.SCREEN_HEIGHT-font->spacing_y)/2,msg,"ui_white");
}

void Engine_Interface::render_fps(int render_rate,double ms_per_frame,int logic_frame_rate){
    get_font("small")->show(2,2,"FPS: "+string_stuff.num_to_string(render_rate)+"\n"+network.get_stats(),"ui_white");
}

void Engine_Interface::render_loading_screen(double percentage,string load_message){
    Bitmap_Font* font=get_font("standard");

    SDL_SetRenderDrawColor(main_window.renderer,0,0,0,255);
    SDL_RenderClear(main_window.renderer);

    ///If images are loaded
    /**render_texture(0,0,image.loading_screen_main,0.25);
    render_texture((main_window.SCREEN_WIDTH-image.logo_hubert.w)/2.0,30,image.logo_hubert);*/

    render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,1.0,"ui_black");

    double bar_width=240.0*percentage;
    double max_bar_width=240.0*1.0;
    render_rectangle(main_window.SCREEN_WIDTH/2.0-120-2,main_window.SCREEN_HEIGHT-75-2,max_bar_width+4,30+4,1.0,"ui_3");
    render_rectangle(main_window.SCREEN_WIDTH/2.0-120,main_window.SCREEN_HEIGHT-75,bar_width,30,1.0,"ui_1");

    string msg=string_stuff.num_to_string((int)(percentage*100.0))+"%";

    font->show(main_window.SCREEN_WIDTH/2.0-120+(max_bar_width-msg.length()*font->spacing_x)/2.0,main_window.SCREEN_HEIGHT-75+font->spacing_y/4.0,msg,"ui_0");

    font->show((main_window.SCREEN_WIDTH-load_message.length()*font->spacing_x)/2.0,main_window.SCREEN_HEIGHT-75+font->spacing_y*2.0,load_message,"ui_0");

    SDL_RenderPresent(main_window.renderer);
}

string Engine_Interface::get_game_window_caption(){
    string msg="";

    return msg;
}
