/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "tooltip.h"
#include "world.h"
#include "render.h"

using namespace std;

Tooltip::Tooltip(){
    on=false;

    message="";

    font="";

    x=0;
    y=0;
    w=0;
    h=0;
}

void Tooltip::setup(string get_message,int mouse_x,int mouse_y){
    message=get_message;
    on=true;
    x=mouse_x;
    y=mouse_y+engine_interface.get_font(font)->get_letter_height();
    set_dimensions();

    //If the tooltip would be displayed off the screen, move it.

    if(x+w>main_window.SCREEN_WIDTH){
        x=mouse_x-w;
    }

    if(y+h>main_window.SCREEN_HEIGHT){
        y=mouse_y-engine_interface.get_font(font)->get_letter_height()-h;
    }
}

void Tooltip::set_dimensions(){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    w=engine_interface.gui_border_thickness*2.0+string_stuff.longest_line(message)*ptr_font->spacing_x+ptr_font->gui_padding_x;
    h=engine_interface.gui_border_thickness*2.0+(string_stuff.newline_count(message)+1)*ptr_font->spacing_y+ptr_font->gui_padding_y;
}

void Tooltip::render(){
    //As long as the tooltip is on and has content.
    if(on && message.length()>0){
        Bitmap_Font* ptr_font=engine_interface.get_font(font);

        //Render the border.
        if(engine_interface.current_color_theme()->tooltip_border!="<INVISIBLE>"){
            render_rectangle(x,y,w,h,1.0,engine_interface.current_color_theme()->tooltip_border);
        }

        //Render the background.
        if(engine_interface.current_color_theme()->tooltip_background!="<INVISIBLE>"){
            render_rectangle(x+engine_interface.gui_border_thickness,y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,1.0,engine_interface.current_color_theme()->tooltip_background);
        }

        //Display the message text.
        if(engine_interface.current_color_theme()->tooltip_font!="<INVISIBLE>"){
            ptr_font->show(x+engine_interface.gui_border_thickness,y+engine_interface.gui_border_thickness,message,engine_interface.current_color_theme()->tooltip_font);
        }
    }
}
