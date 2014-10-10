#include "file_data.h"

using namespace std;

File_Data::File_Data(){
    exists=false;
    is_readable=false;
    is_symbolic_link=false;
    is_directory=false;
    is_regular_file=false;
}
