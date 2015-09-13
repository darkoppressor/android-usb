/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef information_h
#define information_h

#include "sprite.h"

#include <string>
#include <vector>

class Information{
public:

    //The coordinate location.
    //This location, as used by a window, is an offset from the window, not the actual screen.
    short x,y;

    short w,h;

    //The starting coordinates.
    short start_x;
    short start_y;

    //Only one of text, special_info_text, special_info_sprite, and sprite should be used.

    std::string text;
    std::string tooltip_text;

    //If true, text is mutable.
    //If false, text is immutable.
    bool text_mutable;
    //The maximum length of text when it is mutable.
    int max_text_length;
    //The kind of text input(s) allowed for a mutable string.
    std::vector<std::string> allowed_input;

    bool scrolling;
    //The dimensions (in characters for width and lines for height) of the scroll box.
    //Obviously only applies if scrolling is true.
    int scroll_width;
    int scroll_height;
    int scroll_offset;

    std::string special_info_text;
    std::string special_info_sprite;

    Sprite sprite;
    std::string background_type;
    double background_opacity;

    std::string font;
    std::string font_color;

    Information();

    void set_default_font();

    void set_dimensions();

    void center_in_window(int window_width,int window_height);

    void set_text(std::string get_text);

    //If there is special info text, this function sets the text.
    //This is used to create strings that take some runtime stuff into account.
    void set_special_text();

    //If there is a special info sprite, this function sets the sprite.
    //This is used to create sprites that take some runtime stuff into account.
    void set_special_sprite();

    //Returns true if the passed input type is allowed.
    bool allows_input(std::string input_type);

    void begin_editing();

    void scroll_up(short y_offset);
    void scroll_down(short y_offset);

    void handle_input_states(int mouse_x,int mouse_y,short x_offset,short y_offset);

    //Returns true if the event was consumed,
    //false otherwise.
    bool handle_input_events(int mouse_x,int mouse_y,short x_offset,short y_offset);

    void animate();
    void render(short x_offset,short y_offset);
};

#endif
