#pragma once
#include "../Networqing/Pacqets.h";

typedef unsigned char uchar;

const int GRID_SIZE = 32;
const int MAP_SIZE = 24;

class Entity
{
private:

	int _width;
	int _height;

public:
	int _id;
	int _x_coordinate;
	int _y_coordinate;
	Networking::USER_INPUT dir;

	bool isCollidedRight = false;
	bool isCollidedLeft = false;
	bool isCollidedTop = false;
	bool isCollidedBottom = false;

	// Default for walls
	Entity(int x, int y, int id)
	{
		_x_coordinate = x;
		_y_coordinate = y;

		_width = GRID_SIZE;
		_height = GRID_SIZE;
		_id = id;

	}

	// For bullets
	Entity(int x, int y, int w, int h, Networking::USER_INPUT d, int id)
	{
		_x_coordinate = x;
		_y_coordinate = y;

		_width = w;
		_height = h;
		dir = d;
		_id = id;
	}

	int bottom()
	{
		return _y_coordinate + _height;
	}

	int top()
	{
		return _y_coordinate;
	}

	int right()
	{
		return _x_coordinate + _width;
	}

	int left()
	{
		return _x_coordinate;
	}
};