#include "game.h"
#include "world.h"
#include "render.h"

using namespace std;

Game::Game(){
    option_effect_limit=0;
    option_camera_speed=0.0;
    option_camera_zoom=0.0;
    option_name="";
    option_chat_timestamps=false;
    option_max_players=0;
    option_screen_shake=false;
    option_adb_path="";
    option_starting_path="";
    option_hidden_files=false;

    display_scoreboard=false;

    in_progress=false;

    reset();
}

void Game::reset(){
    paused=false;

    cam_state="none";

    command_states.clear();

    camera.x=0.0;
    camera.y=0.0;

    reset_camera_dimensions();

    camera_delta_x=0.0;
    camera_delta_y=0.0;

    camera_speed=option_camera_speed;
    camera_zoom=option_camera_zoom;

    current_music="";
}

void Game::reset_camera_dimensions(){
    camera.w=main_window.SCREEN_WIDTH;
    camera.h=main_window.SCREEN_HEIGHT;
}

string Game::get_random_direction_cardinal(){
    int random=rng.random_range(0,3);
    if(random==0){
        return "left";
    }
    else if(random==1){
        return "up";
    }
    else if(random==2){
        return "right";
    }
    else if(random==3){
        return "down";
    }
}

string Game::get_random_direction_cardinal_ordinal(){
    int random=rng.random_range(0,7);
    if(random==0){
        return "left";
    }
    else if(random==1){
        return "up";
    }
    else if(random==2){
        return "right";
    }
    else if(random==3){
        return "down";
    }
    else if(random==4){
        return "left_up";
    }
    else if(random==5){
        return "right_up";
    }
    else if(random==6){
        return "left_down";
    }
    else if(random==7){
        return "right_down";
    }
}

bool Game::effect_allowed(){
    uint32_t effects=/**world.effects_example.size()*/0;

    if(effects<option_effect_limit){
        return true;
    }
    else{
        return false;
    }
}

void Game::manage_music(){
    string music_to_play="";

    /**if(in_progress){
        music_to_play="1";
    }
    else{
        music_to_play="";
    }*/

    if(music_to_play.length()>0){
        if(music_to_play!=current_music){
            music.restart_track(music_to_play);
        }

        music.play_track(music_to_play);
    }
    else{
        music.stop_track();
    }

    current_music=music_to_play;
}

void Game::toggle_pause(){
    paused=!paused;

    sound_system.play_sound("pause");
}

void Game::start(){
    if(!in_progress){
        in_progress=true;

        reset();

        world.generate_world();
    }
}

void Game::start_client(){
    if(!in_progress){
        in_progress=true;

        reset();

        world.clear_world();
    }
}

void Game::stop(){
    network.stop();

    if(in_progress){
        in_progress=false;

        reset();

        world.clear_world();

        if(engine_interface.chat.on){
            engine_interface.chat.toggle_on();
        }
    }
}

void Game::center_camera(Collision_Rect box){
    camera.x=box.center_x()*camera_zoom-camera.w/2.0;
    camera.y=box.center_y()*camera_zoom-camera.h/2.0;
}

void Game::center_camera(Collision_Circ circle){
    camera.x=circle.x*camera_zoom-camera.w/2.0;
    camera.y=circle.y*camera_zoom-camera.h/2.0;
}

void Game::zoom_camera_in(Collision_Rect box){
    if(camera_zoom<ZOOM_MAX){
        Collision_Rect camera_location=box/camera_zoom;

        camera_zoom+=camera_zoom*ZOOM_RATE;
        if(camera_zoom>ZOOM_MAX){
            camera_zoom=ZOOM_MAX;
        }

        center_camera(camera_location);
    }
}

void Game::zoom_camera_in(Collision_Circ circle){
    if(camera_zoom<ZOOM_MAX){
        Collision_Circ camera_location=circle/camera_zoom;

        camera_zoom+=camera_zoom*ZOOM_RATE;
        if(camera_zoom>ZOOM_MAX){
            camera_zoom=ZOOM_MAX;
        }

        center_camera(camera_location);
    }
}

void Game::zoom_camera_out(Collision_Rect box){
    if(camera_zoom>ZOOM_MIN){
        Collision_Rect camera_location=box/camera_zoom;

        camera_zoom-=camera_zoom*ZOOM_RATE;
        if(camera_zoom<ZOOM_MIN){
            camera_zoom=ZOOM_MIN;
        }

        center_camera(camera_location);
    }
}

void Game::zoom_camera_out(Collision_Circ circle){
    if(camera_zoom>ZOOM_MIN){
        Collision_Circ camera_location=circle/camera_zoom;

        camera_zoom-=camera_zoom*ZOOM_RATE;
        if(camera_zoom<ZOOM_MIN){
            camera_zoom=ZOOM_MIN;
        }

        center_camera(camera_location);
    }
}

void Game::set_camera(){
    camera_delta_x=camera.x;
    camera_delta_y=camera.y;

    if(screen_shake.is_active()){
        camera.x-=(int64_t)screen_shake.x;
        camera.y-=(int64_t)screen_shake.y;

        screen_shake.movement();
    }

    if(false/**Something to follow*/){
    }
    else{
        if(cam_state=="left"){
            camera.x-=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="up"){
            camera.y-=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="right"){
            camera.x+=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="down"){
            camera.y+=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="left_up"){
            camera.x-=camera_speed/UPDATE_RATE;
            camera.y-=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="right_up"){
            camera.x+=camera_speed/UPDATE_RATE;
            camera.y-=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="right_down"){
            camera.x+=camera_speed/UPDATE_RATE;
            camera.y+=camera_speed/UPDATE_RATE;
        }
        else if(cam_state=="left_down"){
            camera.x-=camera_speed/UPDATE_RATE;
            camera.y+=camera_speed/UPDATE_RATE;
        }
    }

    //If the camera goes past the bounds of the world, set it back in the bounds.
    if(camera.x<0.0){
        camera.x=0.0;
    }
    if(camera.x+camera.w>0/**World size x*/*camera_zoom){
        camera.x=0/**World size x*/*camera_zoom-camera.w;
    }
    if(camera.y<0.0){
        camera.y=0.0;
    }
    if(camera.y+camera.h>0/**World size y*/*camera_zoom){
        camera.y=0/**World size y*/*camera_zoom-camera.h;
    }

    if(screen_shake.is_active()){
        camera.x+=(int64_t)screen_shake.x;
        camera.y+=(int64_t)screen_shake.y;
    }

    camera_delta_x=camera.x-camera_delta_x;
    camera_delta_y=camera.y-camera_delta_y;
}

void Game::render_scoreboard(){
    if(display_scoreboard){
        Bitmap_Font* font=engine_interface.get_font("small");

        string name_list=network.get_name_list();
        string ping_list=network.get_ping_list();

        render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,0.5,"ui_black");

        font->show(72,(main_window.SCREEN_HEIGHT-(string_stuff.newline_count(name_list)+1)*font->spacing_y)/2.0,name_list,"ui_white");
        font->show(168,(main_window.SCREEN_HEIGHT-(string_stuff.newline_count(ping_list)+1)*font->spacing_y)/2.0,ping_list,"ui_white");
    }
}
