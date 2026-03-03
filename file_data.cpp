/* Copyright (c) Cheese and Bacon Games */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "file_data.h"

using namespace std;

File_Data::File_Data () {
    exists = false;
    is_readable = false;
    is_symbolic_link = false;
    is_directory = false;
    is_regular_file = false;
}
