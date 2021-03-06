#include "Game.h"
#include <conio.h>
#include "Map.h"
#include <iostream>
#include <math.h>

#include  <stdio.h>
#include  <windows.h>

using namespace std;

// Local function to set the color state of the console output.
void ChangeColour(WORD theColour)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole,theColour);		
}


// Default game settings
int MAP_ROWS = 200;
int MAP_COLS = 200;
int VIEWPORT_COLS = 20;		// viewport width from cam ex. 10 will draw the viewport 21 tiles across
int VIEWPORT_ROWS = 10;		// viewport Height from cam ex. 10 will draw the viewport 21 tiles across
int NUM_OF_ENEMIES = 30;
int ENEMY_RANGE = 20;
int NUM_OF_POTIONS = 20;

// characters for drawing boarders
const unsigned char h = 205;
const unsigned char tl = 201;
const unsigned char tr = 187;
const unsigned char bl = 200;
const unsigned char br = 188;
const unsigned char v = 186;
const unsigned char cr = 206;
const unsigned char htu = 202;

// tiles
const unsigned char TILE_KEY = 5;
const unsigned char TILE_DOOR = 234;
const unsigned char TILE_ENEMY = 157;
const unsigned char TILE_POTION = 173;
const unsigned char TILE_WALL = 219;
const unsigned char TILE_PLAYER = 1;

// random function
int Game::randomIntInRange(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}


Game::Game() 
{
	m_colors = true;
	m_renderMode = 0;
	cursorVisibility(false);

	m_visability = new unsigned char[MAP_ROWS * MAP_COLS];
	m_enemies = new Character[NUM_OF_ENEMIES];
	m_viewportBuffer = new unsigned char[((VIEWPORT_COLS*2) + 1) * ((VIEWPORT_ROWS*2) + 1)];
	m_viewportBufferPrevious = new unsigned char[((VIEWPORT_COLS*2) + 1) * ((VIEWPORT_ROWS*2) + 1)];
}

// Sets all the Game variables to a fresh state for
// a new game, also  insures all memory is initialized
void Game::setupGame()
{
	for (int i = 0; i < (((VIEWPORT_COLS*2) + 1) * ((VIEWPORT_ROWS*2) + 1)); i++)
	{
		m_viewportBufferPrevious[i] = m_viewportBuffer[i] = ' ';
	}
	system("cls");

	// generate a new map
	map.generateMap(MAP_ROWS,MAP_COLS);

	// set default values for the message buffer
	m_messages[0]="";
	m_messages[1]="";
	m_messages[2]="";

	VIEWPORT_COLS = 20;		// viewport width from cam ex. 10 will draw the viewport 21 tiles across
	VIEWPORT_ROWS = 10;		// viewport Height from cam ex. 10 will draw the viewport 21 tiles across
	m_fog = true;		// Show Fog Flag
	m_hasKey = false;	// set to true when the player finds the key in the map
	m_turns = 0;		// reset the turns
	m_gameover = false;	// reset the game over flag

	// blank out the visibility buffer  array 
	for (int row = 0; row < MAP_ROWS; ++row)
	{
		for (int col = 0; col < MAP_COLS; ++col)
		{
			m_visability[col + (row * MAP_COLS)] = 177;
		}
	}

	// Create and place the player
	bool validLocation = false;
	while (!validLocation) 
	{
		int row = randomIntInRange(1, MAP_ROWS-2);
		int col = randomIntInRange(1, MAP_COLS-2);
		// if the location is a open tile place the player
		if (map.getMapTile(row,col) == ' ') 
		{			
			validLocation = true;
			m_player.spawnNew(row,col,6);
		}
	}

	// Set the camera position to the players position but within the constraints of the viewport
	setCameraPosition(m_player.getRow(), m_player.getCol());

	// Create the enemies
	validLocation = false;
	unsigned int enemy = 0;
	while (!validLocation)
	{
		int row = randomIntInRange(1, MAP_ROWS-2);
		int col = randomIntInRange(1, MAP_COLS-2);

		// get the distance from the current location to the player
		int distance = (int)sqrt(	(float) 
			( ( (col - m_player.getCol()) * (col - m_player.getCol()) ) + 
			( (row - m_player.getRow()) * (row - m_player.getRow()) ) )
			);

		// make sure it is at least 15 tiles away from the player
		if (distance < 15)
			continue;

		// set the enemy in the current location in the array's attributes
		if (map.getMapTile(row,col) == ' ') 
		{
			m_enemies[enemy].spawnNew(row,col,randomIntInRange(1,5));
			++enemy;
			if (enemy == NUM_OF_ENEMIES) {
				validLocation = true;
			}
		}
	}

	//place key
	validLocation = false;
	while (!validLocation)
	{
		int Row = randomIntInRange(1, MAP_ROWS-2);
		int Col = randomIntInRange(1, MAP_COLS-2);


		int distance = (int)sqrt(	(float) 
			( ( (Col - m_player.getCol()) * (Col - m_player.getCol()) ) + 
			( (Row - m_player.getRow()) * (Row - m_player.getRow()) ) )
			);
		// make sure it is away from the player
		if (distance < 40)
			continue;

		if (map.getMapTile(Row,Col) == ' ') 
		{
			map.setMapTile(Row,Col,TILE_KEY);
			validLocation = true;
		}
	}
	//place door
	validLocation = false;
	while (!validLocation)
	{
		int Row = randomIntInRange(1, MAP_ROWS-2);
		int Col = randomIntInRange(1, MAP_COLS-2);

		// make sure it is away from the player
		int distance = (int)sqrt(	(float) 
			( ( (Col - m_player.getCol()) * (Col - m_player.getCol()) ) + 
			( (Row - m_player.getRow()) * (Row - m_player.getRow()) ) )
			);

		// calculate min distance based on map size
		if ( distance < sqrt((float)(MAP_COLS * MAP_ROWS)) / 4)
			continue;

		if (map.getMapTile(Row,Col) == ' ') 
		{
			map.setMapTile(Row,Col, TILE_DOOR);
			validLocation = true;
		}
	}

	// place potions
	validLocation = false;
	unsigned int pot = 0;
	while (!validLocation)
	{
		int Row = randomIntInRange(1, MAP_ROWS-2);
		int Col = randomIntInRange(1, MAP_COLS-2);

		// make sure it is at least 15 tiles away from the player
		int distance = (int)sqrt(	(float) 
			( ( (Col - m_player.getCol()) * (Col - m_player.getCol()) ) + 
			( (Row - m_player.getRow()) * (Row - m_player.getRow()) ) )
			);

		// calculate min distance based on map size
		if ( distance < sqrt((float)(MAP_COLS * MAP_ROWS)) / 4)
			continue;

		if (map.getMapTile(Row,Col) == ' ') 
		{
			map.setMapTile(Row,Col, TILE_POTION);
			++pot;
			if (pot == NUM_OF_POTIONS) {
				validLocation = true;
			}
		}
	}
}

