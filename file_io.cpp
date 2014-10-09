#include "file_io.h"
#include "world.h"

#ifdef GAME_OS_ANDROID
    #include <fstream>
    #include <errno.h>

    #include <boost/algorithm/string.hpp>
#endif

using namespace std;

File_IO_Load::File_IO_Load(){
    load_success=false;
    data.str("");
}

File_IO_Load::File_IO_Load(string path,bool binary){
    load_file(path,binary);
}

void File_IO_Load::load_file(string path,bool binary){
    load_success=false;
    string str_data="";

    string rw_mode="r";
    if(binary){
        rw_mode+="b";
    }

    SDL_RWops* rwops=SDL_RWFromFile(path.c_str(),rw_mode.c_str());

    if(rwops!=NULL){
        load_success=true;

        unsigned char* data_chunk=(unsigned char*)malloc(100);

        for(long length=0;(length=SDL_RWread(rwops,data_chunk,1,100))>0;){
            for(long i=0;i<length;i++){
                str_data+=data_chunk[i];
            }
        }

        free(data_chunk);
        SDL_RWclose(rwops);
    }

    data.str(str_data.c_str());
}

bool File_IO_Load::file_loaded(){
    return load_success;
}

bool File_IO_Load::eof(){
    if(data.eof()){
        return true;
    }
    else{
        return false;
    }
}

void File_IO_Load::getline(string* line){
    std::getline(data,*line);
}

string File_IO_Load::get_data(){
    return data.str();
}

