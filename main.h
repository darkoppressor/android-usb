/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef main_h
#define main_h

#include <SDL.h>

void game_loop();

//Handle Android/iOS events
int handle_app_events(void* userdata,SDL_Event* event);

//Apparently, SDL likes main() to take these arguments, so that is what we will do.
int main(int argc,char* args[]);

#endif
