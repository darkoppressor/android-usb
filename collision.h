/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef collision_h
#define collision_h

#include <stdint.h>

class Collision_Rect{
public:

    double x;
    double y;
    double w;
    double h;

    Collision_Rect();
    Collision_Rect(double get_x,double get_y,double get_w,double get_h);

    Collision_Rect operator*(double scalar);
    Collision_Rect operator/(double scalar);

    double center_x();
    double center_y();
};

class Collision_Circ{
public:

    double x;
    double y;
    double r;

    Collision_Circ();
    Collision_Circ(double get_x,double get_y,double get_r);

    Collision_Circ operator*(double scalar);
    Collision_Circ operator/(double scalar);
};

class Collision{
public:

    uint32_t object1;
    uint32_t object2;

    Collision();
    Collision(uint32_t get_object1,uint32_t get_object2);

    bool operator==(Collision collision1);
};

bool collision_check_rect(Collision_Rect box_a,Collision_Rect box_b);

bool collision_check_circ(Collision_Circ circle_a,Collision_Circ circle_b);

bool collision_check_circ_rect(Collision_Circ circle,Collision_Rect box);

//Returns a rectangle containing the collision area of the two passed rectangles.
Collision_Rect get_collision_area_rect(Collision_Rect box_a,Collision_Rect box_b);

double get_angle_to_rect(Collision_Rect box_a,Collision_Rect box_b,Collision_Rect camera);
double get_angle_to_rect(Collision_Circ circle,Collision_Rect box,Collision_Rect camera);

double get_angle_to_circ(Collision_Circ circle_a,Collision_Circ circle_b,Collision_Rect camera);
double get_angle_to_circ(Collision_Rect box,Collision_Circ circle,Collision_Rect camera);

#endif
