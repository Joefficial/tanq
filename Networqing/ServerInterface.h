#pragma once
#include "IServer.h"
#include <map>

namespace Networking {
	class ServerInterface {

	public:

		std::map<unsigned char, ULONG> players; //Map char id to ip addr
		IServer* server;
		void loop();
		uchar getNextId();
		void SetServer(IServer*);
		bool isIPConnected(sockaddr_in ip);
		bool thisThreadCurrentlyExists = true;

		void commitSuicide()
		{

			thisThreadCurrentlyExists = false;

		}

		void SendKill(unsigned char uid_to);
		void SendExit(unsigned char uid_to);
		void SendDeath(unsigned char uid_to, uchar uid_who_died);
		void BroadcastDeath(uchar who_died);
		void SendUpdate(unsigned char type, unsigned short x, unsigned short y, unsigned char dir, unsigned char player_id, unsigned char id_involved, int total_num_of_entities, int current_index);
	};

}

