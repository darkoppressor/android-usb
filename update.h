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
