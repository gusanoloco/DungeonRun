#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Character.h"
#include <string>

using namespace std;

class Game
{
public:
	Game();
	~Game();

	// Sets all the Game variables to a fresh state for
	// a new game, also  insures all memory is initialized
	void setupGame();

	// Updates one iteration of the game logic. Used in the
	// main game loop.
	bool update();
	

private:
	bool locateEnemyAt(int r, int c, int &index);

	bool movePlayerNorth();
	bool movePlayerSouth();
	bool movePlayerEast();
	bool movePlayerWest();

	bool moveEnemyNorth(Character &character);
	bool moveEnemySouth(Character &character);
	bool moveEnemyEast(Character &character);
	bool moveEnemyWest(Character &character);
	
	void setCameraPosition(int r, int c);

	int randomIntInRange(int min, int max);

	void drawScreen();
	void respawnDeadMonsters();
	void calculateViewable();
	void raytrace(int x0, int y0, int x1, int y1);
	void win();
	void lose();
	void newMessage(string text);

	void updateViewport();
	void drawViewport();
	void positionCursor(int col, int row);
	void cursorVisibility(bool visibility);

	unsigned char *m_visability;
	unsigned char *m_viewportBuffer;
	unsigned char *m_viewportBufferPrevious;

	bool m_fog;
	Map map;
	Character m_player;
	Character *m_enemies;
	int m_camRow;
	int m_camCol;
	int m_hasKey;
	int m_turns;
	string m_messages[3];
	bool m_gameover;
	int m_renderMode;
	bool m_colors;

};

#endif