// clean up memory when exiting
Game::~Game()
{
	cursorVisibility(true);
	delete [] m_enemies; m_enemies = 0;
	delete [] m_visability; m_visability = 0;
	delete [] m_viewportBuffer; m_viewportBuffer = 0;
	delete [] m_viewportBufferPrevious; m_viewportBufferPrevious = 0;
}

bool Game::update() 
{
	// check if the game is over and exit with a false
	// if it is.
	if(m_gameover)
	{
		return false;
	}

	// Check if the player is dead and call the lose function if 
	// he is.
	if(m_player.isDead())
		lose();


	// check for dead monsters in the enemy array and 
	// respawn them in the map if they are.
	respawnDeadMonsters();

	// set the camera position to the player
	// moved below input to test camara scrolling
	//setCameraPosition(m_player.getRow(), m_player.getCol());

	// draw the map.
	if(m_renderMode == 2)
		drawScreen();
	if(m_renderMode == 0 || m_renderMode == 1)
		updateViewport();

	//get user input and move the player
	switch (getch()) 
	{
	case 'w':
	case 'W':
		{
			movePlayerNorth();
			break;
		}
	case 's':
	case 'S':
		{
			movePlayerSouth();
			break;
		}
	case 'd':
	case 'D':
		{
			movePlayerEast();
			break;
		}
	case 'a':
	case 'A':
		{
			movePlayerWest();
			break;
		}
	case 'q':
	case 'Q':	// use a potion
		{
			if ( m_player.getPotions() )
			{
				m_player.addHP(randomIntInRange(5,50));
				m_player.setPotions(m_player.getPotions()-1);
				newMessage("You Drink A potion!");
			}
			break;
		}
	case 'p':
	case 'P': //exit the Game if p is pressed
		{
			return false;
			break;
		}
	case 'o':
	case 'O': // disable fog with the o key
		{
			if (m_fog)
				m_fog = false;
			else
				m_fog = true;

			newMessage("Cheater!");
			break;
		}
	case '0': // disable fog with the o key
		{
			if (m_colors)
				m_colors = false;
			else
				m_colors = true;

			newMessage("Color Mode Changed");
			return true;
			break;
		}

	case 't':
	case 'T':
		{
			setCameraPosition(m_camRow-1, m_camCol);
			return true;
			break;
		}
	case 'f':
	case 'F':
		{
			setCameraPosition(m_camRow, m_camCol-1);
			return true;
			break;
		}
	case 'g':
	case 'G':
		{
			setCameraPosition(m_camRow+1, m_camCol);
			return true;
			break;
		}
	case 'h':
	case 'H':
		{
			setCameraPosition(m_camRow, m_camCol+1);
			return true;
			break;
		}

	case '7':
		{
			newMessage("Render Mode 1");
			m_renderMode = 0;
			system("cls");
			return true;
			break;
		}

	case '8':
		{
			newMessage("Render Mode 2");
			m_renderMode = 1;
			system("cls");
			return true;
			break;
		}

	case '9':
		{
			newMessage("Render Mode 3");
			m_renderMode = 2;
			system("cls");
			return true;
			break;
		}

	default:
		{ 
			return true;
		}
	} // End input switch

	setCameraPosition(m_player.getRow(), m_player.getCol());


	// Enemy movement loop through the enemies and perform there movement
	for (int i = 0; i < NUM_OF_ENEMIES; ++i) 
	{
		//get the current enemies distance from the player
		int distance = (int)sqrt((float) ( 
			( (m_enemies[i].getCol() - m_player.getCol()) * (m_enemies[i].getCol() - m_player.getCol()) ) + 
			( (m_enemies[i].getRow() - m_player.getRow()) * (m_enemies[i].getRow() - m_player.getRow()) )
			)
			);
		// if the distance is less than ENEMY_RANGE units try to move the enemy towards the player
		if (distance < ENEMY_RANGE)
		{
			bool moved = false;
			int count = 3;	// counter to prevent infinite loop if enemy get stuck
			while (!moved)
			{
				// pick a random direction if it moves the enemy closer to the player then move
				// otherwise try again.
				switch(randomIntInRange(0,3))
				{
				case 0:
					if (m_enemies[i].getRow() > m_player.getRow())
					{
						moved = moveEnemyNorth(m_enemies[i]);
					}
					break;
				case 1:
					if (m_enemies[i].getRow() < m_player.getRow())
					{
						moved = moveEnemySouth(m_enemies[i]);
					}
					break;
				case 2:
					if (m_enemies[i].getCol() < m_player.getCol())
					{
						moved = moveEnemyEast(m_enemies[i]);
					}
					break;
				case 3:
					if (m_enemies[i].getCol() > m_player.getCol())
					{
						moved = moveEnemyWest(m_enemies[i]);
					}
					break;
				default:
					break;
				}
				count--;
				if (count == 0)
					moved = true;
			}
		}
		else // otherwise move randomly
		{
			int count = 3;
			bool moved = false;
			while (!moved)
			{
				switch(randomIntInRange(0,4))
				{
				case 0:
					moved = moveEnemyNorth(m_enemies[i]);
					break;
				case 1:
					moved = moveEnemySouth(m_enemies[i]);
					break;
				case 2:
					moved = moveEnemyEast(m_enemies[i]);
					break;
				case 3:
					moved = moveEnemyWest(m_enemies[i]);
					break;
				default: // dont move if 4 is rolled
					break;
				}
				count--;
				if (count == 0)
					moved = true;
			}
		}
	}
	m_turns++;
	return true;
}

