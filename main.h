#ifndef main_h
#define main_h

#include <SDL.h>

void game_loop();

//Handle Android/iOS events
int handle_app_events(void* userdata,SDL_Event* event);

//Apparently, SDL likes main() to take these arguments, so that is what we will do.
int main(int argc,char* args[]);

#endif
