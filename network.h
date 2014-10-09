#ifndef network_h
#define network_h

#include "server.h"
#include "timer.h"

#include "raknet/Source/RakPeerInterface.h"
#include "raknet/Source/RakNetVersion.h"
#include "raknet/Source/BitStream.h"
#include "raknet/Source/RakString.h"
#include "raknet/Source/GetTime.h"
#include "raknet/Source/MessageIdentifiers.h"
#include "raknet/Source/RakNetStatistics.h"

#include <string>
#include <vector>

enum{
    ID_GAME_VERSION=ID_USER_PACKET_ENUM,
    ID_GAME_CLIENT_DATA,
    ID_GAME_INITIAL_GAME_DATA,
    ID_GAME_CONNECTED,
    ID_GAME_CLIENT_LIST,
    ID_GAME_CHAT_MESSAGE,
    ID_GAME_UPDATE,
    ID_GAME_INPUT,
    ID_GAME_PING_LIST,
    ID_GAME_PAUSED,
    ID_GAME_SOUND
};

enum{
    ORDERING_CHANNEL_UNORDERED,
    ORDERING_CHANNEL_CONNECTION,
    ORDERING_CHANNEL_CLIENT_DATA,
    ORDERING_CHANNEL_CHAT,
    ORDERING_CHANNEL_INPUT,
    ORDERING_CHANNEL_PAUSED,
    ORDERING_CHANNEL_SOUND
};

class Client_Data{
public:

    RakNet::RakNetGUID id;
    RakNet::SystemAddress address;

    bool connected;
    std::string name;
    int ping;
    bool is_us;

    uint32_t rate_bytes;
    uint32_t rate_updates;

    //The number of bytes sent to this client over the duration of the current second.
    uint32_t bytes_this_second;
    //The number of updates sent to this client over the duration of the current second.
    uint32_t updates_this_second;
    uint32_t counter_update;

    std::vector<std::string> command_buffer;
    std::vector<std::string> command_states;

    Client_Data(RakNet::RakNetGUID get_id,RakNet::SystemAddress get_address,std::string get_name,bool get_is_us);

    bool game_command_state(std::string command);
};

class Network{
public:

    std::string status;
    RakNet::RakPeerInterface* peer;
    RakNet::RakNetGUID id;
    RakNet::SystemAddress address;
    RakNet::Packet* packet;
    std::vector<Client_Data> clients;
    //Keeps track of each passing second and resets client update counts (server) or command packet counts (client).
    Timer timer_tick;
    uint64_t stat_bytes_received;
    uint64_t stat_counter_bytes_received;
    uint64_t stat_bytes_sent;
    uint64_t stat_counter_bytes_sent;

    // Server //

    unsigned int max_clients;
    unsigned short port;
    std::string password;
    bool frequent_connection_protection;
    bool ignore_checksum;
    uint32_t rate_bytes_min;
    uint32_t rate_bytes_max;
    uint32_t rate_updates_min;
    uint32_t rate_updates_max;

    // Client //

    std::vector<Server> server_list;
    std::string server_address;
    unsigned short server_port;
    std::string server_password;
    uint32_t rate_bytes;
    uint32_t rate_updates;
    uint32_t rate_commands;
    uint32_t commands_this_second;
    uint32_t counter_commands;
    //When a client connects to a server, its UPDATE_RATE is set to the server's.
    //When the game is stopped, the client reverts to its original UPDATE_RATE.
    double recall_update_rate;
    RakNet::Time last_update_time;
    std::vector<std::string> command_buffer;

    RakNet::RakNetGUID server_id;

    Network();

    void reset();
    void stop();

    void tick();

    void receive_packets();

    std::string get_name_list();
    std::string get_ping_list();
    std::string get_stats();

    Client_Data* get_packet_client();
    int get_client_index(Client_Data* client);

    //Returns a list of clients that are currently players.
    std::vector<Client_Data*> get_players();
    uint32_t get_player_count();
    //Returns our player number, or -1 if we are not a player.
    int get_our_player_number();

    bool client_name_taken(std::string name);

    std::string receive_incompatible_protocol();

    void receive_chat_message();
    void send_chat_message(std::string message,RakNet::RakNetGUID target_id,bool broadcast);

    void add_command(std::string command);

    // Server //

    void start_as_server(bool allow_clients=true);

    void prepare_server_input_states();

    void send_version();

    void receive_client_data();

    void send_name_change(std::string old_name,std::string new_name,bool own_name);

    void send_initial_game_data();

    void receive_connected();

    void send_client_list();

    void send_updates();
    void send_update(Client_Data* client,uint32_t client_rate_bytes);

    void receive_input();

    void send_ping_list();

    void send_paused();

    void send_sound(std::string sound,RakNet::RakNetGUID target_id,bool broadcast);

    // Client //

    void set_server_target(int index);
    void add_server(std::string get_name,std::string get_address,unsigned short get_port,std::string get_password);
    void remove_server(int index);
    void edit_server(int index,std::string get_name,std::string get_address,unsigned short get_port,std::string get_password);
    Server* get_server(int index);

    void start_as_client();
    void connect_to_server();

    void receive_version();

    void send_client_data(bool first_send=false);

    void receive_initial_game_data();

    void send_connected();

    void receive_client_list();

    void receive_update();

    void send_input();

    void receive_ping_list();

    void receive_paused();

    void receive_sound();

    // Game-Specific //

    void write_initial_game_data(RakNet::BitStream* bitstream);
    void read_initial_game_data(RakNet::BitStream* bitstream);

    void write_update(RakNet::BitStream* bitstream);
    void read_update(RakNet::BitStream* bitstream);
};

#endif
