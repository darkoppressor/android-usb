/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef server_h
#define server_h

#include <string>

class Server{
public:

    std::string name;
    std::string address;
    unsigned short port;
    std::string password;

    Server();
};

#endif