//sets the camera position
// trys to center the camera on the position passed in but will not move if 
// it is to close to the side of the map
void Game::setCameraPosition(int r, int c)
{
	if ( r - VIEWPORT_ROWS < 0)
		m_camRow = VIEWPORT_ROWS;
	else if ( r + VIEWPORT_ROWS + 1 > MAP_ROWS)
		m_camRow = MAP_ROWS - VIEWPORT_ROWS - 1;
	else
		m_camRow = r;

	if ( c - VIEWPORT_COLS < 0)
		m_camCol = VIEWPORT_COLS;
	else if ( c + VIEWPORT_COLS + 1 > MAP_COLS)
		m_camCol = MAP_COLS - VIEWPORT_COLS - 1;
	else
		m_camCol = c;
}



// add a new message buffer to the list. only three possible messages can exist  at a time
// FI-FO list
void Game::newMessage(string text)
{
	m_messages[0] = m_messages[1];
	m_messages[1] = m_messages[2];
	m_messages[2] = text;
}

// at the beginning of the update this function is called to respawn any dead enemies 
// back into the map. this helps keep the difficulty constant through the play through
// Works the same way as the setup of the game.
void Game::respawnDeadMonsters()
{	// check each enemy to see if it is dead and respawn a new monster randomly on the map
	// to take its place.
	for (int i = 0; i < NUM_OF_ENEMIES; ++i) 
	{
		if (m_enemies[i].isDead()) 
		{
			// if it is get a random spot on the map
			bool foundSpot = false;
			while (!foundSpot)
			{
				int Row = randomIntInRange(1, MAP_ROWS-2);
				int Col = randomIntInRange(1, MAP_COLS-2);

				// make sure it is at least 15 tiles away from the player
				int distance = (int)sqrt(	(float) 
					( ( (Col - m_player.getCol()) * (Col - m_player.getCol()) ) + 
					( (Row - m_player.getRow()) * (Row - m_player.getRow()) ) )
					);

				if (distance < 15)
					continue;

				// if the tile is empty respawn the monster with new stats
				if (map.getMapTile(Row,Col) == ' ') 
				{
					m_enemies[i].spawnNew(Row,Col,randomIntInRange(1,5));
					foundSpot = true;
				}

			}
		}
	}
}

// passing in coordinates and an index reference
// if an enemy exists at the location passed set the index to that value
// and return true
// otherwise return false and set the index to -1
bool Game::locateEnemyAt(int r, int c, int &index)
{
	index = -1;
	for (int i = 0; i < NUM_OF_ENEMIES; ++i) 
	{
		if ( (m_enemies[i].getRow() == r) && (m_enemies[i].getCol() == c) )
		{
			index = i;
			return true;
		}
	}
	return false;
}

// Called when the win condition is met. displays the winning message and some game stats
// finally sets the game over state to true so that the game will exit after the pause
void Game::win()
{
	system("cls");
	cout << endl << endl << endl;
	cout << "You Win! You made it out of the dungeon and survived to adventure another day." << endl;
	cout << "You lasted for " << m_turns << " turns in the dungeon."<< endl;
	cout << "You defeated " << m_player.getkills() << " emenies."<< endl;
	cout << endl;
	cout << "Your Max HP was: "<< m_player.getMaxHP() << endl;
	cout << "Your Attack Power was: "<< m_player.getATT() << endl;
	cout << "Your Defense Was: "<< m_player.getDEF() << endl;
	Sleep(3000);
	cout << endl;
	system("pause");
	m_gameover = true;
}

// Called when the lose condition is met. displays the losing message and some game stats
// finally sets the game over state to true so that the game will exit after the pause
void Game::lose()
{
	system("cls");
	cout << endl << endl << endl;
	cout << "You Lose! You were defeated. Maybe the next adventure will do better." << endl;
	cout << "You lasted for " << m_turns << " turns in the dungeon."<< endl;
	cout << "You defeated " << m_player.getkills() << " emenies before you were slain."<< endl;
	cout << endl;
	cout << "Your Max HP was: "<< m_player.getMaxHP() << endl;
	cout << "Your Attack Power was: "<< m_player.getATT() << endl;
	cout << "Your Defense Was: "<< m_player.getDEF() << endl;
	cout << endl;
	Sleep(3000);
	system("pause");
	m_gameover = true;
}

