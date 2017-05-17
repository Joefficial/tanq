#pragma once
#include "../Networqing/Pacqets.h"

typedef unsigned char uchar;
const int CAP_PLAYERS = 4;
const int CAP_WALLS = 203;
const int CAP_BULLETS = 254;

class Player
{
public:

	uchar ID;

	int _x_coordinate;
	int _y_coordinate;

	bool IsCollidedLeft = false;
	bool IsCollidedRight = false;
	bool IsCollidedNorth = false;
	bool IsCollidedSouth = false;

	bool IsDead = false;

	Networking::USER_INPUT dir;

	Player(unsigned char id)
	{
		ID = id;
		_x_coordinate = 10;
		_y_coordinate = 10;
		dir = Networking::USER_INPUT::LEFT;
	}

	Player(unsigned char id, int x, int y)
	{
		ID = id;
		_x_coordinate = 10;
		_y_coordinate = 10;
		dir = Networking::USER_INPUT::LEFT;
		_x_coordinate = x;
		_y_coordinate = y;
	}

	int bottom()
	{
		return _y_coordinate + 32;
	}

	int top()
	{
		return _y_coordinate;
	}

	int right()
	{
		return _x_coordinate + 32;
	}

	int left()
	{
		return _x_coordinate;
	}

	// TODO: Define this
	void Die(){}

};
