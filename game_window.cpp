#include "game_window.h"
#include "world.h"
#include "render.h"

#include <time.h>
#include <string>

#include <SDL_image.h>

#include <boost/algorithm/string.hpp>

using namespace std;

Game_Window::Game_Window(){
    SCREEN_WIDTH=0;
    SCREEN_HEIGHT=0;

    screen=0;

    renderer=0;

    icon=0;
    icon_colorkey=0;
}

void Game_Window::cleanup_video(){
    if(renderer!=0){
        SDL_DestroyRenderer(renderer);
        renderer=0;
    }

    if(screen!=0){
        SDL_DestroyWindow(screen);
        screen=0;
    }
}

bool Game_Window::set_resolution(short* desired_resolution_x,short* desired_resolution_y){
    if(engine_interface.resolution_mode=="fixed"){
        SCREEN_WIDTH=engine_interface.logical_screen_width;
        SCREEN_HEIGHT=engine_interface.logical_screen_height;

        *desired_resolution_x=SCREEN_WIDTH;
        *desired_resolution_y=SCREEN_HEIGHT;
    }
    else if(engine_interface.resolution_mode=="scaling"){
        SCREEN_WIDTH=engine_interface.logical_screen_width;
        SCREEN_HEIGHT=engine_interface.logical_screen_height;

        *desired_resolution_x=engine_interface.option_screen_width;
        *desired_resolution_y=engine_interface.option_screen_height;
    }
    else if(engine_interface.resolution_mode=="standard"){
        SCREEN_WIDTH=engine_interface.option_screen_width;
        SCREEN_HEIGHT=engine_interface.option_screen_height;

        *desired_resolution_x=SCREEN_WIDTH;
        *desired_resolution_y=SCREEN_HEIGHT;
    }
    else{
        message_log.add_error("Invalid value for resolution_mode: '"+engine_interface.resolution_mode+"'");
        return false;
    }

    return true;
}

