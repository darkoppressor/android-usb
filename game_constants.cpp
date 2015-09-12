/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "game_constants.h"
#include "world.h"

using namespace std;

double ZOOM_RATE=0.0;
double ZOOM_MIN=0.0;
double ZOOM_MAX=0.0;

void set_game_constant(string name,string value){
    if(name=="zoom_rate"){
        ZOOM_RATE=string_stuff.string_to_double(value);
    }
    else if(name=="zoom_min"){
        ZOOM_MIN=string_stuff.string_to_double(value);
    }
    else if(name=="zoom_max"){
        ZOOM_MAX=string_stuff.string_to_double(value);
    }
}
