/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

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