#ifdef GAME_OS_ANDROID
    bool File_IO::save_file(string path,string data,bool append,bool binary){
        string rw_mode="w";
        if(append){
            rw_mode="a";
        }
        if(binary){
            rw_mode+="b";
        }

        SDL_RWops* rwops=SDL_RWFromFile(path.c_str(),rw_mode.c_str());

        if(rwops!=NULL){
            const char* data_chars=data.c_str();

            size_t length=SDL_strlen(data_chars);

            size_t written_length=SDL_RWwrite(rwops,data_chars,1,length);

            SDL_RWclose(rwops);

            if(written_length!=length){
                //We assume appending means we are working with the error log.
                //We don't post an error about it failing because that would cause an infinite loop.
                if(!append){
                    string msg="Error saving file '"+path+"': ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);
                }

                return false;
            }
        }
        else{
            //We assume appending means we are working with the error log.
            //We don't post an error about it failing because that would cause an infinite loop.
            if(!append){
                string msg="Error opening file '"+path+"' for saving: ";
                msg+=SDL_GetError();
                message_log.add_error(msg);
            }

            return false;
        }

        return true;
    }

    bool File_IO::directory_exists(string path){
        DIR* dir=0;

        if((dir=opendir(path.c_str()))!=0){
            closedir(dir);

            return true;
        }

        return false;
    }

    bool File_IO::file_exists(string path){
        File_IO_Load load(path);

        if(load.file_loaded()){
            return true;
        }
        else{
            return false;
        }
    }

    bool File_IO::is_directory(string path){
        struct stat file_info;
        string file_name_compare=get_file_name(path);

        if(stat(path.c_str(),&file_info)==0){
            if(file_name_compare!="." && file_name_compare!=".." && S_ISDIR(file_info.st_mode)){
                return true;
            }
        }
        else{
            message_log.add_error("Error getting file information for file '"+path+"': "+string_stuff.num_to_string(errno));
        }

        return false;
    }

    bool File_IO::is_regular_file(string path){
        struct stat file_info;

        if(stat(path.c_str(),&file_info)==0){
            if(S_ISREG(file_info.st_mode)){
                return true;
            }
        }
        else{
            message_log.add_error("Error getting file information for file '"+path+"': "+string_stuff.num_to_string(errno));
        }

        return false;
    }

    void File_IO::create_directory(string path){
        if(!directory_exists(path) && mkdir(path.c_str(),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)!=0){
            message_log.add_error("Error creating directory '"+path+"': "+string_stuff.num_to_string(errno));
        }
    }

    void File_IO::remove_file(string path){
        if(file_exists(path) && remove(path.c_str())!=0){
            message_log.add_error("Error removing file '"+path+"': "+string_stuff.num_to_string(errno));
        }
    }

    void File_IO::remove_directory(string path){
        //Look through all of the files in the directory.
        for(File_IO_Directory_Iterator_User_Data it(path);it.evaluate();it.iterate()){
            //If the file is a directory.
            if(it.is_directory()){
                remove_directory(it.get_full_path());
            }
            else if(it.is_regular_file()){
                remove_file(it.get_full_path());
            }
        }

        if(directory_exists(path)){
            rmdir(path.c_str());
        }
    }

    string File_IO::get_file_name(string path){
        for(int i=0;i<path.size();i++){
            if(path[i]=='/' || path[i]=='\\'){
                path.erase(0,i+1);
                i=0;
            }
        }

        return path;
    }

    bool File_IO::external_storage_available(){
        int external_storage_state=SDL_AndroidGetExternalStorageState();

        if(external_storage_state!=0){
            if((external_storage_state&SDL_ANDROID_EXTERNAL_STORAGE_READ)!=0 &&
               (external_storage_state&SDL_ANDROID_EXTERNAL_STORAGE_WRITE)!=0){
                return true;
            }
            else{
                message_log.add_error("External storage is not RW enabled.");
            }
        }
        else{
            string msg="External storage is unavailable: ";
            msg+=SDL_GetError();
            message_log.add_error(msg);
        }

        return false;
    }

    File_IO_Directory_Iterator::File_IO_Directory_Iterator(string get_directory){
        directory=get_directory;
        entry=0;

        File_IO_Load load(directory+"/asset_list");
        if(load.file_loaded()){
            while(!load.eof()){
                string line="";

                load.getline(&line);

                boost::algorithm::trim(line);

                if(line.length()>0){
                    asset_list.push_back(line);
                }
            }
        }
    }

    bool File_IO_Directory_Iterator::evaluate(){
        if(entry<asset_list.size()){
            return true;
        }
        else{
            return false;
        }
    }

    void File_IO_Directory_Iterator::iterate(){
        entry++;
    }

    bool File_IO_Directory_Iterator::is_directory(){
        return false;
    }

    bool File_IO_Directory_Iterator::is_regular_file(){
        return true;
    }

    string File_IO_Directory_Iterator::get_full_path(){
        return directory+"/"+get_file_name();
    }

    string File_IO_Directory_Iterator::get_file_name(){
        return asset_list[entry];
    }

    File_IO_Directory_Iterator_User_Data::File_IO_Directory_Iterator_User_Data(string get_directory){
        directory=get_directory;

        dir_entry=0;
        entry=0;
        entries=0;

        if((dir=opendir(directory.c_str()))!=0){
            while(dir_entry=readdir(dir)){
                entries++;
            }

            closedir(dir);

            dir=opendir(directory.c_str());

            dir_entry=readdir(dir);
        }
    }

    File_IO_Directory_Iterator_User_Data::~File_IO_Directory_Iterator_User_Data(){
        if(dir!=0){
            closedir(dir);
        }
    }

    bool File_IO_Directory_Iterator_User_Data::evaluate(){
        if(entry<entries && file_exists()){
            return true;
        }
        else{
            return false;
        }
    }

    void File_IO_Directory_Iterator_User_Data::iterate(){
        if(++entry<entries){
            dir_entry=readdir(dir);
        }
    }

    bool File_IO_Directory_Iterator_User_Data::file_exists(){
        if(dir_entry!=0){
            return true;
        }
        else{
            return false;
        }
    }

    bool File_IO_Directory_Iterator_User_Data::is_regular_file(){
        struct stat file_info;

        if(stat(get_full_path().c_str(),&file_info)==0){
            if(S_ISREG(file_info.st_mode)){
                return true;
            }
        }
        else{
            message_log.add_error("Error getting file information for file '"+get_full_path()+"': "+string_stuff.num_to_string(errno));
        }

        return false;
    }

    bool File_IO_Directory_Iterator_User_Data::is_directory(){
        struct stat file_info;
        string file_name_compare=dir_entry->d_name;

        if(stat(get_full_path().c_str(),&file_info)==0){
            if(file_name_compare!="." && file_name_compare!=".." && S_ISDIR(file_info.st_mode)){
                return true;
            }
        }
        else{
            message_log.add_error("Error getting file information for file '"+get_full_path()+"': "+string_stuff.num_to_string(errno));
        }

        return false;
    }

    string File_IO_Directory_Iterator_User_Data::get_full_path(){
        return directory+"/"+get_file_name();
    }

    string File_IO_Directory_Iterator_User_Data::get_file_name(){
        return dir_entry->d_name;
    }
