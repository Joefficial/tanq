#pragma once
#include "Entity.h"
#include <vector>


int const TOTAL_SIZE = GRID_SIZE * MAP_SIZE;

struct Grid
{
	int x;
	int y;
	int width;
	int height;
};

class Mapping
{
public:

	/*
	*  Creates a 24x24 map of grids that are 16x16 pixels.
	*  The total map size will be 768 x 768 pixels
	*  The grids will be structs that hold the correct x, y values
	*  for the 24 x 24 tile map that we agreed upon
	*  in the spec
	*
	*  The first index represents the x value in the
	*  grid. And the second index represents the y
	*  value in the grid.
	*
	*  For example, grid_map[4][5] references a grid in
	*  the 4th column in the 5th row. grid_map[4][5]->x
	*  will return the exact pixel location for the x
	*  coordinate in the actual map. grid_map[4][5]->y
	*  will do the same for the y coordinate.
	*
	*  Stop trying make fetch happen.
	*
	*/
	std::vector<std::vector<Grid*>> grid_map;
	std::vector<Entity*> walls;

	int numOfWalls = 0;

	Mapping()
	{
		// initiliaze a grid map of size 24 grids x 24 grids
		grid_map = std::vector<std::vector<Grid*>>(MAP_SIZE, std::vector<Grid*>(MAP_SIZE, nullptr));
		walls.reserve(200);
	}

	/*
	* Builds the coordinates for each 16x16 grid on the map.
	*/
	void InitMap();

	bool** BuildPhysicalMap(char* map);


	/*
	* This function builds an physical game map
	* by creating Map objects at a grid coordinate.
	*
	* For example, passing a 2D bool array with the index
	* at [1][3] set to true will create an entity at the
	* coordinates of the grid at col 1 row 3. Rendering it
	* as collidable object at whatever the x and y vals are.
	*/
	void ConstructMap(bool** physical_map);


};
