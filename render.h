/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef render_h
#define render_h

#include "image_data.h"
#include "rtt_data.h"
#include "collision.h"

#include <string>

#include <SDL.h>

SDL_Surface* scale_surface(SDL_Surface* surface,double scale_x,double scale_y);

SDL_Surface* optimize_surface(SDL_Surface* surface);

SDL_Surface* load_image(std::string filename);

SDL_Texture* load_texture(std::string filename,Image_Data* id);

void render_rtt(double x,double y,Rtt_Data rtt_source,double opacity=1.0,double scale_x=1.0,double scale_y=1.0,double angle=0.0,std::string color_name="white",bool flip_x=false);

void render_texture(double x,double y,Image_Data image_source,double opacity=1.0,double scale_x=1.0,double scale_y=1.0,double angle=0.0,std::string color_name="white",bool flip_x=false);

void render_sprite(double x,double y,Image_Data image_source,Collision_Rect* texture_clip=0,double opacity=1.0,double scale_x=1.0,double scale_y=1.0,double angle=0.0,std::string color_name="white",bool flip_x=false);

void render_rectangle(double x,double y,double w,double h,double opacity,std::string color_name);

void render_rectangle_empty(double x,double y,double w,double h,double opacity,std::string color_name,double line_width);

void render_circle(double x,double y,double radius,double opacity,std::string color_name);

void render_circle_empty(double x,double y,double radius,double opacity,std::string color_name);

void render_line(double x1,double y1,double x2,double y2,double opacity,std::string color_name);

#endif
