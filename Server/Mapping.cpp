#include "Mapping.h"
#include "Player.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>

void Mapping::InitMap()
{
	// build the map grid

	// row Index
	for (int i = 0; i < MAP_SIZE; i++)
	{
		// column index
		for (int j = 0; j < MAP_SIZE; j++)
		{
			// make a 32 x 32 tile where the x is dependent on column index and y is dependent on row index
			Grid* tile = new Grid{ j * GRID_SIZE, i * GRID_SIZE, GRID_SIZE, GRID_SIZE };
			grid_map[j][i] = tile;
		}

	}

}

bool** Mapping::BuildPhysicalMap(char* map)
{
	std::ifstream infile(map, std::ios::binary);
	std::stringstream ss;
	std::string inputLine = "";

	char meep[MAP_SIZE*(MAP_SIZE / 8)]; // Rows times bytes per row = total bytes

	while (getline(infile, inputLine))
	{
		for (int i = 0; i < inputLine.size(); i++)
		{
			meep[i] = inputLine.c_str()[i];
		}
	}

	bool** physical_map = new bool*[MAP_SIZE];
	for (int i = 0; i < MAP_SIZE; i++)
	{
		physical_map[i] = new bool[MAP_SIZE];
	}

	std::string holder = "";

	// Map Builder
	for (int i = 0; i < MAP_SIZE*(MAP_SIZE / 8); i++)
	{
		holder += std::bitset<8>(meep[i]).to_string();
	}

	// 2D Array Populator
	int row = 0;
	int idx = 0;
	for (int col = 0; row < MAP_SIZE; col++, idx++)
	{
		if(col % MAP_SIZE == 0 && col != 0)
		{
			row++;
			col = 0;
		}

		switch(holder[idx])
		{
			case '1':
				physical_map[col][row] = true;
				break;
			case '0':
				physical_map[col][row] = false;
				break;
			default:
				break;
		}
	}

	// 2D Array Printer
	for(int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			switch (physical_map[j][i])
			{
			case true:
				std::cout << '1';
				break;
			case false:
				std::cout << '0';
				break;
			default:
				break;
			}
		}
		std::cout << std::endl;
	}
	return physical_map;
}

void Mapping::ConstructMap(bool ** physical_map)
{
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			if (physical_map[j][i] && numOfWalls < 200)
			{
				Entity* newEntity = new Entity(grid_map[j][i]->x, grid_map[j][i]->y, CAP_PLAYERS + 1 + numOfWalls);
				walls.push_back(newEntity);
				numOfWalls++;
			}
		}
	}
}
