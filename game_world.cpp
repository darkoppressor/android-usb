#include "game_world.h"
#include "world.h"
#include "render.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Game_World::clear_world(){
    files.clear();
    selected_files.clear();

    starting_path="";

    current_path="";
}

void Game_World::generate_world(){
    clear_world();

    if(!file_io.file_exists(game.option_adb_path)){
        message_log.add_error("Could not find '"+game.option_adb_path+"'");
        engine_interface.quit();
    }

    run_command(game.option_adb_path+" start-server");

    starting_path=run_command(game.option_adb_path+" shell echo "+game.option_starting_path);
    boost::algorithm::trim(starting_path);

    current_path=starting_path;
}

string Game_World::get_current_path(){
    if(current_path=="/"){
        return current_path;
    }
    else{
        return current_path+"/";
    }
}

string Game_World::run_command(string command){
    string result="";

    FILE* pipe=popen(command.c_str(),"r");

    if(pipe){
        char buffer[128];

        while(!feof(pipe)){
            if(fgets(buffer,128,pipe)!=NULL){
                result+=buffer;
            }
        }

        pclose(pipe);
    }

    return result;
}

File_Data Game_World::check_file(string path,bool need_directory){
    File_Data file_data;

    boost::algorithm::trim(path);

    for(int i=0;i<2;i++){
        string error_check="";

        if(i==0){
            error_check=run_command(game.option_adb_path+" shell cd \""+path+"\"");
        }
        else if(i==1){
            error_check=run_command(game.option_adb_path+" shell ls \""+path+"\"");
        }

        ///Does this need checking for non-regular files?
        ///Here is the error message if so:
        ///message_log.add_error("'"+get_current_path()+cd+file+"' is not a valid directory or file");

        if(boost::algorithm::icontains(error_check,"no such file or directory")){
            file_data.exists=false;
        }
        else{
            file_data.exists=true;
        }

        if(boost::algorithm::icontains(error_check,"not a directory")){
            file_data.is_directory=false;
        }
        else{
            file_data.is_directory=true;
        }

        if(boost::algorithm::icontains(error_check,"permission denied")){
            file_data.have_permission=false;
        }
        else{
            file_data.have_permission=true;
        }
    }

    if(!file_data.exists){
        message_log.add_log("'"+path+"': No such file or directory");
    }

    if(!file_data.have_permission){
        message_log.add_log("Permission denied for '"+path+"'");
    }

    if(need_directory && !file_data.is_directory){
        message_log.add_log("'"+path+"' is not a directory");
    }

    return file_data;
}

void Game_World::change_directory(string directory){
    boost::algorithm::trim(directory);

    File_Data file_data=check_file(get_current_path()+directory,true);

    if(file_data.is_good_directory()){
        current_path=run_command(game.option_adb_path+" shell readlink -f \""+get_current_path()+directory+"\"");

        boost::algorithm::trim(current_path);

        engine_interface.get_window("browser")->rebuild_scrolling_buttons();
    }
}

void Game_World::remove_file(string file){
    boost::algorithm::trim(file);

    File_Data file_data=check_file(get_current_path()+file);

    if(file_data.is_good_file() || file_data.is_good_directory()){
        run_command(game.option_adb_path+" shell rm -rf \""+get_current_path()+file+"\"");

        message_log.add_log("Removing '"+get_current_path()+file+"'");
    }
}

void Game_World::select_file(unsigned int index,bool only_select){
    Window* window=engine_interface.get_window("browser");
    if(index>0 && index<window->buttons.size()){
        bool selected=false;
        for(int i=0;i<selected_files.size();i++){
            if(selected_files[i]==index){
                selected=true;
                break;
            }
        }

        if(!selected){
            selected_files.push_back(index);

            window->buttons[index].text="** "+window->buttons[index].text+" **";
        }
        else if(!only_select){
            for(int i=0;i<selected_files.size();i++){
                if(selected_files[i]==index){
                    selected_files.erase(selected_files.begin()+i);
                    i--;
                }
            }

            boost::algorithm::erase_first(window->buttons[index].text,"** ");
            boost::algorithm::erase_first(window->buttons[index].text," **");
        }
    }
}