//Moves the player north on the map has all the logic for players 
// interaction with objects walls and enemies
bool Game::movePlayerNorth()
{
	if ( (m_player.getRow() > 1) && (map.getMapTile(m_player.getRow() - 1, m_player.getCol()) == ' ') )
	{
		int enemyIndex = -1;
		if (locateEnemyAt(m_player.getRow() - 1, m_player.getCol(), enemyIndex))
		{
			bool hit = m_player.attack(m_enemies[enemyIndex]);
			if(hit)
				newMessage("You Hit!");
			else
				newMessage("You Missed!");
		}
		else
		{
			m_player.setRow( m_player.getRow() - 1 );
		}
		return true;
	}
	else if ( (m_player.getRow() > 1) && (map.getMapTile(m_player.getRow() - 1, m_player.getCol()) == TILE_KEY) )
	{
		map.setMapTile(m_player.getRow() - 1, m_player.getCol(),' ');
		m_hasKey = true;
		newMessage("You Got The Key!");
		return true;
	}
	else if ( (m_player.getRow() > 1) && (map.getMapTile(m_player.getRow() - 1, m_player.getCol()) == TILE_POTION) )
	{
		map.setMapTile(m_player.getRow() - 1, m_player.getCol(),' ');
		m_player.setPotions(m_player.getPotions() + 1);
		newMessage("You Found a Potion!");
		return true;
	}
	else if ( (m_player.getRow() > 1) && (map.getMapTile(m_player.getRow() - 1, m_player.getCol()) == TILE_DOOR) )
	{

		if (m_hasKey)
			win();

		m_player.setRow( m_player.getRow() - 1 );
		return true;
	}
	return false;
}

//Moves the player South on the map has all the logic for players 
// interaction with objects walls and enemies
bool Game::movePlayerSouth()
{
	if ( (m_player.getRow() < MAP_ROWS - 2) && (map.getMapTile(m_player.getRow() + 1, m_player.getCol()) == ' ') )
	{
		int enemyIndex = -1;
		if (locateEnemyAt(m_player.getRow() + 1, m_player.getCol(), enemyIndex))
		{
			bool hit = m_player.attack(m_enemies[enemyIndex]);
			if(hit)
				newMessage("You Hit!");
			else
				newMessage("You Missed!");
		}
		else
		{
			m_player.setRow( m_player.getRow() + 1 );
		}
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow() + 1, m_player.getCol()) == TILE_KEY) )
	{
		map.setMapTile(m_player.getRow() + 1, m_player.getCol(),' ');
		m_hasKey = true;
		newMessage("You Got The Key!");
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow() + 1, m_player.getCol()) == TILE_POTION) )
	{
		map.setMapTile(m_player.getRow() + 1, m_player.getCol(),' ');
		m_player.setPotions(m_player.getPotions() + 1);
		newMessage("You Found a Potion!");
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow() + 1, m_player.getCol()) == TILE_DOOR) )
	{

		if (m_hasKey)
			win();

		m_player.setRow( m_player.getRow() + 1 );
		return true;
	}
	return false;
}

//Moves the player East on the map has all the logic for players 
// interaction with objects walls and enemies
bool Game::movePlayerEast()
{
	if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow(), m_player.getCol() + 1) ==' ') )
	{
		int enemyIndex = -1;
		if (locateEnemyAt(m_player.getRow(), m_player.getCol() + 1, enemyIndex))
		{
			bool hit = m_player.attack(m_enemies[enemyIndex]);
			if(hit)
				newMessage("You Hit!");
			else
				newMessage("You Missed!");
		}
		else
		{
			m_player.setCol( m_player.getCol() + 1 );
		}
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow(), m_player.getCol() + 1) == TILE_KEY) )
	{
		map.setMapTile(m_player.getRow(), m_player.getCol() + 1,' ');
		m_hasKey = true;
		newMessage("You Got The Key!");
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow(), m_player.getCol() + 1) == TILE_POTION) )
	{
		map.setMapTile(m_player.getRow(), m_player.getCol() + 1,' ');
		m_player.setPotions(m_player.getPotions() + 1);
		newMessage("You Found a Potion!");
		return true;
	}
	else if ( (m_player.getCol() < MAP_COLS - 2) && (map.getMapTile(m_player.getRow(), m_player.getCol() + 1) == TILE_DOOR) )
	{

		if (m_hasKey)
			win();

		m_player.setCol( m_player.getCol() + 1 );
		return true;
	}

	return false;
}

