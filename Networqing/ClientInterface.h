#pragma once
#include "IClient.h"
#include <vector>
#include <thread>
#include <atomic>

#define BUFSIZE sizeof(Packet)
#define PORT 3323

namespace Networking {
	class ClientInterface {

	private:
		IClient* client;
		USER_INPUT input;

		struct sockaddr_in server_addr;     /* Server address */
		struct sockaddr_in myaddr;      /* our address */

		socklen_t addrlen = sizeof(server_addr);            /* length of addresses */
		int recvlen;                    /* # bytes received */
		SOCKET sckt;                         /* our socket */
		SOCKET ServerSocket;
		char buf[BUFSIZE];     /* receive buffer */
		char buf2[BUFSIZE];
		int broadcast = 1;
		int port = PORT;
		std::atomic<bool> thisThreadCurrentlyExists = true;

	public:
		void commitSuicide();
		void loop();
		void SetClient(IClient*);
		void ConnectToServer();
		void LeaveGame();
		void SendInput(Networking::USER_INPUT input);
	};
}