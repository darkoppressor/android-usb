#include "file_data.h"

using namespace std;

File_Data::File_Data(){
    is_directory=false;
    have_permission=false;
    exists=false;
}

bool File_Data::is_good_directory(){
    if(exists && have_permission && is_directory){
        return true;
    }
    else{
        return false;
    }
}

bool File_Data::is_good_file(){
    if(exists && have_permission && !is_directory){
        return true;
    }
    else{
        return false;
    }
}