bool Game_Window::initialize_video(){
    short desired_resolution_x=0;
    short desired_resolution_y=0;

    if(!set_resolution(&desired_resolution_x,&desired_resolution_y)){
        return false;
    }

    //Set up the screen:
    uint32_t flags=0;

    if(engine_interface.option_bind_cursor){
        flags=SDL_WINDOW_INPUT_GRABBED;
    }

    if(engine_interface.option_fullscreen){
        if(engine_interface.option_fullscreen_mode=="standard"){
            screen=SDL_CreateWindow(engine_interface.game_title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                    0,0,flags|SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else if(engine_interface.option_fullscreen_mode=="windowed"){
            screen=SDL_CreateWindow(engine_interface.game_title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                    desired_resolution_x,desired_resolution_y,flags|SDL_WINDOW_BORDERLESS);
        }
        else{
            message_log.add_error("Invalid value for fullscreen_mode: '"+engine_interface.option_fullscreen_mode+"'");
            return false;
        }
    }
    else{
        screen=SDL_CreateWindow(engine_interface.game_title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                desired_resolution_x,desired_resolution_y,flags);
    }

    //If the screen could not be set up.
    if(screen==0){
        string msg="Unable to create window: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    renderer=SDL_CreateRenderer(screen,-1,SDL_RENDERER_TARGETTEXTURE);
    if(renderer==0){
        string msg="Unable to create renderer: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    SDL_RenderSetLogicalSize(renderer,SCREEN_WIDTH,SCREEN_HEIGHT);

    if(!SDL_RenderTargetSupported(renderer)){
        message_log.add_error("Render targets unsupported by renderer.");
        return false;
    }

    //Create a temporary texture to check for the rendering features we need.
    SDL_Texture* texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,10,10);
    if(texture==0){
        string msg="Unable to check renderer features: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    int support_alpha_blending=SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
    if(support_alpha_blending!=0){
        string msg="Alpha blending unsupported by renderer: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    int support_alpha_mod=SDL_SetTextureAlphaMod(texture,128);
    if(support_alpha_mod!=0){
        string msg="Alpha modding unsupported by renderer: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    int support_color_mod=SDL_SetTextureColorMod(texture,128,128,128);
    if(support_color_mod!=0){
        string msg="Color modding unsupported by renderer: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    SDL_DestroyTexture(texture);

    game.reset_camera_dimensions();

    return true;
}

bool Game_Window::init_sdl(){
    if(SDL_Init(SDL_INIT_EVERYTHING)!=0){
        string msg="Unable to init SDL: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
        return false;
    }

    CURRENT_WORKING_DIRECTORY=engine_interface.get_cwd();
    CHECKSUM=engine_interface.get_checksum();

    return true;
}

void Game_Window::set_sdl_hints(){
    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"opengl");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,string_stuff.num_to_string((int)engine_interface.option_vsync).c_str());
}

bool Game_Window::init(){
    string msg="";

    set_sdl_hints();

    //Set the window icon.
    msg="data/images/icons/standard.bmp";
    icon=SDL_LoadBMP(msg.c_str());

    if(icon==0){
        msg="Unable to load icon: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    icon_colorkey=SDL_MapRGB(icon->format,0,0,0);
    if(SDL_SetColorKey(icon,SDL_TRUE,icon_colorkey)!=0){
        msg="Unable to set icon color key: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    SDL_SetWindowIcon(screen,icon);

    if(!initialize_video()){
        return false;
    }

    int actual_width=0;
    int actual_height=0;
    SDL_GetRendererOutputSize(renderer,&actual_width,&actual_height);
    engine_interface.touch_controller.scale(actual_width,actual_height);

    if(Mix_Init(MIX_INIT_OGG)==0){
        msg="SDL2_mixer initialization failed: ";
        msg+=Mix_GetError();
        message_log.add_error(msg);
    }
    else{
        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024)==-1){
            msg="SDL2_mixer failed to open mixer: ";
            msg+=Mix_GetError();
            message_log.add_error(msg);
        }
        else{
            int channels_requested=2048;
            int channels_allocated=Mix_AllocateChannels(channels_requested);
            if(channels_allocated!=channels_requested){
                msg="Error allocating mixer channels: Requested "+string_stuff.num_to_string(channels_requested)+", allocated "+string_stuff.num_to_string(channels_allocated)+".";
                message_log.add_error(msg);
            }
        }
    }

    if(IMG_Init(IMG_INIT_PNG)==0){
        msg="SDL2_image initialization failed: ";
        msg+=IMG_GetError();
        message_log.add_error(msg);
        return false;
    }

    if(SDL_GameControllerAddMappingsFromFile("data/game_controller_db")==-1){
        msg="Error loading game controller database: ";
        msg+=SDL_GetError();
        message_log.add_error(msg);
    }

    for(int i=0;i<SDL_NumJoysticks();i++){
        if(SDL_IsGameController(i)){
            if(engine_interface.option_accelerometer_controller || !boost::algorithm::icontains(SDL_JoystickNameForIndex(i),"accelerometer")){
                engine_interface.controllers.push_back(Controller(SDL_GameControllerOpen(i)));

                Controller* controller_object=&engine_interface.controllers[engine_interface.controllers.size()-1];
                SDL_GameController* controller=controller_object->controller;

                if(controller!=0){
                    controller_object->instance_id=SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));

                    if(SDL_JoystickIsHaptic(SDL_GameControllerGetJoystick(controller))){
                        controller_object->haptic=SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));

                        if(controller_object->haptic!=0){
                            if(SDL_HapticRumbleSupported(controller_object->haptic)){
                                if(SDL_HapticRumbleInit(controller_object->haptic)!=0){
                                    msg="Error initializing rumble for haptic on controller "+string_stuff.num_to_string(i)+": ";
                                    msg+=SDL_GetError();
                                    message_log.add_error(msg);

                                    SDL_HapticClose(controller_object->haptic);
                                    controller_object->haptic=0;
                                }
                            }
                        }
                        else{
                            msg="Error opening haptic for controller "+string_stuff.num_to_string(i)+": ";
                            msg+=SDL_GetError();
                            message_log.add_error(msg);
                        }
                    }
                }
                else{
                    msg="Error opening controller "+string_stuff.num_to_string(i)+": ";
                    msg+=SDL_GetError();
                    message_log.add_error(msg);

                    engine_interface.controllers.pop_back();
                }
            }
        }
        else{
            SDL_Joystick* joystick=0;

            if((joystick=SDL_JoystickOpen(i))!=0){
                string joystick_name=SDL_JoystickName(joystick);

                char ch_joystick_guid[64];
                SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick),ch_joystick_guid,64);
                string joystick_guid=ch_joystick_guid;

                string joystick_axes=string_stuff.num_to_string(SDL_JoystickNumAxes(joystick));
                string joystick_balls=string_stuff.num_to_string(SDL_JoystickNumBalls(joystick));
                string joystick_buttons=string_stuff.num_to_string(SDL_JoystickNumButtons(joystick));
                string joystick_hats=string_stuff.num_to_string(SDL_JoystickNumHats(joystick));

                SDL_JoystickClose(joystick);

                msg="Joystick detected, but not supported by the game controller interface:\n";
                msg+="Name: "+joystick_name+"\n";
                msg+="GUID: "+joystick_guid+"\n";
                msg+="Number of Axis Controls: "+joystick_axes+"\n";
                msg+="Number of Trackballs: "+joystick_balls+"\n";
                msg+="Number of Buttons: "+joystick_buttons+"\n";
                msg+="Number of POV Hats: "+joystick_hats+"\n";

                message_log.add_error(msg);
            }
            else{
                message_log.add_error("Joystick detected, but not supported by the game controller interface.");
            }
        }
    }

    if(engine_interface.option_touch_controller && SDL_GetNumTouchDevices()>0){
        engine_interface.touch_controls=true;
    }

    return true;
}

