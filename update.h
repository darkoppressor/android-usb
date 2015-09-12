/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef update_h
#define update_h

class Update{
public:

	void tick();

    void ai();

    void input();

    void movement();

    void events();

    void animate();

    void camera(int frame_rate,double ms_per_frame,int logic_frame_rate);

    void render(int frame_rate,double ms_per_frame,int logic_frame_rate);
};

#endif
