/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"
#include "render.h"

using namespace std;

void Engine_Interface::render_dev_info(){
    string msg="";

    if(game.in_progress){
        /**msg+="Camera Position: "+string_stuff.num_to_string(game.camera.x*game.camera_zoom)+","+string_stuff.num_to_string(game.camera.y*game.camera_zoom)+"\n";
        msg+="Camera Size: "+string_stuff.num_to_string(game.camera.w/game.camera_zoom)+","+string_stuff.num_to_string(game.camera.h/game.camera_zoom)+"\n";
        msg+="Camera Zoom: "+string_stuff.num_to_string(game.camera_zoom)+"\n";*/
    }

    if(msg.length()>0){
        Bitmap_Font* font=engine_interface.get_font("standard");

        render_rectangle(2.0,2.0,string_stuff.longest_line(msg)*font->spacing_x,string_stuff.newline_count(msg)*font->spacing_y,0.75,"ui_black");
        font->show(2.0,2.0,msg,"red");
    }
}
