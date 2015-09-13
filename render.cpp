/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "render.h"
#include "world.h"

#include <SDL_image.h>

using namespace std;

SDL_Surface* scale_surface(SDL_Surface* surface,double scale_x,double scale_y){
    if(surface==0 || scale_x==0.0 || scale_y==0.0){
        return 0;
    }

    int width=ceil((double)surface->w*scale_x);
    int height=ceil((double)surface->h*scale_y);

    SDL_Surface* new_surface=SDL_CreateRGBSurface(surface->flags,width,height,surface->format->BitsPerPixel,surface->format->Rmask,surface->format->Gmask,surface->format->Bmask,surface->format->Amask);

    if(new_surface==0){
        string msg="Error creating scaled surface: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    for(int x=0;x<surface->w;x++){
        for(int y=0;y<surface->h;y++){
            for(int o_x=0;o_x<scale_x;o_x++){
                for(int o_y=0;o_y<scale_y;o_y++){
                    surface_put_pixel(new_surface,(int)((scale_x*x)+o_x),(int)((scale_y*y)+o_y),surface_get_pixel(surface,x,y));
                }
            }
        }
    }

    return new_surface;
}

SDL_Surface* optimize_surface(SDL_Surface* surface){
    SDL_PixelFormat RGBAFormat;
    RGBAFormat.palette=0;
    RGBAFormat.BitsPerPixel=32;
    RGBAFormat.BytesPerPixel=4;

    if(SDL_BYTEORDER==SDL_BIG_ENDIAN){
        RGBAFormat.Rmask=0xFF000000; RGBAFormat.Rshift=0; RGBAFormat.Rloss=0;
        RGBAFormat.Gmask=0x00FF0000; RGBAFormat.Gshift=8; RGBAFormat.Gloss=0;
        RGBAFormat.Bmask=0x0000FF00; RGBAFormat.Bshift=16; RGBAFormat.Bloss=0;
        RGBAFormat.Amask=0x000000FF; RGBAFormat.Ashift=24; RGBAFormat.Aloss=0;
    }
    else{
        RGBAFormat.Rmask=0x000000FF; RGBAFormat.Rshift=24; RGBAFormat.Rloss=0;
        RGBAFormat.Gmask=0x0000FF00; RGBAFormat.Gshift=16; RGBAFormat.Gloss=0;
        RGBAFormat.Bmask=0x00FF0000; RGBAFormat.Bshift=8; RGBAFormat.Bloss=0;
        RGBAFormat.Amask=0xFF000000; RGBAFormat.Ashift=0; RGBAFormat.Aloss=0;
    }

    SDL_Surface* conv=SDL_ConvertSurface(surface,&RGBAFormat,0);

    if(conv==0){
        string msg="Error converting surface: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    return conv;
}

SDL_Surface* load_image(string filename){
    SDL_Surface* loaded_image=0;
    SDL_Surface* optimized_image=0;

    loaded_image=IMG_Load(filename.c_str());

    if(loaded_image!=0){
        optimized_image=optimize_surface(loaded_image);
        SDL_FreeSurface(loaded_image);
    }
    else{
        message_log.add_error("Error loading image '"+filename+"': "+IMG_GetError());
    }

    return optimized_image;
}

SDL_Texture* load_texture(string filename,Image_Data* id){
    SDL_Surface* surface=load_image(filename.c_str());

    id->w=surface->w;
    id->h=surface->h;

    SDL_Texture* texture=SDL_CreateTextureFromSurface(main_window.renderer,surface);

    if(texture==0){
        string msg="Error creating texture: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    SDL_FreeSurface(surface);

    return texture;
}

void render_rtt(double x,double y,Rtt_Data rtt_source,double opacity,double scale_x,double scale_y,double angle,string color_name,bool flip_x){
    SDL_Rect rect_dst;
    rect_dst.x=x;
    rect_dst.y=y;
    rect_dst.w=rtt_source.w*scale_x;
    rect_dst.h=rtt_source.h*scale_y;

    SDL_RendererFlip flip=SDL_FLIP_NONE;
    if(flip_x){
        flip=SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureAlphaMod(rtt_source.texture,(short)(opacity*255.0));

    Color* color=engine_interface.get_color(color_name);
    SDL_SetTextureColorMod(rtt_source.texture,color->get_red_short(),color->get_green_short(),color->get_blue_short());

    SDL_RenderCopyEx(main_window.renderer,rtt_source.texture,0,&rect_dst,-angle,0,flip);
}

void render_texture(double x,double y,Image_Data image_source,double opacity,double scale_x,double scale_y,double angle,string color_name,bool flip_x){
    SDL_Rect rect_dst;
    rect_dst.x=x;
    rect_dst.y=y;
    rect_dst.w=image_source.w*scale_x;
    rect_dst.h=image_source.h*scale_y;

    SDL_RendererFlip flip=SDL_FLIP_NONE;
    if(flip_x){
        flip=SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureAlphaMod(image_source.texture,(short)(opacity*255.0));

    Color* color=engine_interface.get_color(color_name);
    SDL_SetTextureColorMod(image_source.texture,color->get_red_short(),color->get_green_short(),color->get_blue_short());

    SDL_RenderCopyEx(main_window.renderer,image_source.texture,0,&rect_dst,-angle,0,flip);
}

void render_sprite(double x,double y,Image_Data image_source,Collision_Rect* texture_clip,double opacity,double scale_x,double scale_y,double angle,string color_name,bool flip_x){
    SDL_Rect rect_src;
    rect_src.x=texture_clip->x;
    rect_src.y=texture_clip->y;
    rect_src.w=texture_clip->w;
    rect_src.h=texture_clip->h;

    SDL_Rect rect_dst;
    rect_dst.x=x;
    rect_dst.y=y;
    rect_dst.w=texture_clip->w*scale_x;
    rect_dst.h=texture_clip->h*scale_y;

    SDL_RendererFlip flip=SDL_FLIP_NONE;
    if(flip_x){
        flip=SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureAlphaMod(image_source.texture,(short)(opacity*255.0));

    Color* color=engine_interface.get_color(color_name);
    SDL_SetTextureColorMod(image_source.texture,color->get_red_short(),color->get_green_short(),color->get_blue_short());

    SDL_RenderCopyEx(main_window.renderer,image_source.texture,&rect_src,&rect_dst,-angle,0,flip);
}

void render_rectangle(double x,double y,double w,double h,double opacity,string color_name){
    SDL_SetRenderDrawBlendMode(main_window.renderer,SDL_BLENDMODE_BLEND);

    Color* color=engine_interface.get_color(color_name);
    SDL_SetRenderDrawColor(main_window.renderer,color->get_red_short(),color->get_green_short(),color->get_blue_short(),(short)(opacity*255.0));

    SDL_Rect rect;
    rect.x=x;
    rect.y=y;
    rect.w=w;
    rect.h=h;

    SDL_RenderFillRect(main_window.renderer,&rect);
}

void render_rectangle_empty(double x,double y,double w,double h,double opacity,string color_name,double line_width){
    render_rectangle(x,y,w,line_width,opacity,color_name);
    render_rectangle(x,y,line_width,h,opacity,color_name);
    render_rectangle(x,y+h-line_width,w,line_width,opacity,color_name);
    render_rectangle(x+w-line_width,y,line_width,h,opacity,color_name);
}

void render_circle(double x,double y,double radius,double opacity,string color_name){
    double sprite_size=radius*2.0;
    double sprite_scale=sprite_size/1024.0;

    render_texture(x-radius,y-radius,*image.get_image("circle"),opacity,sprite_scale,sprite_scale,0.0,color_name);
}

void render_circle_empty(double x,double y,double radius,double opacity,string color_name){
    double sprite_size=radius*2.0;
    double sprite_scale=sprite_size/1024.0;

    render_texture(x-radius,y-radius,*image.get_image("circle_empty"),opacity,sprite_scale,sprite_scale,0.0,color_name);
}

void render_line(double x1,double y1,double x2,double y2,double opacity,string color_name){
    SDL_SetRenderDrawBlendMode(main_window.renderer,SDL_BLENDMODE_BLEND);

    Color* color=engine_interface.get_color(color_name);
    SDL_SetRenderDrawColor(main_window.renderer,color->get_red_short(),color->get_green_short(),color->get_blue_short(),(short)(opacity*255.0));

    SDL_RenderDrawLine(main_window.renderer,x1,y1,x2,y2);
}
