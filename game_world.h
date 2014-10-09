#ifndef game_world_h
#define game_world_h

#include "file_data.h"

#include <vector>
#include <string>

class Game_World{
public:

    std::vector<std::string> files;
    std::vector<unsigned int> selected_files;

    std::string starting_path;

    std::string current_path;

    void clear_world();
    void generate_world();

    std::string get_current_path();
    std::string run_command(std::string command);
    File_Data check_file(std::string path,bool need_directory=false);
    void change_directory(std::string directory);
    void remove_file(std::string file);
    void select_file(unsigned int index,bool only_select=false);
    void select_all();
    void copy_selected();
    void delete_selected();
    void adb_push(std::string path,std::string cd="");
    void adb_pull(std::string file,std::string cd="");
    std::vector<std::string> get_directory_list(std::string path,bool allow_parent_directory);

    void tick();
    void ai();
    void movement();
    void events();
    void animate();
    void render();

    void update_background();
    void render_background();
};

#endif
