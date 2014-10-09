#ifndef screen_shake_h
#define screen_shake_h

class Screen_Shake{
public:

    //Valid range: 0.001-0.999
    double magnitude;
    //Length of the shake in milliseconds
    int length;

    double direction;
    double x;
    double y;
    double distance_moved;
    bool returning;

    double camera_real_x;
    double camera_real_y;

    Screen_Shake();

    void reset();

    bool is_active();

    void change_direction();

    void add_shake(double get_magnitude,int get_length);

    void movement();
};

#endif
