/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "button.h"
#include "world.h"
#include "render.h"

using namespace std;

Button::Button(){
    x=0;
    y=0;
    start_x=0;
    start_y=0;
    text="";
    tooltip_text="";
    font="";
    font_color="";
    event_function="";
    alt_function1="";
    alt_function2="";
    alt_function3="";
    mouse_over_sound="";
    event_fire_sound="";
    sprite.name="";
    sprite_moused.name="";
    sprite_click.name="";

    moused_over=false;
    clicked=false;
    is_x=false;
    is_divider=false;
}

void Button::set_default_font(){
    if(font.length()==0){
        font=engine_interface.default_font;
    }
}

void Button::set_dimensions(){
    set_default_font();

    //Normal.
    if(!moused_over && !clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite.name.length()>0){
            w=sprite.get_width();
            h=sprite.get_height();
        }
        else{
            set_dimensions_text();
        }
    }
    //Moused over.
    else if(moused_over && !clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite_moused.name.length()>0){
            w=sprite_moused.get_width();
            h=sprite_moused.get_height();
        }
        else{
            set_dimensions_text();
        }
    }
    //Clicked down on.
    else if(clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite_click.name.length()>0){
            w=sprite_click.get_width();
            h=sprite_click.get_height();
        }
        else{
            set_dimensions_text();
        }
    }
}

void Button::set_dimensions_text(){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    w=engine_interface.gui_border_thickness*2.0+string_stuff.longest_line(text)*ptr_font->spacing_x+ptr_font->gui_padding_x;
    h=engine_interface.gui_border_thickness*2.0+(string_stuff.newline_count(text)+1)*ptr_font->spacing_y+ptr_font->gui_padding_y;
}

void Button::center_in_window(int window_width,int window_height){
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
        y=window_height-h-engine_interface.window_border_thickness*2;
    }
}

bool Button::is_moused_over(int mouse_x,int mouse_y,short x_offset,short y_offset){
    Collision_Rect box_a(mouse_x,mouse_y,engine_interface.cursor_width,engine_interface.cursor_height);
    Collision_Rect box_b(x_offset+x,y_offset+y,w,h);

    if(engine_interface.mouse_allowed() && engine_interface.gui_mode=="mouse" && collision_check_rect(box_a,box_b)){
        return true;
    }
    else if((engine_interface.gui_mode=="keyboard" || engine_interface.gui_mode=="controller") && engine_interface.is_gui_object_selected(this)){
        return true;
    }

    return false;
}

bool Button::reset_moused_over(){
    if(moused_over){
        moused_over=false;
        return true;
    }
    else{
        return false;
    }
}

void Button::reset_clicked(){
    clicked=false;
}

void Button::mouse_over(bool previously_moused_over){
    if(!previously_moused_over && mouse_over_sound.length()>0 && !clicked){
        sound_system.play_sound(mouse_over_sound);
    }

    moused_over=true;
}

void Button::mouse_button_down(){
    clicked=true;
}

bool Button::mouse_button_up(Window* parent_window){
    bool window_opened_on_top=false;

    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    if(clicked){
        if(((engine_interface.gui_mode=="mouse" || engine_interface.gui_mode=="keyboard") && (keystates[SDL_SCANCODE_RCTRL] || keystates[SDL_SCANCODE_LCTRL])) ||
           (engine_interface.gui_mode=="controller" && engine_interface.controller_state(-1,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))){
            window_opened_on_top=fire_alt_event1(parent_window);
        }
        else if(((engine_interface.gui_mode=="mouse" || engine_interface.gui_mode=="keyboard") && (keystates[SDL_SCANCODE_RSHIFT] || keystates[SDL_SCANCODE_LSHIFT])) ||
                (engine_interface.gui_mode=="controller" && engine_interface.controller_state(-1,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))){
            window_opened_on_top=fire_alt_event2(parent_window);
        }
        else if(((engine_interface.gui_mode=="mouse" || engine_interface.gui_mode=="keyboard") && keystates[SDL_SCANCODE_SLASH]) ||
                (engine_interface.gui_mode=="controller" && engine_interface.controller_state(-1,SDL_CONTROLLER_BUTTON_X))){
            window_opened_on_top=fire_alt_event3(parent_window);
        }
        else{
            window_opened_on_top=fire_event(parent_window);
        }

        if(event_fire_sound.length()>0){
            sound_system.play_sound(event_fire_sound);
        }
    }

    return window_opened_on_top;
}

