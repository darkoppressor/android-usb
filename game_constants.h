#ifndef game_constants_h
#define game_constants_h

#include <string>
#include <stdint.h>

extern double ZOOM_RATE;
extern double ZOOM_MIN;
extern double ZOOM_MAX;

void set_game_constant(std::string name,std::string value);

#endif