//Moves the player west on the map has all the logic for players 
// interaction with objects walls and enemies
bool Game::movePlayerWest()
{
	if ( (m_player.getCol() > 1) && (map.getMapTile(m_player.getRow(), m_player.getCol() - 1) == ' ') )
	{
		int enemyIndex = -1;
		if (locateEnemyAt(m_player.getRow(), m_player.getCol() - 1, enemyIndex))
		{
			bool hit = m_player.attack(m_enemies[enemyIndex]);
			if(hit)
				newMessage("You Hit!");
			else
				newMessage("You Missed!");
		}
		else
		{
			m_player.setCol( m_player.getCol() - 1 );
		}
		return true;
	}
	else if ( (m_player.getCol() > 1) && (map.getMapTile(m_player.getRow(), m_player.getCol() - 1) == TILE_KEY) )
	{
		map.setMapTile(m_player.getRow(), m_player.getCol() - 1,' ');
		m_hasKey = true;
		newMessage("You Got The Key!");
		return true;
	}
	else if ( (m_player.getCol() > 1) && (map.getMapTile(m_player.getRow(), m_player.getCol() - 1) == TILE_POTION) )
	{
		map.setMapTile(m_player.getRow(), m_player.getCol() - 1,' ');
		m_player.setPotions(m_player.getPotions() + 1);
		newMessage("You Found a Potion!");
		return true;
	}
	else if ( (m_player.getCol() > 1) && (map.getMapTile(m_player.getRow(), m_player.getCol() - 1) == TILE_DOOR) )
	{

		if (m_hasKey)
			win();

		m_player.setCol( m_player.getCol() - 1 );
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Moves the Enemy north on the map has all the logic for enemy 
// interaction with objects walls and the player and other enemies
bool Game::moveEnemyNorth(Character &character)
{
	if ( (character.getRow() > 1) && (
		(map.getMapTile(character.getRow() - 1, character.getCol()) == ' ') || 
		(map.getMapTile(character.getRow() - 1, character.getCol()) == TILE_DOOR) || 
		(map.getMapTile(character.getRow() - 1, character.getCol()) == TILE_KEY) || 
		(map.getMapTile(character.getRow() - 1, character.getCol()) == TILE_POTION)
		)
		)
	{
		int enemyIndex = -1;
		if (locateEnemyAt(character.getRow() - 1, character.getCol(), enemyIndex))
		{
			//character.attack(m_enemies[enemyIndex]);
		} 
		else if ( character.getRow() - 1 == m_player.getRow() && character.getCol() == m_player.getCol() )
		{
			bool hit = character.attack(m_player);
			if(hit)
				newMessage("Enemy Hit You!");
			else
				newMessage("Enemy Missed!");
		}
		else
		{
			character.setRow( character.getRow() - 1 );
		}
		return true;
	}
	return false;
}

// Moves the Enemy south on the map has all the logic for enemy 
// interaction with objects walls and the player and other enemies
bool Game::moveEnemySouth(Character &character)
{
	if ( (character.getRow() < MAP_ROWS - 2) && (
		(map.getMapTile(character.getRow() + 1, character.getCol()) == ' ') ||
		(map.getMapTile(character.getRow() + 1, character.getCol()) == TILE_DOOR) ||
		(map.getMapTile(character.getRow() + 1, character.getCol()) == TILE_KEY) ||
		(map.getMapTile(character.getRow() + 1, character.getCol()) == TILE_POTION)
		) 
		)
	{
		int enemyIndex = -1;
		if (locateEnemyAt(character.getRow() + 1, character.getCol(), enemyIndex))
		{
			//character.attack(m_enemies[enemyIndex]);
		} 
		else if ( character.getRow() + 1 == m_player.getRow() && character.getCol() == m_player.getCol() )
		{
			bool hit = character.attack(m_player);
			if(hit)
				newMessage("Enemy Hit You!");
			else
				newMessage("Enemy Missed!");
		}
		else
		{
			character.setRow( character.getRow() + 1 );
		}
		return true;
	}
	return false;
}

// Moves the Enemy east on the map has all the logic for enemy 
// interaction with objects walls and the player and other enemies
bool Game::moveEnemyEast(Character &character)
{
	if ( (character.getCol() < MAP_COLS - 2) && (
		(map.getMapTile(character.getRow(), character.getCol() + 1) == ' ') || 
		(map.getMapTile(character.getRow(), character.getCol() + 1) == TILE_DOOR) || 
		(map.getMapTile(character.getRow(), character.getCol() + 1) == TILE_KEY) || 
		(map.getMapTile(character.getRow(), character.getCol() + 1) == TILE_POTION)
		)
		)
	{
		int enemyIndex = -1;
		if (locateEnemyAt(character.getRow(), character.getCol() + 1, enemyIndex))
		{
			//character.attack(m_enemies[enemyIndex]);
		} 
		else if ( character.getRow() == m_player.getRow() && character.getCol() + 1 == m_player.getCol() )
		{
			bool hit = character.attack(m_player);
			if(hit)
				newMessage("Enemy Hit You!");
			else
				newMessage("Enemy Missed!");
		}
		else
		{
			character.setCol( character.getCol() + 1 );
		}
		return true;
	}
	return false;
}

// Moves the Enemy west on the map has all the logic for enemy 
// interaction with objects walls and the player and other enemies
bool Game::moveEnemyWest(Character &character)
{
	if ( (character.getCol() > 1) && (
		(map.getMapTile(character.getRow(), character.getCol()-1) == ' ') ||
		(map.getMapTile(character.getRow(), character.getCol()-1) == TILE_DOOR) ||
		(map.getMapTile(character.getRow(), character.getCol()-1) == TILE_KEY) ||
		(map.getMapTile(character.getRow(), character.getCol()-1) == TILE_POTION)
		)
		)
	{
		int enemyIndex = -1;
		if (locateEnemyAt(character.getRow(), character.getCol() - 1, enemyIndex))
		{
			//character.attack(m_enemies[enemyIndex]);

		} 
		else if ( character.getRow() == m_player.getRow() && character.getCol() - 1 == m_player.getCol() )
		{
			bool hit = character.attack(m_player);
			if(hit)
				newMessage("Enemy Hit You!");
			else
				newMessage("Enemy Missed!");
		}
		else
		{
			character.setCol( character.getCol() - 1 );
		}
		return true;
	}
	return false;
}

// Calculates the viewable portion of the map from the characters perspective
void Game::calculateViewable()
{
	// set the fog character based on if fog is enabled
	unsigned char fog = 0;
	if (m_fog)
		fog = 177;

	// blank out the visibility array but ignoring the walls and door if they have been 
	// drawn to the array. allowing them to persist even after they are no longer viewable.
	for (int row = 0; row < MAP_ROWS; ++row)
	{
		for (int col = 0; col < MAP_COLS; ++col)
		{
			if ((m_visability[col + (row * MAP_COLS)] != TILE_WALL) && (m_visability[col + (row * MAP_COLS)] != TILE_DOOR))
				m_visability[col + (row * MAP_COLS)] = fog;
		}
	}

	// For each tile starting from the edge of the viewable area and going around the viewport
	// perform the ray trace from the player to the tile.
	for (int Row = m_camRow - VIEWPORT_ROWS; Row < m_camRow + VIEWPORT_ROWS + 1; ++Row) 
	{
		for (int Col = m_camCol - VIEWPORT_COLS; Col < m_camCol + VIEWPORT_COLS + 1; ++Col) 
		{
			if ( (Row == (m_camRow - VIEWPORT_ROWS)) || (Col == (m_camCol - VIEWPORT_COLS)) ||
				(Row == (m_camRow + VIEWPORT_ROWS)) || (Col == (m_camCol + VIEWPORT_COLS)) )
			{
				raytrace(m_player.getCol(), m_player.getRow(), Col, Row);
				//cout << map.getMapTile(Row,Col);
			}
		}
	}
}

void Game::raytrace(int startX, int startY, int endX, int endY)
{
	int distanceX = abs(endX - startX);
	int distanceY = abs(endY - startY);
	int x = startX;
	int y = startY;
	int n = 1 + distanceX + distanceY;
	int x_incrament = (endX > startX) ? 1 : -1;
	int y_incrament = (endY > startY) ? 1 : -1;
	int error = distanceX - distanceY;
	distanceX *= 2;
	distanceY *= 2;

	unsigned char fog = 177;
	for (; n > 0; --n)
	{
		////////////////////////////////////////////
		// if the ray hits a wall draw a wall tile and return
		if (map.getMapTile(y,x) == TILE_WALL)
		{
			if (m_visability[x + (y * MAP_COLS)] != TILE_WALL )
			m_visability[x + (y * MAP_COLS)] = TILE_WALL;
			return;
		} // if the tile is the door write a door to the array
		else if (map.getMapTile(y,x) == TILE_DOOR)
		{
			if (m_visability[x + (y * MAP_COLS)] != TILE_DOOR )
			{
				m_visability[x + (y * MAP_COLS)] = TILE_DOOR;
			}
			return;
		}// if it is anything else write a zero
		else if (m_visability[x + (y * MAP_COLS)] != TILE_WALL )
			m_visability[x + (y * MAP_COLS)] = 0;

		///////////////////////////////////////////
		if (error > 0)
		{
			x += x_incrament;
			error -= distanceY;
		}
		else
		{
			y += y_incrament;
			error += distanceX;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// positions the cursor on the screen at the givin location
void Game::positionCursor( int col, int row )
{
	HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD pos;
	pos.X = col;
	pos.Y = row;

	SetConsoleCursorPosition(output_handle, pos);
}

// change the cursor visability in the console
void Game::cursorVisibility( bool visibility )
{
	HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO ccInfo;
	ccInfo.dwSize = sizeof(CONSOLE_CURSOR_INFO);
	ccInfo.bVisible = visibility;

	SetConsoleCursorInfo(output_handle, &ccInfo);
}

// this function updates the viewport data and calls draw viewport.
void Game::updateViewport()
{
	calculateViewable();

	// write new char info to the new buffer
	for (int Row = 0; Row < ((VIEWPORT_ROWS*2) + 1); ++Row) 
	{
		for (int Col = 0; Col < ((VIEWPORT_COLS*2) + 1); ++Col) 
		{
			if (m_visability[(Col+(m_camCol - VIEWPORT_COLS)) + ((Row+(m_camRow - VIEWPORT_ROWS)) * MAP_COLS)] == 0)
			{
				if ( map.getMapTile(Row + (m_camRow - VIEWPORT_ROWS),Col+(m_camCol - VIEWPORT_COLS)) != ' ' )
				{
					m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] = map.getMapTile(Row+(m_camRow - VIEWPORT_ROWS),Col+(m_camCol - VIEWPORT_COLS));
				}
				else 
				{
					int temp;
					if ( (m_player.getCol() == Col+(m_camCol - VIEWPORT_COLS)) && (m_player.getRow() == (Row+(m_camRow - VIEWPORT_ROWS))) )
					{
						m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] = TILE_PLAYER;
					}
					else if	(locateEnemyAt(Row+(m_camRow - VIEWPORT_ROWS),Col+(m_camCol - VIEWPORT_COLS),temp))
					{
						m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] = TILE_ENEMY;
					}
					else
					{
						m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] = ' ';
					}
				}
			}
			else
			{
				m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] = m_visability[(Col+(m_camCol - VIEWPORT_COLS)) + ((Row+(m_camRow - VIEWPORT_ROWS)) * MAP_COLS)];
			}
		}
	}

	drawViewport();
	// copy buffer info to the old buffer
	for (int i = 0; i < (((VIEWPORT_COLS*2) + 1) * ((VIEWPORT_ROWS*2) + 1)); i++)
	{
		m_viewportBufferPrevious[i] = m_viewportBuffer[i];
	}
}

