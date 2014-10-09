#ifndef file_data_h
#define file_data_h

class File_Data{
public:

    bool is_directory;
    bool have_permission;
    bool exists;

    File_Data();

    bool is_good_directory();
    bool is_good_file();
};

#endif
