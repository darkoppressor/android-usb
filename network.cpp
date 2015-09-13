/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "network.h"
#include "world.h"

using namespace std;

Client_Data::Client_Data(RakNet::RakNetGUID get_id,RakNet::SystemAddress get_address,string get_name,bool get_is_us){
    id=get_id;
    address=get_address;

    connected=false;
    name=get_name;
    ping=0;
    is_us=get_is_us;

    rate_bytes=0;
    rate_updates=0;

    bytes_this_second=0;
    updates_this_second=0;
    counter_update=0;
}

bool Client_Data::game_command_state(string command){
    for(int i=0;i<command_states.size();i++){
        if(command_states[i]==command){
            return true;
        }
    }

    return false;
}

Network::Network(){
    status="off";
    reset();

    max_clients=0;
    port=0;
    password="";
    frequent_connection_protection=false;
    ignore_checksum=false;
    rate_bytes_min=0;
    rate_bytes_max=0;
    rate_updates_min=0;
    rate_updates_max=0;

    server_address="";
    server_port=0;
    server_password="";
    rate_bytes=0;
    rate_updates=0;
    rate_commands=0;
    recall_update_rate=UPDATE_RATE;
}

void Network::reset(){
    peer=0;
    id=RakNet::UNASSIGNED_RAKNET_GUID;
    address=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    packet=0;
    stat_bytes_received=0;
    stat_counter_bytes_received=0;
    stat_bytes_sent=0;
    stat_counter_bytes_sent=0;

    clients.clear();
    timer_tick.stop();

    commands_this_second=0;
    counter_commands=0;
    last_update_time=0;
    command_buffer.clear();

    server_id=RakNet::UNASSIGNED_RAKNET_GUID;
}

void Network::stop(){
    if(status!="off"){
        peer->DeallocatePacket(packet);

        peer->Shutdown(100);

        RakNet::RakPeerInterface::DestroyInstance(peer);

        reset();

        if(status=="client"){
            engine_interface.set_logic_update_rate(recall_update_rate);
        }

        status="off";
    }
}

void Network::tick(){
    if(status!="off" && timer_tick.get_ticks()>=1000){
        if(status=="server"){
            for(int i=0;i<clients.size();i++){
                clients[i].bytes_this_second=0;
                clients[i].updates_this_second=0;
            }

            send_ping_list();
        }
        else if(status=="client"){
            commands_this_second=0;
        }

        stat_bytes_received=stat_counter_bytes_received;
        stat_counter_bytes_received=0;

        stat_bytes_sent=stat_counter_bytes_sent;
        stat_counter_bytes_sent=0;

        timer_tick.start();
    }
}

void Network::receive_packets(){
    if(status!="off"){
        tick();

        for(packet=peer->Receive();packet!=0;peer->DeallocatePacket(packet),packet=peer->Receive()){
            RakNet::MessageID packet_id;

            //If the packet has a timestamp.
            if((RakNet::MessageID)packet->data[0]==ID_TIMESTAMP){
                packet_id=(RakNet::MessageID)packet->data[sizeof(RakNet::MessageID)+sizeof(RakNet::Time)];
            }
            //If the packet has no timestamp.
            else{
                packet_id=(RakNet::MessageID)packet->data[0];
            }

            switch(packet_id){
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    if(status=="client"){
                        server_id=packet->guid;

                        message_log.add_log("Connected to server: "+string(packet->systemAddress.ToString(true)));
                    }
                    break;

                case ID_CONNECTION_ATTEMPT_FAILED:
                    if(status=="client"){
                        message_log.add_log("Could not connect to server: "+server_address+"|"+string_stuff.num_to_string(server_port));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Could not connect to server.");
                    }
                    break;

                case ID_ALREADY_CONNECTED:
                    if(status=="client"){
                        message_log.add_log("Already connected to server: "+string(packet->systemAddress.ToString(true)));
                    }
                    break;

                case ID_NEW_INCOMING_CONNECTION:
                    if(status=="server"){
                        //If this client is not already connected.
                        if(get_packet_client()==0){
                            clients.push_back(Client_Data(packet->guid,packet->systemAddress,"",false));

                            send_version();
                        }
                    }
                    break;

                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    if(status=="client"){
                        message_log.add_log("Server is full: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Server is full.");
                    }
                    break;

                case ID_DISCONNECTION_NOTIFICATION:
                    if(status=="server"){
                        Client_Data* client=get_packet_client();

                        if(client!=0){
                            string msg=client->name+" ("+packet->systemAddress.ToString(true)+") has disconnected";

                            engine_interface.add_chat(msg);
                            send_chat_message(msg,packet->guid,true);

                            int client_index=get_client_index(client);
                            if(client_index!=-1){
                                clients.erase(clients.begin()+client_index);
                            }

                            send_client_list();
                        }
                    }
                    else{
                        message_log.add_log("Disconnected from server: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Disconnected from server.");
                    }
                    break;

                case ID_CONNECTION_LOST:
                    if(status=="server"){
                        Client_Data* client=get_packet_client();

                        if(client!=0){
                            string msg=client->name+" ("+packet->systemAddress.ToString(true)+") has dropped";

                            engine_interface.add_chat(msg);
                            send_chat_message(msg,packet->guid,true);

                            int client_index=get_client_index(client);
                            if(client_index!=-1){
                                clients.erase(clients.begin()+client_index);
                            }

                            send_client_list();
                        }
                    }
                    else{
                        message_log.add_log("Lost connection to server: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Lost connection to server.");
                    }
                    break;

                case ID_CONNECTION_BANNED:
                    if(status=="client"){
                        message_log.add_log("Banned from: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Banned from server.");
                    }
                    break;

                case ID_INVALID_PASSWORD:
                    if(status=="client"){
                        message_log.add_log("Invalid password for server: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Invalid password for server.");
                    }
                    break;

                case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                    message_log.add_log("Incompatible protocol version: "+string(packet->systemAddress.ToString(true))+"\nOur protocol: "+string_stuff.num_to_string((int)RAKNET_PROTOCOL_VERSION)+"\nServer protocol: "+receive_incompatible_protocol());

                    if(status=="client"){
                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Network protocol does not match server.");
                    }
                    break;

                case ID_IP_RECENTLY_CONNECTED:
                    if(status=="client"){
                        message_log.add_log("Connected too recently to server: "+string(packet->systemAddress.ToString(true)));

                        engine_interface.button_events_manager.handle_button_event("stop_game");
                        engine_interface.make_notice("Connected too recently to server.");
                    }
                    break;

                case ID_GAME_VERSION:
                    if(status=="client"){
                        receive_version();
                    }
                    break;

                case ID_GAME_CLIENT_DATA:
                    if(status=="server"){
                        receive_client_data();
                    }
                    break;

                case ID_GAME_INITIAL_GAME_DATA:
                    if(status=="client"){
                        receive_initial_game_data();
                    }
                    break;

                case ID_GAME_CONNECTED:
                    if(status=="server"){
                        receive_connected();
                    }
                    break;

                case ID_GAME_CLIENT_LIST:
                    if(status=="client"){
                        receive_client_list();
                    }
                    break;

                case ID_GAME_CHAT_MESSAGE:
                    receive_chat_message();
                    break;

                case ID_GAME_UPDATE:
                    if(status=="client"){
                        receive_update();
                    }
                    break;

                case ID_GAME_INPUT:
                    if(status=="server"){
                        receive_input();
                    }
                    break;

                case ID_GAME_PING_LIST:
                    if(status=="client"){
                        receive_ping_list();
                    }
                    break;

                case ID_GAME_PAUSED:
                    if(status=="client"){
                        receive_paused();
                    }
                    break;

                case ID_GAME_SOUND:
                    if(status=="client"){
                        receive_sound();
                    }
                    break;

                default:
                    message_log.add_log("Received message with unknown identifier: "+string_stuff.num_to_string((int)packet_id));
                    break;
            }

            if(status=="off"){
                break;
            }
        }
    }
}