bool Button::has_tooltip(){
    if(tooltip_text.length()>0){
        return true;
    }
    else{
        return false;
    }
}

bool Button::fire_event(Window* parent_window){
    return engine_interface.button_events_manager.handle_button_event(event_function,parent_window);
}

bool Button::fire_alt_event1(Window* parent_window){
    return engine_interface.button_events_manager.handle_button_event(alt_function1,parent_window);
}
bool Button::fire_alt_event2(Window* parent_window){
    return engine_interface.button_events_manager.handle_button_event(alt_function2,parent_window);
}
bool Button::fire_alt_event3(Window* parent_window){
    return engine_interface.button_events_manager.handle_button_event(alt_function3,parent_window);
}

void Button::animate(){
    //If we are using sprites for the button instead of the standard system.
    if(sprite.name.length()>0){
        sprite.animate();
    }

    //If we are using sprites for the button instead of the standard system.
    if(sprite_moused.name.length()>0){
        sprite_moused.animate();
    }

    //If we are using sprites for the button instead of the standard system.
    if(sprite_click.name.length()>0){
        sprite_click.animate();
    }
}

void Button::render(short x_offset,short y_offset){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    set_dimensions();

    string font_color_real=font_color;
    if(font_color.length()==0){
        font_color_real=engine_interface.current_color_theme()->button_font;
    }

    //Normal.
    if(!moused_over && !clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite.name.length()>0){
            sprite.render(x_offset+x,y_offset+y);
        }
        else{
            //Render the border.
            if(engine_interface.current_color_theme()->button_border!="<INVISIBLE>"){
                render_rectangle(x_offset+x,y_offset+y,w,h,1.0,engine_interface.current_color_theme()->button_border);
            }

            //Render the background.
            if(engine_interface.current_color_theme()->button_background!="<INVISIBLE>"){
                render_rectangle(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,1.0,engine_interface.current_color_theme()->button_background);
            }

            //Display the button's text.
            if(font_color_real!="<INVISIBLE>"){
                ptr_font->show(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,text,font_color_real);
            }
        }
    }
    //Moused over.
    else if(moused_over && !clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite_moused.name.length()>0){
            sprite_moused.render(x_offset+x,y_offset+y);
        }
        else{
            //Render the border.
            if(engine_interface.current_color_theme()->button_border!="<INVISIBLE>"){
                render_rectangle(x_offset+x,y_offset+y,w,h,1.0,engine_interface.current_color_theme()->button_border);
            }

            //Render the background.
            if(engine_interface.current_color_theme()->button_background_moused!="<INVISIBLE>"){
                render_rectangle(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,1.0,engine_interface.current_color_theme()->button_background_moused);
            }

            //Display the button's text.
            if(font_color_real!="<INVISIBLE>"){
                ptr_font->show(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,text,font_color_real);
            }
        }
    }
    //Clicked down on.
    else if(clicked){
        //If we are using sprites for the button instead of the standard system.
        if(sprite_click.name.length()>0){
            sprite_click.render(x_offset+x,y_offset+y);
        }
        else{
            //Render the border.
            if(engine_interface.current_color_theme()->button_border!="<INVISIBLE>"){
                render_rectangle(x_offset+x,y_offset+y,w,h,1.0,engine_interface.current_color_theme()->button_border);
            }

            //Render the background.
            if(engine_interface.current_color_theme()->button_background_click!="<INVISIBLE>"){
                render_rectangle(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,1.0,engine_interface.current_color_theme()->button_background_click);
            }

            //Display the button's text.
            if(font_color_real!="<INVISIBLE>"){
                ptr_font->show(x_offset+x+engine_interface.gui_border_thickness,y_offset+y+engine_interface.gui_border_thickness,text,font_color_real);
            }
        }
    }
}
