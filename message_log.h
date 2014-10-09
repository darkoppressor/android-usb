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
