#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ServerInterface.h"
#include <mutex>

namespace Networking {

#define BUFSIZE sizeof(Packet)
#define PORT 3323
#define OTHER_PORT 8889

	//IP Code to manage gross ip grossness in cpp '
	//credit to https://stackoverflow.com/users/838180/bilash-saha

	std::mutex mtx;

	struct IPv4
	{
		byte b1, b2, b3, b4;
	};

	uchar lastAssignedId = 0;

	bool getMyIP(IPv4 & myIP)
	{
		char szBuffer[1024];

#ifdef WIN32
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 0);
		if (::WSAStartup(wVersionRequested, &wsaData) != 0)
			return false;
#endif


		if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
		{
#ifdef WIN32
			WSACleanup();
#endif
			return false;
		}

		struct hostent *host = gethostbyname(szBuffer);
		if (host == NULL)
		{
#ifdef WIN32
			WSACleanup();
#endif
			return false;
		}

		//Obtain the computer's IP
		myIP.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
		myIP.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
		myIP.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
		myIP.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
		WSACleanup();
#endif
		return true;
	}

	struct sockaddr_in myaddr;      /* our address */
	struct sockaddr_in remaddr;     /* remote address */
	socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
	int recvlen;                    /* # bytes received */
	SOCKET sckt;                         /* our socket */
	char buf[BUFSIZE];     /* receive buffer */
	char buf2[BUFSIZE];
	int iResult = 0;
	SOCKET playerSckt;
	struct sockaddr_in playerAddr;

	WSADATA wsaData;


	void ServerInterface::loop() {



		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			printf("WSAStartup failed with error %d\n", iResult);
		}

		/* create a UDP socket */
		sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sckt == INVALID_SOCKET) {
			perror("cannot create socket\n");
			WSACleanup();
		}

		/* bind the socket to any valid IP address and a specific port */

		memset((char *)&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = htons(PORT);

		if (bind(sckt, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			perror("bind failed \n");
			closesocket(sckt);
			WSACleanup();
		}


		/* create a UDP socket */
		playerSckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (playerSckt == INVALID_SOCKET) {
			perror("cannot create socket\n");
			WSACleanup();
		}


		int total = 0;
		Packet* p;
		Packet response;

		IPv4 myIp;
		getMyIP(myIp);


		bool endLoop = false;

		printf("waiting on port %d\nPress any key to quit \n", PORT);

		/* now loop, receiving data and printing what we received */
		while (!endLoop && thisThreadCurrentlyExists) {

			bool respond = false;

			int timeout = 5000;
			setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

			recvlen = recvfrom(sckt, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);

			if (recvlen != SOCKET_ERROR) {
				
				p = (Packet *)buf;

				TYPES packetType = (TYPES)p->info.type;

				switch (packetType) {
				case CONNECTION:
				{
					respond = false;
					char ac[80];
					if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
						printf("Error getting host name \n");
					}
					

					struct hostent *phe = gethostbyname(ac);
					if (phe == 0) {
						printf("Bad host lookup \n");
					}

					struct in_addr addr;

					//someone is trying to find us, send back a connection response
					for (int i = 0; phe->h_addr_list[i] != 0; ++i) {

						memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
						if (addr.S_un.S_un_b.s_b1 != 192) {
							printf("Address: %s \n", inet_ntoa(addr));
							inet_pton(AF_INET, inet_ntoa(addr), &myaddr.sin_addr);
						}
					}

					//check that we haven't already added this user
					if (!isIPConnected(remaddr)) {
						Connection c = { TYPES::CONNECTION,{ myaddr.sin_addr.S_un.S_un_b.s_b1, myaddr.sin_addr.S_un.S_un_b.s_b2, myaddr.sin_addr.S_un.S_un_b.s_b3, myaddr.sin_addr.S_un.S_un_b.s_b4 }, PORT };
						response.connection = c;
						//generate id for player
						uchar pid = getNextId();
						//map id to ip of player connected
						players[pid] = remaddr.sin_addr.S_un.S_addr;
						server->UserConnectedCallback(pid);
						respond = true;
					}
					break;


				}
				case KEY_PRESS:
				{
					respond = false;
					char pid = 0;
					
					//find which user pressed the key
					for (auto it = players.begin(); it != players.end(); ++it) {
						if ((it->second) == remaddr.sin_addr.S_un.S_addr) {
							pid = it->first;
						}
					}
					//send that data to the server
					server->UserInputCallback(p->keyPress, pid);

					break;
				}
				case EXIT:
				{
					//tell the server the player left
					char pid = 0;

					for (auto it = players.begin(); it != players.end(); ++it) {
						if ((it->second) == remaddr.sin_addr.S_un.S_addr) {
							pid = it->first;
						}
					}
					server->SomeoneLeftCallback(pid);

					//remove player from list of players who need updates
					auto it = players.find(pid);
					players.erase(it);

				}
				default:
				{
					respond = false;
					break;
				}
				}
				if (respond) {
					sendto(sckt, (char *)&response, sizeof(response), 0, (struct sockaddr *)&remaddr, addrlen);
				}
			}
		}
		closesocket(sckt);

		WSACleanup();

	}

	uchar ServerInterface::getNextId() {
		auto result = lastAssignedId;
		lastAssignedId += 1;
		return lastAssignedId;
	}

	void ServerInterface::SetServer(IServer* s)
	{
		server = s;
	}


	bool ServerInterface::isIPConnected(sockaddr_in ip) {
		//search through out map 
		for (auto it = players.begin(); it != players.end(); ++it)
			if ((it->second) == ip.sin_addr.S_un.S_addr)
				return true;
		return false;
	}


	void ServerInterface::SendKill(unsigned char uid_to)
	{
		Kill k = {TYPES::KILL, uid_to};


		sockaddr_in playerAddr;
		playerAddr.sin_family = AF_INET;
		playerAddr.sin_addr.s_addr = players[uid_to];
		playerAddr.sin_port = htons(OTHER_PORT);

		sendto(playerSckt, (char *)&k, sizeof(k), 0, (struct sockaddr *)&playerAddr, addrlen);
		mtx.unlock();

	}

	void ServerInterface::SendExit(unsigned char uid_to)
	{
	}

	void ServerInterface::SendDeath(unsigned char uid_to, uchar uid_involved)
	{
		mtx.lock();
		Death d = { TYPES::DEATH, uid_involved };

		sockaddr_in playerAddr;
		playerAddr.sin_family = AF_INET;
		playerAddr.sin_addr.s_addr = players[uid_to];
		playerAddr.sin_port = htons(OTHER_PORT);

		sendto(playerSckt, (char *)&d, sizeof(d), 0, (struct sockaddr *)&playerAddr, addrlen);
		mtx.unlock();
	}

	void ServerInterface::BroadcastDeath(uchar who_died)
	{
		mtx.lock();
		Death d = { TYPES::DEATH, who_died };

		//send update to all players who are connected
		for (auto const& p : players)
		{
			sockaddr_in playerAddr;
			playerAddr.sin_family = AF_INET;
			playerAddr.sin_addr.s_addr = p.second;
			playerAddr.sin_port = htons(OTHER_PORT);

			sendto(playerSckt, (char *)&d, sizeof(d), 0, (struct sockaddr *)&playerAddr, addrlen);
		}
		mtx.unlock();
	}

	void ServerInterface::SendUpdate(unsigned char type, unsigned short x, unsigned short y, unsigned char dir, unsigned char player_id, unsigned char id_involved, int len, int index)
	{
		mtx.lock();
		EntityUpdate update = { TYPES::UPDATE, type, x, y, dir, id_involved, len, index };

		sockaddr_in playerAddr;
		playerAddr.sin_family = AF_INET;
		playerAddr.sin_addr.s_addr = players[player_id];
		playerAddr.sin_port = htons(OTHER_PORT);

		sendto(playerSckt, (char *)&update, sizeof(update), 0, (struct sockaddr *)&playerAddr, addrlen);
		mtx.unlock();
	}

}