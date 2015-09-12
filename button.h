/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef button_h
#define button_h

#include "sprite.h"

#include <string>

//Incomplete declaration of Window.
//We have to do this here, because a pointer to Window objects is used by the event-related functions, but Window relies on Button.
class Window;

class Button{
public:

    //The coordinate location.
    //This location, as used by a window, is an offset from the window, not the actual screen.
    short x,y;

    short w,h;

    //The starting coordinates.
    short start_x;
    short start_y;

    //If true, the mouse is currently over this button.
    //If false, it is not.
    bool moused_over;

    //If true, the mouse button has clicked down on this button.
    //If false, it has not.
    bool clicked;

    //If true, this button is its window's special case X button.
    //If false, it is not.
    bool is_x;

    //If true, this button is a divider button.
    //If false, it is not.
    bool is_divider;

    //What event function the button will call when it fires its event.
    std::string event_function;

    //Alternate events.
    std::string alt_function1;
    std::string alt_function2;
    std::string alt_function3;

    //Sounds for mousing over and firing the button's event.
    std::string mouse_over_sound;
    std::string event_fire_sound;

    std::string text;
    std::string tooltip_text;

    std::string font;
    std::string font_color;

    Sprite sprite;
    Sprite sprite_moused;
    Sprite sprite_click;

    Button();

    void set_default_font();

    void set_dimensions();
    void set_dimensions_text();

    void center_in_window(int window_width,int window_height);

    bool is_moused_over(int mouse_x,int mouse_y,short x_offset,short y_offset);

    //Resets moused over state.
    //Returns true if the button was moused over.
    //Returns false if the button was not moused over.
    bool reset_moused_over();

    //Resets clicked state.
    void reset_clicked();

    //The mouse is over this button.
    void mouse_over(bool previously_moused_over);

    //The mouse button has clicked down on this button.
    void mouse_button_down();

    //The mouse button has been released over this button.
    bool mouse_button_up(Window* parent_window);

    bool has_tooltip();

    bool fire_event(Window* parent_window);

    bool fire_alt_event1(Window* parent_window);
    bool fire_alt_event2(Window* parent_window);
    bool fire_alt_event3(Window* parent_window);

    void animate();
    void render(short x_offset,short y_offset);
};

#endif
