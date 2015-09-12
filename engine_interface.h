/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef engine_interface_h
#define engine_interface_h

#include "file_io.h"
#include "rng.h"
#include "special_info.h"
#include "button_events.h"
#include "font.h"
#include "cursor.h"
#include "color.h"
#include "tooltip.h"
#include "toast.h"
#include "color_theme.h"
#include "animation.h"
#include "window.h"
#include "timer.h"
#include "console.h"
#include "touch_controller.h"
#include "coords.h"
#include "game_command.h"
#include "game_option.h"
#include "game_constants.h"
#include "sprite.h"

///#include "example_game_script.h"

#include <string>

class Version_Series{
public:

    int major_1;
    int minor_1;
    int micro_1;

    int major_2;
    int minor_2;
    int micro_2;

    Version_Series(int get_major_1,int get_minor_1,int get_micro_1,int get_major_2,int get_minor_2,int get_micro_2);
};

class GUI_Object{
public:

    //Valid values:
    //button
    //information
    std::string type;

    int index;

    int x;
    int y;

    GUI_Object(std::string get_type,int get_index,int get_x,int get_y);
};

class GUI_Selector_Chaser{
public:

    double x;
    double y;

    GUI_Selector_Chaser();
};

class Controller{
public:

    SDL_GameController* controller;
    SDL_JoystickID instance_id;
    SDL_Haptic* haptic;

    Controller(SDL_GameController* get_controller);
};

class Engine_Interface{
public:

    Special_Info special_info_manager;
    Button_Events button_events_manager;

    std::vector<Bitmap_Font> fonts;
    std::vector<Cursor> cursors;
    std::vector<Color> colors;
    std::vector<Color_Theme> color_themes;
    std::vector<Animation> animations;
    std::vector<Window> windows;

    ///std::vector<Example_Game_Script> example_game_scripts;

    Tooltip tooltip;

    std::vector<Toast> toasts;

    Console console;

    Console chat;

    Touch_Controller touch_controller;

    //The z order of all open windows.
    std::vector<Window*> window_z_order;
    //The window that the mouse is currently over.
    Window* window_under_mouse;

    bool controller_text_entry_small;

    //If true, always render the cursor.
    //If false, only render the cursor when a window is open.
    bool cursor_render_always;

    //The current cursor.
    std::string cursor;

    //Cursor for when a button is moused over.
    std::string cursor_mouse_over;

    //The current color theme.
    std::string color_theme;

    //The font for toast notifications.
    std::string toast_font;

    //The default font for objects that do not have a font specified.
    std::string default_font;

    //Various lengths for toast notifications, in seconds.
    int toast_length_short;
    int toast_length_medium;
    int toast_length_long;

    //The game's logical resolution.
    //If resolution mode is fixed, the game window will be forced to remain at this resolution, ignoring the resolution option.
    //If resolution mode is scaling, the game logic will use this resolution, and the game window will scale to the resolution option.
    //If resolution mode is standard, this is ignored, and logical resolution matches the resolution option.
    int logical_screen_width;
    int logical_screen_height;

    //How the game handles different logical resolutions and resolution options.
    std::string resolution_mode;

    int axis_scroll_rate;

    int scrolling_buttons_offset;

    int cursor_width;
    int cursor_height;

    //These heights are in lines of text.
    int console_height;
    int chat_height;

    double sound_falloff;

    double window_border_thickness;
    double gui_border_thickness;

    double touch_finger_size;
    bool touch_controller_shoulders;
    bool touch_controller_guide;
    bool touch_controller_xy;

    std::string game_title;
    std::string home_directory;
    std::string developer;

    std::vector<std::string> starting_windows;

    std::string option_save_location;

    std::string option_version;

    int option_screen_width;
    int option_screen_height;
    bool option_fullscreen;
    std::string option_fullscreen_mode;

    bool option_vsync;
    bool option_accelerometer_controller;
    bool option_touch_controller;
    double option_touch_controller_opacity;
    bool option_font_shadows;
    bool option_screen_keyboard;
	bool option_hw_cursor;
	bool option_bind_cursor;

    bool option_fps;
    bool option_dev;
    double option_volume_global;
    double option_volume_sound;
    double option_volume_music;
    bool option_mute_global;
    bool option_mute_sound;
    bool option_mute_music;

    bool hide_gui;

    RNG rng;

    //Set this to true during runtime to tell the engine to reinitialize everything.
    bool need_to_reinit;

    //If true, some button is moused over this frame.
    bool mouse_over;

    //If true, we are using the on-screen touch controller.
    bool touch_controls;

    std::vector<SDL_Event> touch_controller_events;

    //Possible values:
    //mouse
    //keyboard
    //controller
    std::string gui_mode;

    //-1 if no object is selected.
    int gui_selected_object;

    std::string gui_selector_style;

    std::vector<GUI_Selector_Chaser> gui_selector_chasers;

    std::vector<Controller> controllers;

    std::vector<Game_Command> game_commands;

    std::vector<Game_Option> game_options;

    std::string gui_axis_nav_last_direction;

    int controller_dead_zone;

    int counter_gui_scroll_axis;

    int configure_command;

