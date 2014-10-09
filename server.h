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