// this function draws changes to the viewport buffer to the screen along with the in game menues
void Game::drawViewport()
{
	if(m_gameover)
		system("cls");

	for (int Row = 0; Row < ((VIEWPORT_ROWS*2) + 1); ++Row) 
	{
		for (int Col = 0; Col < ((VIEWPORT_COLS*2) + 1); ++Col) 
		{
			if(m_renderMode == 0) // rendermode 0
			{
				positionCursor(Col + 38, Row);

				if (m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] != TILE_ENEMY )
				{
					if (m_colors)
					{
						//change the color assigned to the tile
						switch(m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))])
						{
						case TILE_KEY:
						case TILE_DOOR:
							ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
							break;
						case TILE_POTION:
							ChangeColour(FOREGROUND_RED  |FOREGROUND_INTENSITY);
							break;
						default:
							ChangeColour(7);
							break;

						}
					}

					//draw the tile
					cout << m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))];

					//return the tile to default color
					ChangeColour(7);
				}
				else // if the map tile is a space check for characters on the map
				{

					int temp;
					// if the tile is a player print the player
					if ( (m_player.getCol() == Col+(m_camCol - VIEWPORT_COLS)) && (m_player.getRow() == (Row+(m_camRow - VIEWPORT_ROWS))) )
					{
						cout << TILE_PLAYER;
					}
					if	(locateEnemyAt(Row+(m_camRow - VIEWPORT_ROWS),Col+(m_camCol - VIEWPORT_COLS),temp)) // if it is an enemy
					{ // Reference link http://stackoverflow.com/questions/8285825/how-to-change-text-or-background-color-in-a-windows-console-application

						// set the color based on the enemies level
						if (m_colors)
						{
							switch(m_enemies[temp].getLevel())
							{
							case 1:
							case 2:
								ChangeColour(FOREGROUND_GREEN  | FOREGROUND_INTENSITY);
								break;
							case 3:
							case 4:
								ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
								break;
							case 5:
								ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
								break;
							default:
								break;

							}
						}
						// draw the enemy
						cout << TILE_ENEMY;
						// reset the color to default
						ChangeColour(7);
					}
					else // otherwise draw an empty tile
					{
						cout << ' ';
					}
				}
			}
			else // rendermode 1
			{
				// Render mode 1 has some visual artifacts on windows 7 when colors are enabled.
				if (
					((m_viewportBufferPrevious[Col + (Row * ((VIEWPORT_COLS*2) + 1))] != m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))])) ||
					((Row > VIEWPORT_ROWS - 1) && (Row < VIEWPORT_ROWS + 1))
					)
				{
					positionCursor(Col + 38, Row);

					if (m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))] != TILE_ENEMY )
					{
						if (m_colors)
						{
							//change the color assigned to the tile
							switch(m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))])
							{
							case TILE_KEY:
							case TILE_DOOR:
								ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
								break;
							case TILE_POTION:
								ChangeColour(FOREGROUND_RED  |FOREGROUND_INTENSITY);
								break;
							default:
								ChangeColour(7);
								break;

							}
						}

						//draw the tile
						cout << m_viewportBuffer[Col + (Row * ((VIEWPORT_COLS*2) + 1))];

						//return the tile to default color
						ChangeColour(7);
					}
					else // if the map tile is a space check for characters on the map
					{

						int temp;
						//// if the tile is a player print the player
						//if ( (m_player.getCol() == Col+(m_camCol - VIEWPORT_COLS)) && (m_player.getRow() == (Row+(m_camRow - VIEWPORT_ROWS))) )
						//{
						//	cout << TILE_PLAYER;
						//}
						if	(locateEnemyAt(Row+(m_camRow - VIEWPORT_ROWS),Col+(m_camCol - VIEWPORT_COLS),temp)) // if it is an enemy
						{
							// set the color based on the enemies level
							if (m_colors)
							{
								switch(m_enemies[temp].getLevel())
								{
								case 1:
								case 2:
									ChangeColour(FOREGROUND_GREEN  | FOREGROUND_INTENSITY);
									break;
								case 3:
								case 4:
									ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
									break;
								case 5:
									ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
									break;
								default:
									break;
								}
							}
							// draw the enemy
							cout << TILE_ENEMY;
							// reset the color to default
							ChangeColour(7);
						}
						else // otherwise draw an empty tile
						{
							cout << ' ';
						}
					}
				}
			}
		}
	}


	positionCursor(38,21);
	cout <<"                    ";
	positionCursor(38,22);
	cout << "                    ";
	positionCursor(38,23);
	cout << "                    ";

	positionCursor(38,21);
	cout << m_messages[2];
	positionCursor(38,22);
	cout << m_messages[1];
	positionCursor(38,23);
	cout << m_messages[0];

	for (int textrow = 0; textrow < 25; textrow++)
	{
		// I am not going to explain it in detail it is simple and just alot of formatting.
		positionCursor(0,textrow);
		if (textrow == 1)
		{	
			cout << "                                      ";
			positionCursor(0,textrow);
			cout << " Hit Points = " << m_player.getHP() << "/" << m_player.getMaxHP() << "  Key? = " << ((m_hasKey) ? "Found" : "Not Found");
		}
		if (textrow == 3)
		{
			cout << "                                    ";
			positionCursor(0,textrow);
			cout << "      To Move         Potions = " << m_player.getPotions();
		}
		if (textrow == 4)
			cout << "        "<<tl<<h<<tr <<"               "<<tl<<h<<tr;
		if (textrow == 5)
			cout << "        "<<v<<"W"<<v <<"               "<<v<<"Q"<<v;;
		if (textrow == 6)
			cout << "      "<<tl<<h<<cr<<h<<cr<<h<<tr <<"             "<<bl<<h<<br;
		if (textrow == 7)
			cout << "      "<<v<<"A"<<v<<"S"<<v<<"D"<<v<<"            Drink";
		if (textrow == 8)
			cout << "      "<<bl<<h<<htu<<h<<htu<<h<<br;
		if (textrow == 10)
			cout << "      Get Key      Bump into enemies";
		if (textrow == 11)
		{
			if (m_colors)
			ChangeColour(7);

			cout << "         ";
			if (m_colors)
			ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
			cout << TILE_KEY;
			if (m_colors)
			ChangeColour(7);
			cout << "          to fight them.";

		}
		if (textrow == 13)
			cout << "     Find Exit";
		if (textrow == 14)
		{
			if (m_colors)
			ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
			cout << "         "<<TILE_DOOR;
			if (m_colors)
			ChangeColour(7);
		}
		if (textrow == 16)
			cout << "       Avoid";
		if (textrow == 17)
		{
			if (m_colors)
			ChangeColour(FOREGROUND_GREEN  | FOREGROUND_INTENSITY);
			cout << "       "<<TILE_ENEMY;
			if (m_colors)
			ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout <<" "<<TILE_ENEMY;
			if (m_colors)
			ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
			cout <<" "<<TILE_ENEMY;
			if (m_colors)
			ChangeColour(7);
		}
		if (textrow == 19)
			cout << "      Potions";
		if (textrow == 20)
		{
			if (m_colors)
			ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
			cout << "         "<<TILE_POTION;
			if (m_colors)
			ChangeColour(7);
		}
	}



}

