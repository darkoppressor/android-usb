#include "network.h"
#include "world.h"

using namespace std;

void Network::start_as_server(bool allow_clients){
    if(status=="off"){
        unsigned int actual_max_clients=max_clients;
        if(!allow_clients){
            actual_max_clients=0;
        }

        peer=RakNet::RakPeerInterface::GetInstance();

        RakNet::SocketDescriptor sd(port,"");
        RakNet::StartupResult startup=peer->Startup(max_clients,&sd,1);

        if(startup==RakNet::RAKNET_STARTED){
            peer->SetMaximumIncomingConnections(actual_max_clients);

            peer->SetUnreliableTimeout(500);

            peer->SetOccasionalPing(true);

            peer->SetLimitIPConnectionFrequency(frequent_connection_protection);

            if(password.length()>0){
                peer->SetIncomingPassword(password.c_str(),password.length());
            }

            id=peer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
            address=peer->GetSystemAddressFromGuid(id);

            clients.push_back(Client_Data(id,address,game.option_name,true));
            clients[0].connected=true;

            timer_tick.start();

            status="server";
        }
        else{
            message_log.add_error("Error initializing server: "+string_stuff.num_to_string(startup));
        }
    }
}

void Network::prepare_server_input_states(){
    clients[0].command_states.clear();

    clients[0].command_states=game.command_states;
}

void Network::send_version(){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_VERSION);

        bitstream.WriteCompressed((RakNet::RakString)engine_interface.get_version().c_str());
        if(!ignore_checksum){
            bitstream.WriteCompressed((RakNet::RakString)CHECKSUM.c_str());
        }
        else{
            bitstream.WriteCompressed((RakNet::RakString)"");
        }

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,MEDIUM_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CONNECTION,packet->guid,false);
    }
}

void Network::receive_client_data(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    bool first_send=false;
    bitstream.ReadCompressed(first_send);

    RakNet::RakString rstring;
    bitstream.ReadCompressed(rstring);

    uint32_t client_rate_bytes=0;
    bitstream.ReadCompressed(client_rate_bytes);

    uint32_t client_rate_updates=0;
    bitstream.ReadCompressed(client_rate_updates);

    Client_Data* client=get_packet_client();

    if(client!=0){
        string old_name=client->name;

        client->name=rstring.C_String();
        client->rate_bytes=client_rate_bytes;
        client->rate_updates=client_rate_updates;

        if(first_send){
            send_initial_game_data();
        }
        else if(client->name!=old_name){
            send_name_change(old_name,client->name,false);
        }
    }
}

void Network::send_name_change(string old_name,string new_name,bool own_name){
    if(status=="server"){
        if(own_name){
            clients[0].name=new_name;
        }

        string msg=old_name+" has changed their name to "+new_name;

        engine_interface.add_chat(msg);
        send_chat_message(msg,RakNet::UNASSIGNED_RAKNET_GUID,true);

        send_client_list();
    }
}

void Network::send_initial_game_data(){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_INITIAL_GAME_DATA);

        bitstream.Write(UPDATE_RATE);

        write_initial_game_data(&bitstream);

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,MEDIUM_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CONNECTION,packet->guid,false);
    }
}

void Network::receive_connected(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    Client_Data* client=get_packet_client();

    if(client!=0){
        client->connected=true;

        string msg=client->name+" has connected from "+packet->systemAddress.ToString(true);

        engine_interface.add_chat(msg);
        send_chat_message(msg,packet->guid,true);

        send_client_list();
    }
}

void Network::send_client_list(){
    if(status=="server"){
        int client_index=-1;

        for(int i=0;i<clients.size();i++){
            Client_Data* client=&clients[i];

            if(client->connected){
                client_index++;

                RakNet::BitStream bitstream;
                bitstream.Write((RakNet::MessageID)ID_GAME_CLIENT_LIST);

                int clients_size=0;
                for(int j=0;j<clients.size();j++){
                    if(clients[j].connected){
                        clients_size++;
                    }
                }

                bitstream.WriteCompressed(clients_size);
                for(int j=0;j<clients.size();j++){
                    if(clients[j].connected){
                        bitstream.WriteCompressed((RakNet::RakString)clients[j].name.c_str());
                    }
                }

                bitstream.WriteCompressed(client_index);

                stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
                peer->Send(&bitstream,LOW_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CHAT,client->id,false);
            }
        }
    }
}