void Game_World::select_all(){
    for(int i=0;i<files.size();i++){
        select_file(i,true);
    }
}

void Game_World::copy_selected(){
    for(int i=0;i<selected_files.size();i++){
        if(selected_files[i]>0 && selected_files[i]<files.size()){
            adb_pull(files[selected_files[i]]);
        }
    }
}

void Game_World::delete_selected(){
    for(int i=0;i<selected_files.size();i++){
        if(selected_files[i]>0 && selected_files[i]<files.size()){
            remove_file(files[selected_files[i]]);
        }
    }

    engine_interface.get_window("browser")->rebuild_scrolling_buttons();
}

void Game_World::adb_push(string path,string cd){
    boost::algorithm::trim(path);
    boost::algorithm::trim(cd);

    if(file_io.is_directory(path)){
        string file_name=file_io.get_file_name(path);

        run_command(game.option_adb_path+" shell mkdir \""+get_current_path()+cd+file_name+"\"");

        message_log.add_log("Copying '"+path+"' to '"+get_current_path()+cd+"'");

        string new_cd=cd+file_name+"/";

        for(File_IO_Directory_Iterator it(path);it.evaluate();it.iterate()){
            adb_push(it.get_full_path(),new_cd);
        }
    }
    else if(file_io.is_regular_file(path)){
        run_command(game.option_adb_path+" push \""+path+"\" \""+get_current_path()+cd+"\"");

        message_log.add_log("Copying '"+path+"' to '"+get_current_path()+cd+"'");
    }
    else{
        message_log.add_error("'"+path+"' is not a valid directory or file");
    }
}

void Game_World::adb_pull(string file,string cd){
    boost::algorithm::trim(file);
    boost::algorithm::trim(cd);

    File_Data file_data=check_file(get_current_path()+cd+file);

    if(file_data.is_good_file()){
        run_command(game.option_adb_path+" pull \""+get_current_path()+cd+file+"\" "+engine_interface.get_home_directory()+"files/"+cd+file);

        message_log.add_log("Copying '"+get_current_path()+cd+file+"' to '"+engine_interface.get_home_directory()+"files/"+cd+"'");
    }
    else if(file_data.is_good_directory()){
        file_io.create_directory(engine_interface.get_home_directory()+"files/"+cd+file);

        message_log.add_log("Copying '"+get_current_path()+cd+file+"' to '"+engine_interface.get_home_directory()+"files/"+cd+"'");

        vector<string> file_list=get_directory_list(get_current_path()+cd+file,false);

        string new_cd=cd+file+"/";

        for(int i=0;i<file_list.size();i++){
            adb_pull(file_list[i],new_cd);
        }
    }
}

vector<string> Game_World::get_directory_list(string path,bool allow_parent_directory){
    vector<string> file_list;

    string files_string=run_command(game.option_adb_path+" shell ls \""+path+"\"");

    file_list.push_back("");

    for(int i=0;i<files_string.size();i++){
        if(files_string[i]=='\n'){
            file_list.push_back("");
        }
        else{
            file_list[file_list.size()-1]+=files_string[i];
        }
    }

    for(int i=0;i<file_list.size();i++){
        if(file_list[i].length()==0 || file_list[i]=="." || file_list[i]==".."){
            file_list.erase(file_list.begin()+i);
            i--;
        }
    }

    if(allow_parent_directory && path!="/"){
        file_list.insert(file_list.begin(),"..");
    }

    return file_list;
}

void Game_World::tick(){
}

void Game_World::ai(){
}

void Game_World::movement(){
}

void Game_World::events(){
}

void Game_World::animate(){
}

void Game_World::render(){
}

void Game_World::update_background(){
}

void Game_World::render_background(){
    render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,1.0,"ui_black");
}
