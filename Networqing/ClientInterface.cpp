#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ClientInterface.h"
#include <iostream>
#define OTHER_PORT 8889

namespace Networking {

	//IP Code to manage gross ip grossness in cpp '
	//credit to https://stackoverflow.com/users/838180/bilash-saha

	struct sockaddr_in server_addr;
	
	struct IPv4
	{
		byte b1, b2, b3, b4;
	};


	bool getMyIP(IPv4 & myIP)
	{
		char szBuffer[1024];

		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 0);

		if (::WSAStartup(wVersionRequested, &wsaData) != 0)
			return false;

		if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
		{
			WSACleanup();
			return false;
		}

		struct hostent *host = gethostbyname(szBuffer);
		if (host == NULL)
		{
			WSACleanup();
			return false;
		}

		//Obtain the computer's IP
		myIP.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
		myIP.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
		myIP.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
		myIP.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;

		WSACleanup();

		return true;
	}

	//end of ip code

	void ClientInterface::ConnectToServer() {

		WSADATA wsaData;
		int iResult = 0;
		sockaddr_in remaddr;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (iResult != NO_ERROR) {
			printf("WSAStartup failed with error %d\n", iResult);
		}

		/* create a UDP socket */
		sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sckt == INVALID_SOCKET || ServerSocket == INVALID_SOCKET) {
			perror("cannot create socket\n");
			WSACleanup();
		}
		setsockopt(sckt, SOL_SOCKET, SO_BROADCAST,
			(char *)&broadcast, sizeof broadcast);
		
		int timeout = 5000;
		if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&timeout, sizeof(timeout)) < 0) {
			printf("Error setting socket options for Server Socket");
		}
	
		//reset if not empty
		memset(&myaddr, 0, sizeof(myaddr));
		//set properties
		myaddr.sin_family = AF_INET;
		myaddr.sin_port = htons(OTHER_PORT);
		char ac[80];
		if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
			printf("Error getting host name \n");
		}
		printf("Host name is %s \n" , ac);

		struct hostent *phe = gethostbyname(ac);
		if (phe == 0) {
			printf("Bad host lookup \n");
		}

		for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
			if (addr.S_un.S_un_b.s_b1 != 192) {
				printf("Address: %s \n", inet_ntoa(addr));
				inet_pton(AF_INET, inet_ntoa(addr), &myaddr.sin_addr);
			}
		}

		
		/* bind the socket to any valid IP address and a specific port */
		if (bind(sckt, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			perror("bind failed \n");
			closesocket(sckt);
			sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			WSACleanup();
		}

		IPv4 myIp;

		getMyIP(myIp);

		//Create Connect packet
		Connection c = { TYPES::CONNECTION, { myIp.b1, myIp.b2, myIp.b3, myIp.b4 }, port };

		//broadcast signal asking for server
		bool serverConnect = false;
		bool mine = true;

		while (!serverConnect && thisThreadCurrentlyExists) {
			std::cout << "ping" << std::endl;
			struct sockaddr_in Sender_addr;
			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons(PORT);
			Sender_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

			sendto(sckt, (char*)&c, sizeof(c), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr));
			int timeout = 5000;
			setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
			
			recvlen = recvfrom(sckt, buf, BUFSIZE, 0, NULL, NULL);

			if (recvlen != SOCKET_ERROR) {
				//check if we got something from the server

				Packet* inData = (Packet *)buf;

				if (inData->info.type == TYPES::CONNECTION) {

					server_addr.sin_family = AF_INET;
					//set the connection
					server_addr.sin_addr.S_un.S_un_b.s_b1 = inData->connection.ip[0];
					server_addr.sin_addr.S_un.S_un_b.s_b2 = inData->connection.ip[1];
					server_addr.sin_addr.S_un.S_un_b.s_b3 = inData->connection.ip[2];
					server_addr.sin_addr.S_un.S_un_b.s_b4 = inData->connection.ip[3];
					server_addr.sin_port = htons(PORT);

					serverConnect = true;
				}
			}
		}

		if (serverConnect) {
			//Start the main loop
			closesocket(sckt);

			loop();
		}
		else { //if we committed suicide end the socket
			closesocket(sckt);

			WSACleanup();

		}

	}

	void ClientInterface::commitSuicide()
	{
		
		thisThreadCurrentlyExists = false;

	}

	void ClientInterface::loop() {

		int total = 0;
		Packet* p;
		Packet response;

		bool endLoop = false;
		

		////Set up the port all over again /////
		/* bind the socket to any valid IP address and a specific port */
		/* create a UDP socket */
		SOCKET sckt2;
		sckt2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sckt2 == INVALID_SOCKET || ServerSocket == INVALID_SOCKET) {
			perror("cannot create socket\n");
			WSACleanup();
		}

		//reset if not empty
		memset(&myaddr, 0, sizeof(myaddr));
		//set properties
		myaddr.sin_family = AF_INET;
		myaddr.sin_port = htons(OTHER_PORT);

		if (bind(sckt2, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			perror("bind failed \n");
			closesocket(sckt2);
			WSACleanup();
		}

		/* now loop, receiving data and printing what we received */
		while (!endLoop && thisThreadCurrentlyExists) {

			bool respond = false;

			int timeout = 5000;
			setsockopt(sckt2, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

			recvlen = recvfrom(sckt2, buf, BUFSIZE, 0, (struct sockaddr *)&server_addr, &addrlen);

			if (recvlen != SOCKET_ERROR) {

				p = (Packet *)buf;

				TYPES packetType = (TYPES)p->info.type;

				switch (packetType) {
				case DEATH:
				{
					respond = false;
					client->DeathCallback(p->death);
					break;
				}
				case UPDATE:
				{
					respond = false;
					 client->EntityUpdateCallback(p->entityUpdate);
					break;
				}
				case KILL:
				{
					client->KillCallback(p->kill);
					respond = false;
					break;
				}
				default:
				{
					respond = false;
					break;
				}
				}
				if (respond) sendto(sckt2, (char *)&response, sizeof(response), 0, (struct sockaddr *)&server_addr, addrlen);
			}
		}
		closesocket(sckt2);

		WSACleanup();

	}
	void ClientInterface::SetClient(IClient * c)
	{
		client = c;
	}

	void ClientInterface::LeaveGame()
	{
		server_addr.sin_port = htons(PORT);
		Exit toSend = { TYPES::EXIT };
		sendto(sckt, (char *)&toSend, sizeof(toSend), 0, (struct sockaddr *)&server_addr, addrlen);
	}
	void ClientInterface::SendInput(Networking::USER_INPUT input)
	{
		this->input = input;
		KeyPress toSend = { TYPES::KEY_PRESS, input };
		server_addr.sin_port = htons(PORT);
		int error = sendto(sckt, (char *)&toSend, sizeof(toSend), 0, (struct sockaddr *)&server_addr, addrlen);
 		if (error != SOCKET_ERROR) {
			//printf("Data sent \n"); 
		}
		else
		{
			
			int err = WSAGetLastError();
			printf("Error sending: "+err);
		}
	}



}