/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef file_io_h
#define file_io_h

#include <string>
#include <sstream>

#ifdef GAME_OS_ANDROID
    #include <vector>
    #include <stdint.h>
    #include <dirent.h>
    #include <sys/stat.h>
#else
    #include <boost/filesystem.hpp>
#endif

class File_IO_Load{
public:

    bool load_success;
    std::istringstream data;

    File_IO_Load();
    File_IO_Load(std::string path,bool binary=false);

    void load_file(std::string path,bool binary=false);

    bool file_loaded();
    bool eof();

    void getline(std::string* line);
    std::string get_data();
};

#ifdef GAME_OS_ANDROID
    class File_IO{
    public:
        bool save_file(std::string path,std::string data,bool append=false,bool binary=false);

        bool directory_exists(std::string path);
        bool file_exists(std::string path);
        bool is_directory(std::string path);
        bool is_regular_file(std::string path);
        void create_directory(std::string path);
        void remove_file(std::string path);
        void remove_directory(std::string path);
        std::string get_file_name(std::string path);

        bool external_storage_available();
    };

    class File_IO_Directory_Iterator{
    public:

        std::string directory;
        std::vector<std::string> asset_list;
        uint32_t entry;

        File_IO_Directory_Iterator(std::string get_directory);

        bool evaluate();
        void iterate();
        bool is_directory();
        bool is_regular_file();
        std::string get_full_path();
        std::string get_file_name();
    };

    class File_IO_Directory_Iterator_User_Data{
    public:

        std::string directory;
        DIR* dir;
        struct dirent* dir_entry;
        uint32_t entry;
        uint32_t entries;

        File_IO_Directory_Iterator_User_Data(std::string get_directory);
        ~File_IO_Directory_Iterator_User_Data();

        bool evaluate();
        void iterate();
        bool file_exists();
        bool is_regular_file();
        bool is_directory();
        std::string get_full_path();
        std::string get_file_name();
    };
#else
    class File_IO{
    public:
        bool save_file(std::string path,std::string data,bool append=false,bool binary=false);

        bool directory_exists(std::string path);
        bool file_exists(std::string path);
        bool is_directory(std::string path);
        bool is_regular_file(std::string path);
        void create_directory(std::string path);
        void remove_file(std::string path);
        void remove_directory(std::string path);
        std::string get_file_name(std::string path);
    };

    class File_IO_Directory_Iterator{
    public:

        boost::filesystem::directory_iterator it;

        File_IO_Directory_Iterator(std::string get_directory);

        bool evaluate();
        void iterate();
        bool is_directory();
        bool is_regular_file();
        std::string get_full_path();
        std::string get_file_name();
    };
#endif

#endif
