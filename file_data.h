#ifndef file_data_h
#define file_data_h

class File_Data{
public:

    bool exists;
    bool is_readable;
    bool is_symbolic_link;
    bool is_directory;
    bool is_regular_file;

    File_Data();
};

#endif
