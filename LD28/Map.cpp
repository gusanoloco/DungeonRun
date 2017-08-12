#include "Map.h"
#include <stdlib.h>

int Map::randomIntInRange(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}

// this constructor can be used to generate a map with the 
// given rows and columns
Map::Map(int rows, int cols) 
{
	generateMap(rows, cols);
}

// constructor calls delete map to release memory
Map::~Map()
{
	deleteMap();
}

// cleans up memory
void Map::deleteMap()
{
	// Clean up the heap
	delete [] m_currentMap;
	m_currentMap = 0;
}

// Generate new map data
void Map::generateMap(int rows, int cols)
{
	// set rows and columns
	m_rows = rows;
	m_cols = cols;

	m_currentMap = new unsigned char[m_rows * cols];

	//Fill the array with solid blocks
	for (int row = 0; row < m_rows; ++row)
	{
		for (int col = 0; col < m_cols; ++col)
		{
			m_currentMap[col + (row * m_cols)] = 219;
		}
	} 

	// starting in the center of the map
	int row = m_rows / 2;
	int col = m_cols / 2;

	int prevdir = 0;
	// for n iterations dig out caves
	for (int i = 0; i < (m_cols * m_rows) / 2; i++)
	{
		// place a space character in the array at the current position
		if (m_currentMap[col + (row * m_cols)] = 219)
		{
			m_currentMap[col + (row * m_cols)] = ' ';
		}

		// pick a random direction
		int direction = randomIntInRange(0,5);

		// for 0-3 move in a direction
		// for 4-5 keep going in the same direction as the previous iteration.
		// this help to favor longer tunnels and smaller rooms
		switch( direction )
		{
		case 0: // North
			row++;
			break;
		case 1: // South
			row--;
			break;
		case 2: // East
			col++;
			break;
		case 3: // West
			col--;
			break;
		case 4:
		case 5://Previous Direction
			switch( prevdir )
			{
			case 0: // North
				row++;
				break;
			case 1: // South
				row--;
				break;
			case 2: // East
				col++;
				break;
			case 3: // West
				col--;
				break;
			}
			break;
		}

		//ensure that the new location is within the bounds of the map
		// leaving a 1 unit boarder all the way around solid
		if(row < 1)
			row = 1;
		if(col < 1)
			col = 1;

		if(row > m_rows-2)
			row = m_rows-2;
		if(col > m_cols-2)
			col = m_cols-2;

		// Save the direction for use as previous
		prevdir = direction;
	}
}

// Returns the char at the location in the arry
unsigned char Map::getMapTile(int row, int col) 
{
	return m_currentMap[col + (row * m_cols)];
}

// Sets a location in the array to the provided char at the provided location
void Map::setMapTile(int row, int col, unsigned char ch)
{
	m_currentMap[col + (row * m_cols)] = ch;
}