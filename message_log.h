/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef message_log_h
#define message_log_h

#include <string>

class Message_Log{
public:

    void clear_error_log();

    void add_error(std::string message);

    void add_log(std::string message);
};

#endif