// draw the screen
// rendermode 2

// this was the origianl render code.
//i left it in because it was a learning experiance and could be usefull?
void Game::drawScreen()
{
	// calculate the viewable area of the map form the players perspective.
	calculateViewable();

	// clear the screen
	system("cls");

	int textrow = 0; //counter used to draw text to the left of the viewport


	// draw the play area viewport
	for (int Row = m_camRow - VIEWPORT_ROWS; Row < m_camRow + VIEWPORT_ROWS + 1; ++Row) 
	{
		for (int Col = m_camCol - VIEWPORT_COLS; Col < m_camCol + VIEWPORT_COLS + 1; ++Col) 
		{
			// if the tiles visability atribute is zero
			if (m_visability[Col + (Row * MAP_COLS)] == 0)
			{
				// and the map tile is not a space
				if ( map.getMapTile(Row,Col) != ' ' )
				{
					//change the color assigned to the tile
					if (m_colors)
					{
						switch(map.getMapTile(Row,Col))
						{
						case TILE_KEY:
						case TILE_DOOR:
							ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
							break;
						case TILE_POTION:
							ChangeColour(FOREGROUND_RED  |FOREGROUND_INTENSITY);
							break;
						default:
							ChangeColour(7);
							break;

						}
					}

					//draw the tile
					cout << map.getMapTile(Row,Col);

					//return the tile to default color
					ChangeColour(7);
				}
				else // if the map tile is a space check for characters on the map
				{

					int temp;
					// if the tile is a player print the player
					if ( (m_player.getCol() == Col) && (m_player.getRow() == Row) )
					{
						cout << TILE_PLAYER;
					}
					else if	(locateEnemyAt(Row,Col,temp)) // if it is an enemy
					{
						// set the color based on the enemies level
						if (m_colors)
						{
							switch(m_enemies[temp].getLevel())
							{
							case 1:
							case 2:
								ChangeColour(FOREGROUND_GREEN  | FOREGROUND_INTENSITY);
								break;
							case 3:
							case 4:
								ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
								break;
							case 5:
								ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
								break;
							default:
								break;
							}
						}
						// draw the enemy
						cout << TILE_ENEMY;
						// reset the color to default
						ChangeColour(7);
					}
					else // otherwise draw an empty tile
					{
						cout << ' ';
					}
				}
			}
			else // otherwise draw the tile from the visibility buffer
			{
				cout << m_visability[Col + (Row * MAP_COLS)];
			}
		}

		// draw the left side data by checking what line the draw is on and adding 
		// they string information to the side
		// I am not going to explain it in detail it is simple and just alot of formatting.
		if (textrow == 1)
			cout << "  Hit Points = " << m_player.getHP() << "/" << m_player.getMaxHP() << "  Key? = " << ((m_hasKey) ? "Found" : "Not Found");
		if (textrow == 3)
			cout << "       To Move         Potions = " << m_player.getPotions();
		if (textrow == 4)
			cout << "         "<<tl<<h<<tr <<"               "<<tl<<h<<tr;
		if (textrow == 5)
			cout << "         "<<v<<"W"<<v <<"               "<<v<<"Q"<<v;;
		if (textrow == 6)
			cout << "       "<<tl<<h<<cr<<h<<cr<<h<<tr <<"             "<<bl<<h<<br;
		if (textrow == 7)
			cout << "       "<<v<<"A"<<v<<"S"<<v<<"D"<<v<<"            Drink";
		if (textrow == 8)
			cout << "       "<<bl<<h<<htu<<h<<htu<<h<<br;
		if (textrow == 10)
			cout << "       Get Key      Bump into enemies";
		if (textrow == 11)
		{
			ChangeColour(7);

			cout << "          ";
			ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
			cout << TILE_KEY;
			ChangeColour(7);
			cout << "           to fight them.";

		}
		if (textrow == 13)
			cout << "      Find Exit";
		if (textrow == 14)
		{
			ChangeColour(FOREGROUND_BLUE  | FOREGROUND_GREEN  |FOREGROUND_INTENSITY);
			cout << "          "<<TILE_DOOR;
			ChangeColour(7);
		}
		if (textrow == 16)
			cout << "        Avoid";
		if (textrow == 17)
		{
			ChangeColour(FOREGROUND_GREEN  | FOREGROUND_INTENSITY);
			cout << "        "<<TILE_ENEMY;
			ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout <<" "<<TILE_ENEMY;
			ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
			cout <<" "<<TILE_ENEMY;
			ChangeColour(7);
		}
		if (textrow == 19)
			cout << "       Potions";
		if (textrow == 20)
		{
			ChangeColour(FOREGROUND_RED  | FOREGROUND_INTENSITY);
			cout << "          "<<TILE_POTION;
			ChangeColour(7);
		}

		cout << endl;
		textrow++;
	}

	// Print out the message buffer at the bottom
	cout << m_messages[2] << endl;
	cout << m_messages[1] << endl;
	cout << m_messages[0];
}