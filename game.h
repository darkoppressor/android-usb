/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef game_h
#define game_h

#include "rng.h"
#include "collision.h"
#include "screen_shake.h"
#include "game_world.h"

#include <string>
#include <vector>

class Game{
public:

    uint32_t option_effect_limit;
    double option_camera_speed;
    double option_camera_zoom;
    std::string option_name;
    bool option_chat_timestamps;
    uint32_t option_max_players;
    bool option_screen_shake;
    std::string option_adb_path;
    std::string option_starting_path;
    bool option_hidden_files;

    bool display_scoreboard;

    RNG rng;

    //If true, a game is currently in progress.
    //If false, a game is not in progress.
    bool in_progress;

    //If true, the game is paused.
    //If false, the game is not paused.
    bool paused;

    //Current movement state of the camera.
    std::string cam_state;

    //Holds a string representing each command that is currently in the on state, and is relevant for networking.
    //This is updated each frame.
    std::vector<std::string> command_states;

    Collision_Rect camera;

    double camera_delta_x;
    double camera_delta_y;

    double camera_speed;
    double camera_zoom;

    Screen_Shake screen_shake;

    std::string current_music;

    Game_World world;

    Game();

    void reset();
    void reset_camera_dimensions();

    std::string get_random_direction_cardinal();
    std::string get_random_direction_cardinal_ordinal();

    //Returns true if the number of effects does not exceed the effect limit.
    bool effect_allowed();

    void manage_music();

    void toggle_pause();

    void start();
    void start_client();
    void stop();

    void center_camera(Collision_Rect box);
    void center_camera(Collision_Circ circle);

    void zoom_camera_in(Collision_Rect box);
    void zoom_camera_in(Collision_Circ circle);

    void zoom_camera_out(Collision_Rect box);
    void zoom_camera_out(Collision_Circ circle);

    void set_camera();

    void render_scoreboard();

    void prepare_for_input();

    void handle_command_states_multiplayer();
    void handle_game_commands_multiplayer();

    void handle_input_states_gui();
    void handle_input_states();
    bool handle_game_command_gui(std::string command_name);
    bool handle_game_command(std::string command_name);
    bool handle_input_events_gui();
    bool handle_input_events();
};

#endif
