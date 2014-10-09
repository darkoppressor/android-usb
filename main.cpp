#include "main.h"
#include "world.h"

#ifdef GAME_OS_OSX
    #include <CoreFoundation/CoreFoundation.h>
#endif

using namespace std;

void game_loop(){
    //The maximum number of frames to be skipped.
    int max_frameskip=5;

    double next_game_tick=(double)SDL_GetTicks();
    double next_render_tick=(double)SDL_GetTicks();

    //The number of logic updates that have occurred since the last render.
    int number_of_updates=0;

    Timer timer_frame_rate;
    Timer timer_logic_frame_rate;
    Timer timer_ms_per_frame;
    Timer ms_per_frame_update;

    double ms_per_frame=0.0;

    int frame_count=0;
    int frame_rate=0;

    int logic_frame_count=0;
    int logic_frame_rate=0;

    timer_frame_rate.start();
    timer_logic_frame_rate.start();
    timer_ms_per_frame.start();
    ms_per_frame_update.start();

    while(true){
        if(ms_per_frame_update.get_ticks()>=100){
            ms_per_frame=(double)timer_ms_per_frame.get_ticks();

            ms_per_frame_update.start();
        }
        timer_ms_per_frame.start();

        if(timer_frame_rate.get_ticks()>=1000){
            frame_rate=frame_count;
            frame_count=0;

            timer_frame_rate.start();
        }

        //For our game loop, we first update game logic, and then render. The two are kept separate and independent.

        number_of_updates=0;

        //We consume SKIP_TICKS sized chunks of time, which ultimately translates to updating the logic UPDATE_LIMIT times a second.
        //We also check to see if we've updated logic max_frameskip times without rendering, at which point we render.
        while((double)SDL_GetTicks()>next_game_tick && number_of_updates<max_frameskip){
            if(timer_logic_frame_rate.get_ticks()>=1000){
                logic_frame_rate=logic_frame_count;
                logic_frame_count=0;

                timer_logic_frame_rate.start();
            }
            logic_frame_count++;

            //We are doing another game logic update.
            //If this reaches max_frameskip, we will render.
            number_of_updates++;

            //Clamp the time step to something reasonable.
            if(abs((double)SDL_GetTicks()-next_game_tick)>SKIP_TICKS*2.0){
                next_game_tick=(double)SDL_GetTicks()-SKIP_TICKS*2.0;
            }

            //Consume another SKIP_TICKS sized chunk of time.
            next_game_tick+=SKIP_TICKS;

            //Now we update the game logic:

            if(engine_interface.need_to_reinit){
                engine_interface.need_to_reinit=false;
                main_window.reinitialize();
            }

            engine_interface.rebuild_window_data();

            network.receive_packets();

            update.ai();

            update.input();

            network.send_updates();
            network.send_input();

            update.tick();

            update.movement();

            update.events();

            update.animate();

            update.camera(frame_rate,ms_per_frame,logic_frame_rate);
        }

        //Now that we've handled logic updates, we do our rendering.

        if((double)SDL_GetTicks()>next_render_tick){
            frame_count++;

            if(abs((double)SDL_GetTicks()-next_render_tick)>SKIP_TICKS_RENDER*2.0){
                next_render_tick=(double)SDL_GetTicks()-SKIP_TICKS_RENDER*2.0;
            }

            next_render_tick+=SKIP_TICKS_RENDER;

            update.render(frame_rate,ms_per_frame,logic_frame_rate);
        }
    }
}

int handle_app_events(void* userdata,SDL_Event* event){
    switch(event->type){
    case SDL_APP_TERMINATING:
        message_log.add_error("The OS is terminating this application, shutting down...");

        engine_interface.quit();
        return 0;
    default:
        return 1;
    }
}

int main(int argc,char* args[]){
    #ifdef GAME_OS_OSX
        //Set the working directory to the Resources directory of our bundle.
        char path[PATH_MAX];
        CFURLRef url=CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
        CFURLGetFileSystemRepresentation(url,true,(uint8_t*)path,PATH_MAX);
        CFRelease(url);
        chdir(path);
    #endif

    if(!main_window.init_sdl()){
        return 1;
    }

    #ifdef GAME_OS_ANDROID
        if(!file_io.external_storage_available()){
            return 2;
        }
    #endif

    if(!engine_interface.load_data_engine()){
        return 3;
    }

    if(!engine_interface.load_save_location()){
        return 4;
    }

    engine_interface.make_directories();

    message_log.clear_error_log();

    int things_loaded=0;
    int things_to_load=1;

    if(!load_world()){
        return 5;
    }
    engine_interface.render_loading_screen((double)++things_loaded/(double)things_to_load,"Initializing");

    game.start();

    for(int i=0;i<engine_interface.starting_windows.size();i++){
        engine_interface.get_window(engine_interface.starting_windows[i])->toggle_on(true,true);
    }

    SDL_SetEventFilter(handle_app_events,NULL);

    SDL_EventState(SDL_DROPFILE,SDL_ENABLE);

    game_loop();

    return 0;
}