    std::vector<std::string> characters_lower;
    std::vector<std::string> characters_upper;
    std::vector<std::string> characters_numbers;
    std::vector<std::string> characters_symbols;

    Coords text_entry_small_selector;

    Sprite text_selector;

    int editing_server;

    //Mutable text cursor.
    //The time to spend on each step of fading animation, in milliseconds.
    int counter_cursor;
    bool cursor_fade_direction;
    int cursor_opacity;

    //Pointer to the current mutable text Information object, if any.
    Information* ptr_mutable_info;

    Engine_Interface();

    //Reinitialize the game window when next possible.
    void reload();

    void quit();

    void build_text_input_characters();

    void set_logic_update_rate(double frame_rate);
    void set_render_update_rate(double frame_rate);

    //Loads only engine data.
    //Returns false if engine data could not be loaded.
    //Returns true otherwise.
    bool load_data_engine();

    void load_data_game();

    //Loads everything but engine data and game options.
    void load_data_main();

    //Loads only game options.
    void load_data_game_options();

    //Load any data of the passed script type.
    //Returns false if passed script type's data could not be loaded.
    //Returns true otherwise.
    void load_data_script_game(std::string script,File_IO_Load* load);

    //Load any data of the passed script type.
    //Returns false if passed script type's data could not be loaded.
    //Returns true otherwise.
    bool load_data(std::string script);

    void unload_data_game();
    void unload_data();

    void load_engine_data(File_IO_Load* load);
    void load_font(File_IO_Load* load);
    void load_cursor(File_IO_Load* load);
    void load_color(File_IO_Load* load);
    void load_color_theme(File_IO_Load* load);
    void load_animation(File_IO_Load* load);
    void load_window(File_IO_Load* load);
    void load_information(File_IO_Load* load);
    void load_button(File_IO_Load* load);
    void load_game_command(File_IO_Load* load);
    void load_game_option(File_IO_Load* load);
    void load_game_constant(File_IO_Load* load);

    Bitmap_Font* get_font(std::string name);
    Cursor* get_cursor(std::string name);
    Color* get_color(std::string name);
    Color_Theme* get_color_theme(std::string name);
    Animation* get_animation(std::string name);
    Window* get_window(std::string name);
    Game_Command* get_game_command(std::string name);
    Game_Option* get_game_option(std::string name);

    ///void load_example_game_script(File_IO_Load* load);
    ///Example_Game_Script* get_example_game_script(std::string name);

    Color_Theme* current_color_theme();
    bool animation_exists(std::string animation_name);

    //Rebuild any window data if needed.
    void rebuild_window_data();

    bool is_window_open(Window* window);
    bool is_window_on_top(Window* window);
    Window* get_top_window();
    void open_window(Window* window);
    void close_window(Window* window);
    void bring_window_to_top(Window* window);

    bool is_exclusive_window_open();
    bool is_any_window_open();
    int open_window_count();
    void close_all_windows(Window* ignore=0);
    void close_top_window();
    bool is_mouse_over_window();
    bool is_mouse_over_open_window();

    bool poll_event(SDL_Event* event_storage);

    bool mouse_allowed();

    //Check if we need to scroll the scrolling button list.
    void gui_check_scrolling_button();
    void reset_gui_selector_chasers();
    void reset_gui_selected_object();
    void set_gui_mode(std::string new_gui_mode);
    //Returns the number of GUI objects on the currently focused window.
    //Returns 0 if no window is currently in focus.
    int get_gui_object_count();
    void change_gui_selected_object(std::string direction);
    //Returns a GUI_Object for the selected GUI object.
    //If there is no selected GUI object, the GUI_Object's type will equal none.
    GUI_Object get_gui_selected_object();
    //Returns a Engine_Rect with the position and size of the selected GUI object.
    //If there is no selected GUI object, returns a Engine_Rect of all -1's.
    Engine_Rect get_gui_selected_object_pos();
    bool is_gui_object_selected(Button* button_to_check);
    bool is_gui_object_selected(Information* info_to_check);
    //Activates the currently selected GUI object.
    void confirm_gui_object();
    void scroll_gui_object_up();
    void scroll_gui_object_down();

    void get_mouse_state(int* mouse_x,int* mouse_y);
    void get_rgba_masks(uint32_t* rmask,uint32_t* gmask,uint32_t* bmask,uint32_t* amask);

    void update_window_caption(int render_rate,double ms_per_frame,int logic_frame_rate);

    //Sets the passed info to be the currently selected mutable.
    void set_mutable_info(Information* ptr_info);
    //Clear the selected info mutable.
    void clear_mutable_info();
    //Returns true if there is a mutable info selected,
    //false otherwise.
    bool mutable_info_selected();
    //Returns true if the passed info matches the selected mutable info.
    bool mutable_info_this(Information* ptr_info);

    void add_chat(std::string message);

    void make_notice(std::string message);

    void make_toast(std::string message,std::string length="medium",int custom_length=-1);

