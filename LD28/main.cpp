// Ludum Dare 28 - You Only Get One
// Bobby Atkins (GusanoLocovg)

#include <iostream>
#include "Game.h"
#include <stdlib.h>
#include <time.h>
#include <conio.h>

using namespace std;

//create shortcuts to special characters
const unsigned char h = 205;
const unsigned char tl = 201;
const unsigned char tr = 187;
const unsigned char bl = 200;
const unsigned char br = 188;
const unsigned char v = 186;

void displayHelp();

int main() 
{
	srand( time(NULL) );

	Game theGame;

	bool exiting = false;
	do
	{
		// Setup the game object
		theGame.setupGame();

		// display menu
		system("cls");
		cout << endl;
		cout << endl;
		cout << "        _____                                       _____" << endl;       
        cout << "       |  __ \\                                     |  __ \\" << endl;
        cout << "       | |  | |_   _ _ __   __ _  ___  ___  _ __   | |__) |   _ _ _" << endl;
        cout << "       | |  | | | | | '_ \\ / _` |/ _ \\/ _ \\| '_ \\  |  _  / | | | '_ \\" << endl;
        cout << "       | |__| | |_| | | | | (_| |  __/ (_) | | | | | | \\ \\ |_| | | | |" << endl;
        cout << "       |_____/ \\__,_|_| |_|\\__, |\\___|\\___/|_| |_| |_|  \\_\\__,_|_| |_|" << endl;
        cout << "                            __/ |" << endl;
        cout << "                           |___/  ..GusanoLocoVG.." << endl;
        cout << "    Ludum Dare 28 - You Only Get One, One Life, One way out, One Key to find." << endl;
		cout << endl;
		cout << "                                 1. Play Game" << endl;
		cout << "                                 2. How to Play" << endl;
		cout << "                                 3. Exit Game" << endl << endl;
		cout << endl;
		cout << " " << tl << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << tr << endl;
		cout << " " << v << " While walking along you hear a crack beneath your feet. Before you know it " << v << endl;
		cout << " " << v << " the ground under you is gone and you are falling into darkness. You don't  " << v << endl;
		cout << " " << v << " know where you are and can't reach the hole you fell from. You must find   " << v << endl;
		cout << " " << v << " your way out of the caves quickly. You hear growls around you.             " << v << endl;
		cout << " " << v << "                                                                 Run!!!!    " << v << endl;
		cout << " " << bl << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << h << br << endl;
		
		// Get User Input
		switch (getch()) 
		{
		case '1':
			{
				// if the selection is play game play the game

				// Main Game Loop
				bool running = true;
				theGame.setupGame();
				while (running)
				{	
					
					running = theGame.update();
				}
				/////////////////////////////////////
				break;
			}
		case '3':
			{
				// if the selection is exit Exit the program
				exiting = true;
				break;
			}
		case '2':
			{
				// if the selection is help display help screen
				displayHelp();
				break;
			}
		default:
			{ 
				break;
			}
		} // End input switch

	}	
	while(!exiting);

	return 0;
}

void displayHelp()
	{
		unsigned char p = 1;
	 system("cls");
		cout << endl;
		cout << "The Goal of Dungeon Run is to find the key hidden in the dungeon and" << endl;
		cout << "then make it to the exit while avoiding or fighting off enemies." << endl;       
        cout << "" << endl;
        cout << "The player " << p << " can moved around the screen with the 'W', 'A', 'S', and 'D' keys." << endl;
        cout << "You can look around with the camera with the 'T', 'F', 'G', and 'H' keys." << endl;
        cout << "" << endl;
        cout << "Along the way you can pick up potions buy walking onto them. These can be used" << endl;
        cout << "anytime with the 'Q' key to regain health up to the players max hit points." << endl;
        cout << "" << endl;
		cout << "To fight enemies just walk into them combat is automatic and the results are" << endl;
		cout << "displayed in the message list. It is also possible to trick enemyies and" << endl;
		cout << "move past them at times it is important to remember that passing an enemy" << endl;
		cout << "can sometimes allow it to get in an easy hit." << endl;
		cout << "" << endl;
		cout << "Enemy color indicates difficulty green to red red being the most difficult." << endl;
		cout << "" << endl;
		cout << "The 'P' key can be pressed at any time to quit the game and return to the " << endl;
		cout << "main menu screen. Quiting the current run." << endl;
		cout << "" << endl;
		system ("pause");
	}