void Network::send_updates(){
    if(status=="server"){
        if(game.in_progress){
            for(int i=1;i<clients.size();i++){
                if(clients[i].connected){
                    uint32_t client_rate_bytes=clients[i].rate_bytes;
                    if(rate_bytes_min!=0 && client_rate_bytes<rate_bytes_min){
                        client_rate_bytes=rate_bytes_min;
                    }
                    else if(rate_bytes_max!=0 && client_rate_bytes>rate_bytes_max){
                        client_rate_bytes=rate_bytes_max;
                    }

                    uint32_t client_rate_updates=clients[i].rate_updates;
                    if(rate_updates_min!=0 && client_rate_updates<rate_updates_min){
                        client_rate_updates=rate_updates_min;
                    }
                    else if(rate_updates_max!=0 && client_rate_updates>rate_updates_max){
                        client_rate_updates=rate_updates_max;
                    }

                    if(clients[i].updates_this_second<client_rate_updates && ++clients[i].counter_update>=(uint32_t)ceil(UPDATE_RATE/(double)client_rate_updates)){
                        clients[i].counter_update=0;

                        send_update(&clients[i],client_rate_bytes);
                    }
                }
            }
        }
    }
}

void Network::send_update(Client_Data* client,uint32_t client_rate_bytes){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_TIMESTAMP);
        bitstream.Write(RakNet::GetTime());
        bitstream.Write((RakNet::MessageID)ID_GAME_UPDATE);

        write_update(&bitstream);

        uint32_t stream_bytes=bitstream.GetNumberOfBytesUsed();
        if(client->bytes_this_second+stream_bytes<=client_rate_bytes){
            client->bytes_this_second+=stream_bytes;
            client->updates_this_second++;

            stat_counter_bytes_sent+=stream_bytes;
            peer->Send(&bitstream,HIGH_PRIORITY,UNRELIABLE,ORDERING_CHANNEL_UNORDERED,client->id,false);
        }
    }
}

void Network::receive_input(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    Client_Data* client=get_packet_client();

    if(client!=0){
        client->command_states.clear();

        int command_buffer_size=0;
        bitstream.ReadCompressed(command_buffer_size);

        for(int i=0;i<command_buffer_size;i++){
            RakNet::RakString rstring;
            bitstream.ReadCompressed(rstring);

            client->command_buffer.push_back(rstring.C_String());
        }

        int command_states_size=0;
        bitstream.ReadCompressed(command_states_size);

        for(int i=0;i<command_states_size;i++){
            RakNet::RakString rstring;
            bitstream.ReadCompressed(rstring);

            client->command_states.push_back(rstring.C_String());
        }
    }
}

void Network::send_ping_list(){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_PING_LIST);

        bitstream.WriteCompressed((int)clients.size());
        for(int i=1;i<clients.size();i++){
            int ping=0;
            if(clients[i].connected){
                ping=peer->GetAveragePing(clients[i].id);
            }

            bitstream.WriteCompressed(ping);
        }

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,LOW_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CHAT,RakNet::UNASSIGNED_RAKNET_GUID,true);
    }
}

void Network::send_paused(){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_PAUSED);

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,HIGH_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_PAUSED,RakNet::UNASSIGNED_RAKNET_GUID,true);
    }
}

void Network::send_sound(string sound,RakNet::RakNetGUID target_id,bool broadcast){
    if(status=="server"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_SOUND);

        bitstream.WriteCompressed((RakNet::RakString)sound.c_str());

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,HIGH_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_SOUND,target_id,broadcast);
    }
}
