/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

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
