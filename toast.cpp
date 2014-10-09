#include "toast.h"
#include "render.h"
#include "world.h"

using namespace std;

Toast::Toast(string get_message,double get_fade_rate){
    message=get_message;
    fade_rate=get_fade_rate;

    opacity=1.0;

    string font=engine_interface.toast_font;

    set_dimensions(font);

    x=(main_window.SCREEN_WIDTH-w)/2.0;
    y=main_window.SCREEN_HEIGHT-h-engine_interface.get_font(font)->spacing_y;
}

void Toast::set_dimensions(string font){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    w=engine_interface.gui_border_thickness*2.0+string_stuff.longest_line(message)*ptr_font->spacing_x+ptr_font->gui_padding_x;
    h=engine_interface.gui_border_thickness*2.0+(string_stuff.newline_count(message)+1)*ptr_font->spacing_y+ptr_font->gui_padding_y;
}

bool Toast::is_done(){
    if(opacity==0.0){
        return true;
    }
    else{
        return false;
    }
}

void Toast::animate(){
    opacity-=fade_rate;

    if(opacity<0.0){
        opacity=0.0;
    }
}

void Toast::render(){
    string font=engine_interface.toast_font;

    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    //Render the border.
    if(engine_interface.current_color_theme()->toast_border!="<INVISIBLE>"){
        render_rectangle(x,y,w,h,opacity,engine_interface.current_color_theme()->toast_border);
    }

    //Render the background.
    if(engine_interface.current_color_theme()->toast_background!="<INVISIBLE>"){
        render_rectangle(x+engine_interface.gui_border_thickness,y+engine_interface.gui_border_thickness,w-engine_interface.gui_border_thickness*2.0,h-engine_interface.gui_border_thickness*2.0,opacity,engine_interface.current_color_theme()->toast_background);
    }

    //Display the message text.
    if(engine_interface.current_color_theme()->toast_font!="<INVISIBLE>"){
        ptr_font->show(x+engine_interface.gui_border_thickness,y+engine_interface.gui_border_thickness,message,engine_interface.current_color_theme()->toast_font,opacity);
    }
}