    //Play a rumble effect on the passed controller.
    //Pass CONTROLLER_ID_ALL for controller_number to play the rumble on all controllers.
    //Pass CONTROLLER_ID_TOUCH for controller_number to play the rumble on the touch controller.
    //strength is a 0.0-1.0 float.
    //length is in milliseconds.
    void make_rumble(int controller_number,float strength,uint32_t length);

    void handle_text_input(std::string text);

    //Returns true if the passed button is currently pressed on the passed controller number.
    //Returns false if it is not.
    //Pass CONTROLLER_ID_ALL for controller_number to check if any controller has the passed button pressed.
    //Pass CONTROLLER_ID_TOUCH for controller_number to check if the touch controller has the passed button pressed.
    bool controller_state(int controller_number,SDL_GameControllerButton button);

    //Returns the axis state as a value from -32768 to 32767.
    //Returns 0 on failure.
    //Pass CONTROLLER_ID_ALL for controller_number to check for a non-zero axis value on any controller.
    //Pass CONTROLLER_ID_TOUCH for controller_number to check for a non-zero axis value on the touch controller.
    int controller_state(int controller_number,SDL_GameControllerAxis axis);

    //If the game command uses a key or button:
    //Returns 1 if the passed game command's corresponding input is currently pressed.
    //Returns 0 if the passed game command's corresponding input is currently unpressed.
    //If the game command uses an axis:
    //Returns the axis state as a value from -32768 to 32767.
    int game_command_state(std::string name);

    //Only used by Android hardware buttons (I think...):
    void gui_nav_back_android();
    void gui_nav_toggle_menu_android();

    //Only used by controller mode:
    void gui_nav_back_controller();
    void gui_nav_toggle_menu_controller();

    //Only used by mouse and keyboard modes:
    void gui_nav_back_keyboard_and_mouse();

    //Used by all control modes:
    void gui_nav_confirm(std::string gui_type);
    void gui_nav_up(std::string gui_type);
    void gui_nav_down(std::string gui_type);
    void gui_scroll_up(std::string gui_type);
    void gui_scroll_down(std::string gui_type);

    //Returns true if there is a mutable info selected, and it is the console.
    bool is_console_selected();

    //Returns true if there is a mutable info selected, and it is the chat box.
    bool is_chat_selected();

    int get_text_input_selected_chunk();
    std::vector<std::string>* get_text_input_character_set();

    void input_backspace();
    void input_newline();

    bool allow_screen_keyboard();

    std::string get_text_entry_small_character();

    void prepare_for_input();

    bool handle_input_events_drag_and_drop();
    bool handle_input_events_touch();
    bool handle_input_events_command_set();
    bool handle_input_events(bool event_ignore_command_set);
    void handle_input_states();
    void animate();
    void animate_gui_selector_chasers();
    void render_title_background();
    void render_pause();
    void render_fps(int render_rate,double ms_per_frame,int logic_frame_rate);
    void render_gui_selector();
    void render_small_text_inputter();
    void render_text_inputter();
    void render(int render_rate,double ms_per_frame,int logic_frame_rate);
    void render_loading_screen(double percentage,std::string load_message);

    std::string get_game_window_caption();

    void render_dev_info();
    std::string get_system_info();

    //** Options **//

    std::string get_version();
    std::string get_build_date();

    std::string get_checksum();

    //Returns a string with a time and/or date stamp.
    //If filename is true, the returned timestamp is in a format appropriate for a filename.
    std::string get_timestamp(bool include_date=true,bool include_time=true,bool filename=false);

    //Returns a string with the absolute path to the current working directory.
    std::string get_cwd();
    //Returns a string with the absolute path to the current save location.
    std::string get_save_path();

    //Change any incorrect slashes to the correct type.
    void correct_slashes(std::string* str_input);
    //Returns a string with the home directory.
    std::string get_home_directory();
    //Create the home directory.
    void make_home_directory();
    //Create the entire needed directory structure.
    void make_directories();

    bool save_save_location();
    bool load_save_location();

    //Compares two versions.
    //Returns 0 if they are the same.
    //Returns -1 if version 1 is less than version 2.
    //Returns 1 if version 1 is greater than version 2.
    int version_compare(int major_1,int minor_1,int micro_1,int major_2,int minor_2,int micro_2);

    //Returns the index into the passed vector that the passed version is a part of.
    //Returns -1 if the passed version is not in any series.
    int which_version_series(std::vector<Version_Series>* version_series,int major,int minor,int micro);

    //Returns true if the version of the passed profile is in the same series of compatible releases as the current version.
    bool version_compatible(std::string name_to_check);

    std::string get_option_value(std::string option);
    std::string get_option_description(std::string option);
    void change_option(std::string option,std::string new_value);

    //Returns false if options could not be saved.
    //Returns true otherwise.
    bool save_options();
    //Returns false if options could not be loaded.
    //Returns true otherwise.
    bool load_options();

    //Returns false if game commands could not be saved.
    //Returns true otherwise.
    bool save_game_commands();
    //Returns false if game commands could not be loaded.
    //Returns true otherwise.
    bool load_game_commands();

    //Returns false if servers could not be saved.
    //Returns true otherwise.
    bool save_servers();
    //Returns false if servers could not be loaded.
    //Returns true otherwise.
    bool load_servers();
};

#endif
