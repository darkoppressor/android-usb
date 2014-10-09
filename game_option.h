#ifndef game_option_h
#define game_option_h

#include <string>

class Game_Option{
public:

    std::string name;
    std::string description;

    Game_Option();

    std::string get_value();

    void set_value(std::string new_value);
};

#endif
