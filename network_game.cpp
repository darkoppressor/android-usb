#include "network.h"
#include "world.h"

using namespace std;

void Network::write_initial_game_data(RakNet::BitStream* bitstream){
    ///bitstream->WriteCompressed();
}

void Network::read_initial_game_data(RakNet::BitStream* bitstream){
    ///bitstream->ReadCompressed();
}

void Network::write_update(RakNet::BitStream* bitstream){
    ///bitstream->WriteCompressed();
}

void Network::read_update(RakNet::BitStream* bitstream){
    ///bitstream->ReadCompressed();
}
