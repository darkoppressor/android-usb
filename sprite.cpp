#include "sprite.h"
#include "world.h"
#include "render.h"

using namespace std;

Sprite::Sprite(){
    name="";

    reset_animation();

    animating=true;
}

double Sprite::get_width(){
    if(is_animated()){
        return engine_interface.get_animation(name)->sprite_sheet[frame].w;
    }
    else{
        return image.get_image(name)->w;
    }
}

double Sprite::get_height(){
    if(is_animated()){
        return engine_interface.get_animation(name)->sprite_sheet[frame].h;
    }
    else{
        return image.get_image(name)->h;
    }
}

bool Sprite::is_animated(){
    if(engine_interface.animation_exists(name)){
        return true;
    }
    else{
        return false;
    }
}

void Sprite::reset_animation(){
    frame=0;
    frame_counter=0;
}

void Sprite::set_name(string get_name){
    name=get_name;

    reset_animation();
}

void Sprite::animate(int animation_speed_override){
    if(is_animated() && animating){
        int animation_speed=engine_interface.get_animation(name)->animation_speed;
        if(animation_speed_override!=-1){
            animation_speed=animation_speed_override;
        }

        if(animation_speed!=-1 && ++frame_counter>=(int)ceil(((double)animation_speed/1000.0)*UPDATE_RATE)){
            frame_counter=0;

            if(++frame>engine_interface.get_animation(name)->frame_count-1){
                frame=0;

                if(engine_interface.get_animation(name)->end_behavior=="stop"){
                    animating=false;
                }
            }
        }
    }
}

void Sprite::render(double x,double y,double opacity,double scale_x,double scale_y,double angle,string color_name){
    if(is_animated()){
        render_sprite(x,y,*image.get_image(name),&engine_interface.get_animation(name)->sprite_sheet[frame],opacity,scale_x,scale_y,angle,color_name);
    }
    else{
        render_texture(x,y,*image.get_image(name),opacity,scale_x,scale_y,angle,color_name);
    }
}
