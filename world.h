#ifndef world_h
#define world_h

#include "collision.h"
#include "vector.h"
#include "coords.h"
#include "pixels.h"
#include "math.h"

#include "game_window.h"
#include "image.h"
#include "rtt_manager.h"
#include "music.h"
#include "sound.h"
#include "engine_interface.h"
#include "update.h"
#include "message_log.h"
#include "string_stuff.h"
#include "file_io.h"
#include "game.h"
#include "game_constants.h"
#include "network.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdio>

const int CONTROLLER_ID_ALL=-1;
const int CONTROLLER_ID_TOUCH=-2;

extern bool world_loaded;

extern bool save_location_loaded;

extern std::string CURRENT_WORKING_DIRECTORY;
extern std::string CHECKSUM;

extern double UPDATE_RATE;
extern double SKIP_TICKS;

extern double UPDATE_RATE_RENDER;
extern double SKIP_TICKS_RENDER;

extern Message_Log message_log;

extern String_Stuff string_stuff;

extern Game_Window main_window;

extern SDL_Event event;

extern Engine_Interface engine_interface;

extern Update update;

extern File_IO file_io;

extern Network network;

extern Image image;

extern Rtt_Manager rtt_manager;

extern Music music;

extern Sound sound_system;

extern Game game;

bool load_world();

void unload_world();

#endif
