#include "screen_shake.h"
#include "world.h"

using namespace std;

Screen_Shake::Screen_Shake(){
    reset();
}

void Screen_Shake::reset(){
    magnitude=0.0;
    length=0;

    direction=0.0;
    x=0.0;
    y=0.0;
    distance_moved=0.0;
    returning=false;

    camera_real_x=0.0;
    camera_real_y=0.0;

    change_direction();
}

bool Screen_Shake::is_active(){
    if(length>0){
        return true;
    }
    else{
        return false;
    }
}

void Screen_Shake::change_direction(){
    if(!returning){
        direction=game.rng.random_range(0,359);
    }
    else{
        Collision_Rect box(x,y,2.0,2.0);
        Collision_Rect box_mid(0.0,0.0,2.0,2.0);
        direction=get_angle_to_rect(box,box_mid,box_mid);
    }
}

void Screen_Shake::add_shake(double get_magnitude,int get_length){
    if(game.option_screen_shake){
        if(get_magnitude>magnitude){
            magnitude=get_magnitude;
        }

        if(magnitude<0.001){
            magnitude=0.001;
        }
        else if(magnitude>0.999){
            magnitude=0.999;
        }

        length+=get_length;
    }
}

void Screen_Shake::movement(){
    if(is_active()){
        Vector velocity((0.65/pow(1.0-magnitude,0.25))*((game.camera.w+game.camera.h)/2.0),direction);

        Vector_Components vc=velocity.get_components();

        double move_x=vc.a/UPDATE_RATE;
        double move_y=vc.b/UPDATE_RATE;

        x+=move_x;
        y+=move_y;

        if(!returning){
            distance_moved+=abs(move_x);
            distance_moved+=abs(move_y);

            if(distance_moved>=(0.045/(pow(magnitude,0.5)))*magnitude*((game.camera.w+game.camera.h)/2.0)){
                distance_moved=0.0;

                returning=!returning;

                change_direction();
            }
        }
        else{
            bool done_x=false;
            bool done_y=false;

            if(move_x<0.0 && x<=0.0){
                x=0.0;
                done_x=true;
            }
            else if(move_x>0.0 && x>=0.0){
                x=0.0;
                done_x=true;
            }

            if(move_y<0.0 && y<=0.0){
                y=0.0;
                done_y=true;
            }
            else if(move_y>0.0 && y>=0.0){
                y=0.0;
                done_y=true;
            }

            if(done_x && done_y){
                returning=!returning;

                change_direction();
            }
        }

        length-=(int)ceil(1000.0/UPDATE_RATE);

        if(length<=0){
            reset();
        }
    }
}
