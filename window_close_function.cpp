#include "window.h"
#include "world.h"

using namespace std;

void Window::exec_close_function(){
    if(close_function.length()>0){
        if(close_function=="configure_command"){
            engine_interface.configure_command=-1;
        }
        else if(close_function=="confirm_delete_selected"){
            informations[1].text="";
        }
        else if(close_function=="rename_file"){
            set_info_text(0,"");
        }
        else{
            message_log.add_error("Invalid close function: '"+close_function+"'");
        }
    }
}