#else
    bool File_IO::save_file(string path,string data,bool append,bool binary){
        string rw_mode="w";
        if(append){
            rw_mode="a";
        }
        if(binary){
            rw_mode+="b";
        }

        SDL_RWops* rwops=SDL_RWFromFile(path.c_str(),rw_mode.c_str());

        if(rwops!=NULL){
            const char* data_chars=data.c_str();

            size_t length=SDL_strlen(data_chars);

            size_t written_length=SDL_RWwrite(rwops,data_chars,1,length);

            SDL_RWclose(rwops);

            if(written_length!=length){
                //We assume appending means we are working with the error log.
                //We don't post an error about it failing because that would cause an infinite loop.
                if(!append){
                    string msg="Error saving file '"+path+"': ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);
                }

                return false;
            }
        }
        else{
            //We assume appending means we are working with the error log.
            //We don't post an error about it failing because that would cause an infinite loop.
            if(!append){
                string msg="Error opening file '"+path+"' for saving: ";
                msg+=SDL_GetError();
                message_log.add_error(msg);
            }

            return false;
        }

        return true;
    }

    bool File_IO::directory_exists(string path){
        return boost::filesystem::exists(path);
    }

    bool File_IO::file_exists(string path){
        return boost::filesystem::exists(path);
    }

    bool File_IO::is_directory(string path){
        return boost::filesystem::is_directory(path);
    }

    bool File_IO::is_regular_file(string path){
        return boost::filesystem::is_regular_file(path);
    }

    void File_IO::create_directory(string path){
        boost::filesystem::create_directory(path);
    }

    void File_IO::remove_file(string path){
        boost::filesystem::remove(path);
    }

    void File_IO::remove_directory(string path){
        boost::filesystem::remove_all(path);
    }

    string File_IO::get_file_name(string path){
        boost::filesystem::path boost_path(path);
        return boost_path.filename().string();
    }

    File_IO_Directory_Iterator::File_IO_Directory_Iterator(string get_directory){
        it=boost::filesystem::directory_iterator(get_directory);
    }

    bool File_IO_Directory_Iterator::evaluate(){
        if(it!=boost::filesystem::directory_iterator()){
            return true;
        }
        else{
            return false;
        }
    }

    void File_IO_Directory_Iterator::iterate(){
        it++;
    }

    bool File_IO_Directory_Iterator::is_directory(){
        if(boost::filesystem::is_directory(it->path())){
            return true;
        }
        else{
            return false;
        }
    }

    bool File_IO_Directory_Iterator::is_regular_file(){
        if(boost::filesystem::is_regular_file(it->path())){
            return true;
        }
        else{
            return false;
        }
    }

    string File_IO_Directory_Iterator::get_full_path(){
        return it->path().string();
    }

    string File_IO_Directory_Iterator::get_file_name(){
        return it->path().filename().string();
    }
#endif
