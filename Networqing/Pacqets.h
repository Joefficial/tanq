#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short word;
typedef unsigned short ushort;

namespace Networking {

#pragma pack(push) // save current packing 
#pragma pack(1)    // 1 byte alignment for structs in this file 

	enum TYPES : byte {
		UPDATE,
		CONNECTION,
		KEY_PRESS,
		DEATH,
		KILL,
		EXIT,
		INVALID = 255
	};

	enum ENTITY_TYPES : byte {
		TANK,
		WALL,
		BULLET
	};

	enum USER_INPUT : word {
		RIGHT,
		UP,
		LEFT,
		DOWN,
		FIRE
	};

	struct PacketInformation
	{
		byte type;
		byte data[15];
	};

	struct EntityUpdate
	{
		byte type;
		byte entity_type;
		word x;
		word y;
		byte dir;
		byte id;
		byte len;
		byte offset;
		byte unused[7];
	};

	struct Connection
	{
		byte type;
		byte ip[4];
		word port;
		byte unused[9];
	};

	struct KeyPress
	{
		byte type;
		USER_INPUT input;
		byte len;
		byte offset;
		byte unused[11];
	};

	struct Death
	{
		byte type;
		byte id;
		byte unused[14];
	};

	struct Kill
	{
		byte type;
		byte id;
		byte unused[14];
	};

	struct Exit
	{
		byte type;
		byte id;
		byte unused[14];
	};

	union Packet {

		byte raw[16];
		PacketInformation info;
		EntityUpdate entityUpdate;
		Connection connection;
		KeyPress keyPress;
		Death death;
		Kill kill;
		Exit exit;
	};
}
#pragma pack(pop) // back to default packing 


