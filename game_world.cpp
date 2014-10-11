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

    ///Maybe I should add something around here to help with initially connecting to the device in adb
    ///What if multiple devices are connected?

    run_command(game.option_adb_path+" start-server");

    starting_path=run_command(game.option_adb_path+" shell echo "+game.option_starting_path);
    boost::algorithm::trim(starting_path);

    if(starting_path.length()>0 && starting_path[0]=='/'){
        starting_path.erase(starting_path.begin());
    }

    current_path="/";

    change_directory(starting_path,false);
}

string Game_World::get_current_path(){
    if(current_path=="/"){
        return current_path;
    }
    else{
        return current_path+"/";
    }
}

string Game_World::path_to_filename(string path){
    boost::algorithm::trim(path);

    for(int i=0;i<path.size();i++){
        if(path[i]=='/'){
            path.erase(0,i+1);
            i=0;
        }
    }

    return path;
}

string Game_World::directory_up(string path){
    boost::algorithm::trim(path);

    for(int i=path.size()-1;i>0;i--){
        bool slash=false;
        if(path[i]=='/'){
            slash=true;
        }

        path.erase(path.begin()+i);
        if(i<path.size()){
            i++;
        }

        if(slash){
            break;
        }
    }

    return path;
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

File_Data Game_World::check_file(string path){
    File_Data file_data;

    boost::algorithm::trim(path);

    if(boost::algorithm::contains(run_command(game.option_adb_path+" shell if [ -e \""+path+"\" ]; then echo \"yeppers!\"; fi"),"yeppers!")){
        file_data.exists=true;

        if(boost::algorithm::contains(run_command(game.option_adb_path+" shell if [ -r \""+path+"\" ]; then echo \"yeppers!\"; fi"),"yeppers!")){
            file_data.is_readable=true;

            if(boost::algorithm::contains(run_command(game.option_adb_path+" shell if [ -h \""+path+"\" ]; then echo \"yeppers!\"; fi"),"yeppers!")){
                file_data.is_symbolic_link=true;
            }

            if(boost::algorithm::contains(run_command(game.option_adb_path+" shell if [ -d \""+path+"\" ]; then echo \"yeppers!\"; fi"),"yeppers!")){
                file_data.is_directory=true;
            }

            if(boost::algorithm::contains(run_command(game.option_adb_path+" shell if [ -f \""+path+"\" ]; then echo \"yeppers!\"; fi"),"yeppers!")){
                file_data.is_regular_file=true;
            }
        }
        else{
            message_log.add_log("'"+path+"' is not readable (permission denied)");
        }
    }
    else{
        message_log.add_log("'"+path+"' does not exist");
    }

    return file_data;
}

void Game_World::change_directory(string directory,bool rebuild){
    boost::algorithm::trim(directory);

    if(directory.length()>0){
        string path=get_current_path()+directory;

        File_Data file_data=check_file(path);

        if(file_data.exists && file_data.is_readable){
            if(file_data.is_directory){
                current_path=run_command(game.option_adb_path+" shell readlink -f \""+path+"\"");

                boost::algorithm::trim(current_path);

                if(rebuild){
                    engine_interface.get_window("browser")->rebuild_scrolling_buttons();
                }
            }
            else{
                message_log.add_log("'"+path+"' is not a directory");
            }
        }
    }
}

bool Game_World::remove_file(string path){
    boost::algorithm::trim(path);

    string file=path_to_filename(path);

    if(path.length()>0 && file!="." && file!=".."){
        File_Data file_data=check_file(path);

        if(file_data.exists && file_data.is_readable){
            if(file_data.is_regular_file || file_data.is_directory){
                run_command(game.option_adb_path+" shell rm -rf \""+path+"\"");

                message_log.add_log("Removing '"+path+"'");

                return true;
            }
            else{
                message_log.add_log("'"+path+"' is not a valid file or directory");
            }
        }
    }
    else{
        message_log.add_log("Failed to remove '"+path+"' (invalid filename)");
    }

    return false;
}

bool Game_World::is_selected(string path){
    boost::algorithm::trim(path);

    for(int i=0;i<selected_files.size();i++){
        if(selected_files[i]==path){
            return true;
        }
    }

    return false;
}

void Game_World::update_selected_buttons(){
    Window* window=engine_interface.get_window("browser");

    for(int i=0;i<window->buttons.size();i++){
        boost::algorithm::erase_first(window->buttons[i].text,"** ");
        boost::algorithm::erase_first(window->buttons[i].text," **");

        if(is_selected(get_current_path()+window->buttons[i].text)){
            window->buttons[i].text="** "+window->buttons[i].text+" **";
        }
    }
}

void Game_World::select_file(string path,bool only_select){
    boost::algorithm::trim(path);

    string file=path_to_filename(path);

    if(path.length()>0 && file!="." && file!=".."){
        bool selected=false;
        for(int i=0;i<selected_files.size();i++){
            if(selected_files[i]==path){
                selected=true;
                break;
            }
        }

        if(!selected){
            selected_files.push_back(path);

            update_selected_buttons();
        }
        else if(!only_select){
            for(int i=0;i<selected_files.size();i++){
                if(selected_files[i]==path){
                    selected_files.erase(selected_files.begin()+i);
                    i--;
                }
            }

            update_selected_buttons();
        }
    }
    else{
        message_log.add_log("Failed to select '"+path+"' (invalid filename)");
    }
}

void Game_World::select_all(){
    for(int i=0;i<files.size();i++){
        select_file(get_current_path()+files[i],true);
    }
}

void Game_World::deselect_all(){
    selected_files.clear();

    update_selected_buttons();
}

void Game_World::rename_file(){
    bool file_renamed=false;

    if(selected_files.size()==1){
        string path=selected_files[0];
        string filename=path_to_filename(path);

        string path_to_parent=path;
        path_to_parent.erase(path_to_parent.size()-filename.size(),filename.size());

        string filename_new=engine_interface.get_window("rename_file")->get_info_text(0);

        boost::algorithm::trim(path);
        boost::algorithm::trim(path_to_parent);
        boost::algorithm::trim(filename);
        boost::algorithm::trim(filename_new);

        if(filename_new.length()>0 && filename_new!="." && filename_new!=".."){
            File_Data file_data=check_file(path);

            if(file_data.exists && file_data.is_readable){
                run_command(game.option_adb_path+" shell mv \""+path+"\" \""+path_to_parent+filename_new+"\"");

                file_renamed=true;

                message_log.add_log("Renaming '"+path+"' to '"+path_to_parent+filename_new+"'");
            }
        }
        else{
            message_log.add_log("Failed to rename '"+path+"' (invalid filename)");
        }
    }
    else if(selected_files.size()==0){
        message_log.add_log("Failed to rename file (no file selected)");
    }
    else{
        message_log.add_log("Cannot rename multiple files at once");
    }

    selected_files.clear();

    if(file_renamed){
        engine_interface.get_window("browser")->rebuild_scrolling_buttons();
    }
    else{
        update_selected_buttons();
    }
}

void Game_World::create_directory(){
    string path=get_current_path();
    string directory=engine_interface.get_window("create_directory")->get_info_text(0);

    boost::algorithm::trim(path);
    boost::algorithm::trim(directory);

    if(directory.length()>0 && directory!="." && directory!=".."){
        run_command(game.option_adb_path+" shell mkdir \""+path+directory+"\"");

        engine_interface.get_window("browser")->rebuild_scrolling_buttons();

        message_log.add_log("Creating directory '"+path+directory+"'");
    }
    else{
        message_log.add_log("Failed to create directory '"+path+directory+"' (invalid filename)");
    }
}

void Game_World::copy_selected(){
    for(int i=0;i<selected_files.size();i++){
        string file=path_to_filename(selected_files[i]);
        string path=selected_files[i];
        path.erase(path.size()-file.size(),file.size());

        adb_pull(path,file);
    }

    deselect_all();
}

void Game_World::copy_selected_on_device(){
    bool file_copied=false;

    for(int i=0;i<selected_files.size();i++){
        string path=selected_files[i];
        string filename=path_to_filename(selected_files[i]);

        boost::algorithm::trim(path);
        boost::algorithm::trim(filename);

        if(filename.length()>0 && filename!="." && filename!=".."){
            File_Data file_data=check_file(path);

            if(file_data.exists && file_data.is_readable){
                run_command(game.option_adb_path+" shell cp -r \""+path+"\" \""+get_current_path()+"\"");

                file_copied=true;

                message_log.add_log("Copying '"+path+"' to '"+get_current_path()+"'");
            }
        }
        else{
            message_log.add_log("Failed to copy '"+path+"' (invalid filename)");
        }
    }

    selected_files.clear();

    if(file_copied){
        engine_interface.get_window("browser")->rebuild_scrolling_buttons();
    }
    else{
        update_selected_buttons();
    }
}

void Game_World::move_files(){
    bool file_moved=false;

    for(int i=0;i<selected_files.size();i++){
        string path=selected_files[i];
        string filename=path_to_filename(selected_files[i]);

        boost::algorithm::trim(path);
        boost::algorithm::trim(filename);

        if(filename.length()>0 && filename!="." && filename!=".."){
            File_Data file_data=check_file(path);

            if(file_data.exists && file_data.is_readable){
                run_command(game.option_adb_path+" shell mv \""+path+"\" \""+get_current_path()+"\"");

                file_moved=true;

                message_log.add_log("Moving '"+path+"' to '"+get_current_path()+"'");
            }
        }
        else{
            message_log.add_log("Failed to move '"+path+"' (invalid filename)");
        }
    }

    selected_files.clear();

    if(file_moved){
        engine_interface.get_window("browser")->rebuild_scrolling_buttons();
    }
    else{
        update_selected_buttons();
    }
}

void Game_World::delete_selected(){
    bool some_file_removed=false;

    for(int i=0;i<selected_files.size();i++){
        if(remove_file(selected_files[i])){
            some_file_removed=true;
        }
    }

    selected_files.clear();

    if(some_file_removed){
        engine_interface.get_window("browser")->rebuild_scrolling_buttons();
    }
    else{
        update_selected_buttons();
    }
}

void Game_World::adb_push(string path,string cd){
    boost::algorithm::trim(path);
    boost::algorithm::trim(cd);

    if(file_io.is_directory(path)){
        string file_name=file_io.get_file_name(path);

        run_command(game.option_adb_path+" shell mkdir \""+get_current_path()+cd+file_name+"\"");

        message_log.add_log("Pushing '"+path+"' to '"+get_current_path()+cd+"'");

        string new_cd=cd+file_name+"/";

        for(File_IO_Directory_Iterator it(path);it.evaluate();it.iterate()){
            adb_push(it.get_full_path(),new_cd);
        }
    }
    else if(file_io.is_regular_file(path)){
        run_command(game.option_adb_path+" push \""+path+"\" \""+get_current_path()+cd+"\"");

        message_log.add_log("Pushing '"+path+"' to '"+get_current_path()+cd+"'");
    }
    else{
        message_log.add_error("'"+path+"' is not a valid directory or file");
    }
}

void Game_World::adb_pull(string starting_path,string file,string cd){
    boost::algorithm::trim(starting_path);
    boost::algorithm::trim(file);

    if(starting_path.length()>0 && file.length()>0 && file!="." && file!=".."){
        string path=starting_path+cd+file;

        File_Data file_data=check_file(path);

        if(file_data.exists && file_data.is_readable){
            if(file_data.is_directory){
                file_io.create_directory(engine_interface.get_home_directory()+"files/"+cd+file);

                message_log.add_log("Pulling '"+path+"' to '"+engine_interface.get_home_directory()+"files/"+cd+"'");

                vector<string> file_list=get_directory_list(path,false);

                string new_cd=cd+file+"/";

                for(int i=0;i<file_list.size();i++){
                    adb_pull(starting_path,file_list[i],new_cd);
                }
            }
            else if(file_data.is_regular_file){
                run_command(game.option_adb_path+" pull \""+path+"\" \""+engine_interface.get_home_directory()+"files/"+cd+file+"\"");

                message_log.add_log("Pulling '"+path+"' to '"+engine_interface.get_home_directory()+"files/"+cd+"'");
            }
        }
    }
    else{
        message_log.add_log("Failed to pull '"+starting_path+cd+file+"' (invalid filename)");
    }
}

vector<string> Game_World::get_directory_list(string path,bool allow_parent_directory){
    vector<string> file_list;

    File_Data file_data=check_file(path);

    if(file_data.exists && file_data.is_readable && file_data.is_directory){
        string flags="";
        if(game.option_hidden_files){
            flags="-a";
        }

        string files_string=run_command(game.option_adb_path+" shell ls "+flags+" \""+path+"\"");

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
    else{
        message_log.add_log("Error reading '"+path+"', moving up a directory");

        current_path=directory_up(path);
        return get_directory_list(current_path,allow_parent_directory);
    }
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
