LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2
SDL_IMAGE_PATH := ../SDL2_image
SDL_MIXER_PATH := ../SDL2_mixer

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
$(LOCAL_PATH)/$(SDL_IMAGE_PATH) \
$(LOCAL_PATH)/$(SDL_MIXER_PATH) \
C:/Development/c++/boost \
C:/Development/c++/android/raknet

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
../../../../animation.cpp \
../../../../button.cpp \
../../../../button_events.cpp \
../../../../collision.cpp \
../../../../color.cpp \
../../../../color_theme.cpp \
../../../../console.cpp \
../../../../console_commands.cpp \
../../../../coords.cpp \
../../../../cursor.cpp \
../../../../dev_info.cpp \
../../../../engine_interface.cpp \
../../../../engine_interface_options.cpp \
../../../../file_io.cpp \
../../../../font.cpp \
../../../../game.cpp \
../../../../game_command.cpp \
../../../../game_constant.cpp \
../../../../game_input.cpp \
../../../../game_option.cpp \
../../../../game_window.cpp \
../../../../game_world.cpp \
../../../../image.cpp \
../../../../image_data.cpp \
../../../../information.cpp \
../../../../main.cpp \
../../../../math.cpp \
../../../../message_log.cpp \
../../../../music.cpp \
../../../../pixels.cpp \
../../../../render.cpp \
../../../../rng.cpp \
../../../../rtt_data.cpp \
../../../../rtt_manager.cpp \
../../../../sorting.cpp \
../../../../sound.cpp \
../../../../sound_data.cpp \
../../../../special_info.cpp \
../../../../sprite.cpp \
../../../../string_stuff.cpp \
../../../../timer.cpp \
../../../../toast.cpp \
../../../../tooltip.cpp \
../../../../touch_controller.cpp \
../../../../update.cpp \
../../../../vector.cpp \
../../../../window.cpp \
../../../../window_close_function.cpp \
../../../../window_scrolling_buttons.cpp \
../../../../world.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer RakNet

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