string Network::get_name_list(){
    string str_list="Name\n----\n";

    for(int i=0;i<clients.size();i++){
        str_list+=clients[i].name+"\n";
    }

    return str_list;
}

string Network::get_ping_list(){
    string str_list="Ping\n----\n";

    if(status=="server"){
        str_list+="\n";

        for(int i=1;i<clients.size();i++){
            str_list+=string_stuff.num_to_string(peer->GetAveragePing(clients[i].id))+"\n";
        }
    }
    else if(status=="client"){
        str_list+=string_stuff.num_to_string(peer->GetAveragePing(server_id))+"\n";

        for(int i=1;i<clients.size();i++){
            str_list+=string_stuff.num_to_string(clients[i].ping)+"\n";
        }
    }

    return str_list;
}

string Network::get_stats(){
    string str_stats="";

    if(status!="off"){
        str_stats+="In: "+string_stuff.num_to_string((long double)stat_bytes_received/(long double)1000.0)+" kB/s\n";
        str_stats+="Out: "+string_stuff.num_to_string((long double)stat_bytes_sent/(long double)1000.0)+" kB/s\n";
    }

    return str_stats;
}

Client_Data* Network::get_packet_client(){
    for(int i=0;i<clients.size();i++){
        if(clients[i].id==packet->guid){
            return &clients[i];
        }
    }

    return 0;
}

int Network::get_client_index(Client_Data* client){
    for(int i=0;i<clients.size();i++){
        if(&clients[i]==client){
            return i;
        }
    }

    return -1;
}

vector<Client_Data*> Network::get_players(){
    vector<Client_Data*> players;

    for(uint32_t i=0,player_count=0;i<clients.size() && player_count<game.option_max_players;i++){
        Client_Data* client=&clients[i];

        if(client->connected){
            players.push_back(client);

            player_count++;
        }
    }

    return players;
}

uint32_t Network::get_player_count(){
    return get_players().size();
}

int Network::get_our_player_number(){
    for(uint32_t i=0,player_count=0;i<clients.size() && player_count<game.option_max_players;i++){
        Client_Data* client=&clients[i];

        if(client->connected){
            if(client->is_us){
                return player_count;
            }

            player_count++;
        }
    }

    return -1;
}

bool Network::client_name_taken(string name){
    for(int i=0;i<clients.size();i++){
        if(clients[i].name==name){
            return true;
        }
    }

    return false;
}

string Network::receive_incompatible_protocol(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    unsigned char version;
    bitstream.Read(version);

    return string_stuff.num_to_string((int)version);
}

void Network::receive_chat_message(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    RakNet::RakString rstring;
    bitstream.ReadCompressed(rstring);

    engine_interface.add_chat(rstring.C_String());

    if(status=="server"){
        send_chat_message(rstring.C_String(),packet->guid,true);
    }
}

void Network::send_chat_message(string message,RakNet::RakNetGUID target_id,bool broadcast){
    if(status!="off"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_CHAT_MESSAGE);

        bitstream.WriteCompressed((RakNet::RakString)message.c_str());

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,LOW_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CHAT,target_id,broadcast);
    }
}

void Network::add_command(string command){
    if(status=="client"){
        command_buffer.push_back(command);
    }
    else if(status=="server"){
        clients[0].command_buffer.push_back(command);
    }
}
