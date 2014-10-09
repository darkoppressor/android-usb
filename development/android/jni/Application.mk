
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
APP_STL := gnustl_static

# The 'mips' target doesn't work right now. It fails to compile, complaining of missing headers.
APP_ABI := armeabi armeabi-v7a x86
#APP_ABI := armeabi-v7a

# Release options:
APP_CFLAGS += -w
APP_CFLAGS += -s
APP_CFLAGS += -O2
APP_CFLAGS += -fexpensive-optimizations

# Debug options:
#APP_CFLAGS += -g
#APP_CFLAGS += -Wall
#APP_CFLAGS += -fno-inline
#APP_CFLAGS += -fno-omit-frame-pointer

# Global options:
APP_CFLAGS += -fexceptions
APP_CFLAGS += -DGAME_OS_ANDROID
