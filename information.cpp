/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "information.h"
#include "world.h"
#include "render.h"

using namespace std;

Information::Information(){
    x=0;
    y=0;
    start_x=0;
    start_y=0;
    text="";
    tooltip_text="";
    text_mutable=false;
    max_text_length=0;
    scrolling=false;
    scroll_width=0;
    scroll_height=0;
    scroll_offset=0;
    sprite.name="";
    background_type="";
    background_opacity=1.0;
    font_color="";
    special_info_text="";
    special_info_sprite="";
    font="";
}

void Information::set_default_font(){
    if(font.length()==0){
        font=engine_interface.default_font;
    }
}

void Information::set_dimensions(){
    set_default_font();

    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    if(scrolling || (scroll_width>0 && scroll_height>0)){
        w=engine_interface.gui_border_thickness*2.0+ptr_font->spacing_x*(scroll_width+1);
        h=engine_interface.gui_border_thickness*2.0+ptr_font->spacing_y*scroll_height;
    }
    else if(sprite.name.length()>0){
        w=sprite.get_width();
        h=sprite.get_height();
    }
    else if(text.length()>0 || text_mutable){
        if(text_mutable){
            w=engine_interface.gui_border_thickness*2.0+ptr_font->spacing_x*(max_text_length+1);
            h=engine_interface.gui_border_thickness*2.0+ptr_font->spacing_y;
        }
        else{
            w=engine_interface.gui_border_thickness*2.0+string_stuff.longest_line(text)*ptr_font->spacing_x+ptr_font->gui_padding_x;
            h=engine_interface.gui_border_thickness*2.0+(string_stuff.newline_count(text)+1)*ptr_font->spacing_y+ptr_font->gui_padding_y;
        }
    }
}

void Information::center_in_window(int window_width,int window_height){
    if(start_x==-1){
        x=(window_width-w)/2;
    }
    else if(start_x==-2){
        x=engine_interface.window_border_thickness;
    }
    else if(start_x==-3){
        x=window_width-w-engine_interface.window_border_thickness;
    }

    if(start_y==-1){
        y=(window_height-h)/2;
    }
    else if(start_y==-2){
        y=engine_interface.window_border_thickness;
    }
    else if(start_y==-3){
        y=window_height-h-engine_interface.window_border_thickness;
    }
}

void Information::set_text(string get_text){
    text=get_text;

    set_dimensions();
}

void Information::set_special_text(){
    text=engine_interface.special_info_manager.get_special_info_text(special_info_text);
}

void Information::set_special_sprite(){
    sprite.name=engine_interface.special_info_manager.get_special_info_sprite(special_info_sprite);
}

bool Information::allows_input(string input_type){
    for(int i=0;i<allowed_input.size();i++){
        if(allowed_input[i]==input_type){
            return true;
        }
    }

    return false;
}

void Information::begin_editing(){
    if(text_mutable){
        engine_interface.set_mutable_info(this);

        if(scrolling){
            scroll_offset=-string_stuff.newline_count(text);
        }
    }
}

void Information::scroll_up(short y_offset){
    if(scrolling){
        if(y_offset+y+engine_interface.get_font(font)->spacing_y*scroll_offset<y_offset+y+h-engine_interface.get_font(font)->spacing_y*2){
            scroll_offset+=1;
        }
    }
}

void Information::scroll_down(short y_offset){
    if(scrolling){
        if(y_offset+y+engine_interface.get_font(font)->spacing_y*scroll_offset+engine_interface.get_font(font)->spacing_y*string_stuff.newline_count(text)>y_offset+y){
            scroll_offset-=1;
        }
    }
}

void Information::handle_input_states(int mouse_x,int mouse_y,short x_offset,short y_offset){
    if(tooltip_text.length()>0){
        Collision_Rect box_a(mouse_x,mouse_y,engine_interface.cursor_width,engine_interface.cursor_height);
        Collision_Rect box_b(x_offset+x,y_offset+y,w,h);

        if(engine_interface.mouse_allowed() && engine_interface.gui_mode=="mouse" && collision_check_rect(box_a,box_b)){
            engine_interface.tooltip.setup(tooltip_text,mouse_x,mouse_y);
        }
        else if((engine_interface.gui_mode=="keyboard" || engine_interface.gui_mode=="controller") && engine_interface.is_gui_object_selected(this)){
            engine_interface.tooltip.setup(tooltip_text,x_offset+x+w/2,y_offset+y+h/2);
        }
    }
}

