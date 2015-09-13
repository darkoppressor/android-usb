/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "font.h"
#include "render.h"
#include "world.h"

using namespace std;

Bitmap_Font::Bitmap_Font(){
    name="";

    spacing_x=0;
    spacing_y=0;

    gui_padding_x=0;
    gui_padding_y=0;

    shadow_distance=0;
}

void Bitmap_Font::build_font(){
    //Set the cell dimensions:
    double cellW=sprite.get_width()/256.0;
    double cellH=sprite.get_height();

    //The current character we are setting.
    short currentChar=0;

    //Go through the cell columns.
    for(short cols=0;cols<256;cols++){
        //Set the character offset:
        chars[currentChar].x=cellW*cols;
        chars[currentChar].y=0.0;
        //Set the dimensions of the character:
        chars[currentChar].w=cellW;
        chars[currentChar].h=cellH;
        //Go to the next character.
        currentChar++;
    }
}

double Bitmap_Font::get_letter_width(){
    return sprite.get_width()/256.0;
}
double Bitmap_Font::get_letter_height(){
    return sprite.get_height();
}

void Bitmap_Font::show(double x,double y,string text,string font_color,double opacity,double scale_x,double scale_y,double angle,SDL_Rect allowed_area){
    //Temporary offsets.
    double X=x,Y=y;

    double real_spacing_x=spacing_x*scale_x;
    double real_spacing_y=spacing_y*scale_y;

    //Go through the text.
    for(short show=0;text[show]!='\0';show++){
        //Get the ASCII value of the character.
        short ascii=(unsigned char)text[show];
        if(text[show]!='\xA'){
            if(X+get_letter_width()*scale_x>=0 && X<=main_window.SCREEN_WIDTH && Y+get_letter_height()*scale_y>=0 && Y<=main_window.SCREEN_HEIGHT){
                bool allowed_area_present=false;
                if(allowed_area.x!=-1 || allowed_area.y!=-1 || allowed_area.w!=0 || allowed_area.h!=0){
                    allowed_area_present=true;
                }

                if(!allowed_area_present || (allowed_area_present && X>=allowed_area.x && X+get_letter_width()*scale_x<=allowed_area.x+allowed_area.w && Y>=allowed_area.y && Y+get_letter_height()*scale_y<=allowed_area.y+allowed_area.h)){
                    if(shadow_distance!=0 && engine_interface.option_font_shadows){
                        //Render the shadow.
                        render_sprite((int)X+shadow_distance,(int)Y+shadow_distance,*image.get_image(sprite.name),&chars[ascii],opacity,scale_x,scale_y,angle,"ui_black");
                    }

                    //Render the character.
                    render_sprite((int)X,(int)Y,*image.get_image(sprite.name),&chars[ascii],opacity,scale_x,scale_y,angle,font_color);
                }
            }

            //Move over the width of the character with one pixel of padding.
            X+=real_spacing_x;
        }
        else{
            Y+=real_spacing_y;
            X=x;
        }
    }
}
