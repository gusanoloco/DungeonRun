#ifndef MAP_H
#define MAP_H

class Map {
public:
	Map() {};
	Map(int rows, int cols);
	~Map();

	// Generate new map data
	void generateMap(int rows, int cols);

	// deletes map memory
	void deleteMap();

	// Getters
	unsigned char getMapTile(int row, int col);
	int getRows() { return m_rows; };
	int getCols() { return m_cols; };

	/// Setters
	void setMapTile(int row, int col, unsigned char ch);

private:
	int randomIntInRange(int min, int max);

	unsigned char *m_currentMap;	// char array to hold the map data
	int m_rows;			// number of rows in the map
	int m_cols;			// number of columns in the map
};

#endif