bool Information::handle_input_events(int mouse_x,int mouse_y,short x_offset,short y_offset){
    Collision_Rect box_a(mouse_x,mouse_y,engine_interface.cursor_width,engine_interface.cursor_height);
    Collision_Rect box_b(x_offset+x,y_offset+y,w,h);

    switch(event.type){
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button==SDL_BUTTON_LEFT){
                if(text_mutable){
                    if(collision_check_rect(box_a,box_b)){
                        begin_editing();

                        return true;
                    }
                }
            }
            break;

        case SDL_MOUSEWHEEL:
            if(event.wheel.y<0){
                if(scrolling){
                    if(collision_check_rect(box_a,box_b)){
                        scroll_down(y_offset);

                        return true;
                    }
                }
            }
            else if(event.wheel.y>0){
                if(scrolling){
                    if(collision_check_rect(box_a,box_b)){
                        scroll_up(y_offset);

                        return true;
                    }
                }
            }
            break;
    }

    return false;
}

void Information::animate(){
    //If the information has a sprite.
    if(sprite.name.length()>0){
        sprite.animate();
    }
}

void Information::render(short x_offset,short y_offset){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    //If there is special info, we must process it before rendering.
    if(special_info_text.length()>0){
        set_special_text();

        set_dimensions();
    }
    if(special_info_sprite.length()>0){
        set_special_sprite();

        set_dimensions();
    }

    if(sprite.name.length()>0 && background_type=="sprite"){
        sprite.render(x_offset+x,y_offset+y,background_opacity);
    }
    else if(background_type=="standard"){
        //Render the border.
        if(engine_interface.current_color_theme()->information_border!="<INVISIBLE>"){
            render_rectangle(x_offset+x,y_offset+y,w,h,background_opacity,engine_interface.current_color_theme()->information_border);
        }

        //Render the background.
        if(engine_interface.current_color_theme()->information_background!="<INVISIBLE>"){
            render_rectangle(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,background_opacity,engine_interface.current_color_theme()->information_background);
        }
    }

    //If the information has text.
    if(text.length()>0 || text_mutable){
        //If we are scrolling, we will pass an SDL_Rect along to the font renderer to let it know
        //what text is allowed to be displayed.
        SDL_Rect allowed_area;
        allowed_area.x=-1;
        allowed_area.y=-1;
        allowed_area.w=0;
        allowed_area.h=0;

        if(scrolling){
            allowed_area.x=x_offset+x;
            allowed_area.y=y_offset+y;
            allowed_area.w=w;
            allowed_area.h=h;
        }

        int scrolling_adjust_y=0;
        if(scrolling){
            scrolling_adjust_y=ptr_font->spacing_y*scroll_offset;
        }

        string font_color_real=font_color;
        if(font_color.length()==0){
            font_color_real=engine_interface.current_color_theme()->information_font;
        }

        if(font_color_real!="<INVISIBLE>"){
            ptr_font->show(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness+scrolling_adjust_y,text,font_color_real,1.0,1.0,1.0,0.0,allowed_area);

            if(text_mutable && engine_interface.mutable_info_this(this)){
                ptr_font->show(x_offset+x+engine_interface.gui_border_thickness+(ptr_font->spacing_x*string_stuff.length_of_last_line(text)),y_offset+y+engine_interface.gui_border_thickness+scrolling_adjust_y+(ptr_font->spacing_y*string_stuff.newline_count(text)),"\x7F",font_color_real,engine_interface.cursor_opacity*0.1,1.0,1.0,0.0,allowed_area);
            }
        }
    }
    //If the information has a sprite, and it is not already being used as the background.
    else if(sprite.name.length()>0 && background_type!="sprite"){
        sprite.render(x_offset+x,y_offset+y);
    }
}