void Game_Window::reinitialize(){
    short desired_resolution_x=0;
    short desired_resolution_y=0;

    if(set_resolution(&desired_resolution_x,&desired_resolution_y)){
        int toggle_fullscreen=0;

        if(engine_interface.option_fullscreen){
            if(engine_interface.option_fullscreen_mode=="standard"){
                toggle_fullscreen=SDL_SetWindowFullscreen(screen,SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            else if(engine_interface.option_fullscreen_mode=="windowed"){
                toggle_fullscreen=SDL_SetWindowFullscreen(screen,0);
                SDL_SetWindowBordered(screen,SDL_FALSE);
                SDL_SetWindowSize(screen,desired_resolution_x,desired_resolution_y);
                SDL_SetWindowPosition(screen,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
            }
        }
        else{
            toggle_fullscreen=SDL_SetWindowFullscreen(screen,0);
            SDL_SetWindowBordered(screen,SDL_TRUE);
            SDL_SetWindowSize(screen,desired_resolution_x,desired_resolution_y);
            SDL_SetWindowPosition(screen,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
        }

        if(toggle_fullscreen!=0){
            string msg="Error toggling fullscreen: ";
            msg+=SDL_GetError();
            message_log.add_error(msg);
        }
        else{
            int actual_width=0;
            int actual_height=0;
            SDL_GetRendererOutputSize(renderer,&actual_width,&actual_height);

            engine_interface.touch_controller.scale(actual_width,actual_height);
        }
    }
}

void Game_Window::screenshot(){
    #ifdef GAME_OS_ANDROID
        message_log.add_error("Sorry, screenshots are disabled in Android, due to them exploding. Please use Android's own screenshot feature.");
        return;
    #endif

    string screenshot_name=engine_interface.get_home_directory()+"screenshots/";
    screenshot_name+=engine_interface.get_timestamp(true,true,true);
    screenshot_name+=".png";

    int actual_width=0;
    int actual_height=0;
    SDL_GetRendererOutputSize(renderer,&actual_width,&actual_height);

    unsigned char* pixel_data=(unsigned char*)malloc(4*actual_width*actual_height);

    if(pixel_data!=0){
        if(SDL_RenderReadPixels(renderer,NULL,SDL_PIXELFORMAT_ABGR8888,pixel_data,actual_width*4)!=0){
            string msg="Error reading renderer pixels: ";
            msg+=SDL_GetError();
            message_log.add_error(msg);
        }

        uint32_t rmask,gmask,bmask,amask;
        engine_interface.get_rgba_masks(&rmask,&gmask,&bmask,&amask);

        SDL_Surface* surface=SDL_CreateRGBSurfaceFrom(pixel_data,actual_width,actual_height,32,actual_width*4,rmask,gmask,bmask,amask);

        if(surface==0){
            string msg="Error creating surface for screenshot: ";
            msg+=SDL_GetError();
            message_log.add_error(msg);
        }
        else{
            if(IMG_SavePNG(surface,screenshot_name.c_str())!=0){
                string msg="Error saving screenshot: ";
                msg+=IMG_GetError();
                message_log.add_error(msg);
            }

            SDL_FreeSurface(surface);
        }

        free(pixel_data);
    }
    else{
        message_log.add_error("Error allocating memory for screenshot.");
    }
}
