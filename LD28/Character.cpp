#include "Character.h"
#include <iostream>

using namespace std;

// small local function to generate random ints in a range
int randomIntInRange(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}

// Constructor just sets values to defaults
Character::Character()
{
	m_attack	= 0;
	m_defense	= 0;
	m_hitPoints	= 0;
	m_kills		= 0;
	m_maxHP = m_hitPoints;
	m_potions = 0;
	m_level = 1;
}

// Attack is the main combat function and takes in another 
// character as a parameter. If the attack is successful
// the function will return true otherwise it is false.
bool Character::attack(Character &defender) 
{

	// return if the attack was successful
	if ((randomIntInRange(1, 100) < m_attack) && 
		(randomIntInRange(1, 100) > defender.m_defense))
	{
		// apply Damage and check for a kill
		applyDamage(defender);
		if ( defender.isDead() )
			m_kills++;
		return true;
	}
	else
	{
		return false;
	}
}

// Used to add hp to the character insures that hp is always zero
// or more and that hp will not go over the characters max hp.
void Character::addHP(int hp)
{
	m_hitPoints += hp;
	if (m_hitPoints < 0)
		m_hitPoints = 0;

	if (m_hitPoints > m_maxHP)
		m_hitPoints = m_maxHP; 
}

// If an attack is successful this function calculates the damage 
// to apply to the enemy
void Character::applyDamage(Character &defender)
{
	// generate a random amount of damage
	int damage = randomIntInRange(5, 10);

	//crit chance 5%
	bool crit = (randomIntInRange(0, 100) % 20 == 0);

	// if a crit happens triple the damage
	// damage is applied to the other character.
	if (crit)
	{
		defender.addHP(-(damage * 3));
	}
	else
	{
		defender.addHP(-(damage));
	}
}

// places a character with new stats at a location
// takes in a row and column  for the map and a level of 
// that character 1-5 for enemies
void Character::spawnNew(int r, int c, int lvl)
{
	switch(lvl)
	{
	case 1: // easy mob
		m_attack	= randomIntInRange(5,40);
		m_defense	= randomIntInRange(5,40);
		m_hitPoints	= randomIntInRange(5,30);
		break;
	case 2:// easy mob
		m_attack	= randomIntInRange(5,50);
		m_defense	= randomIntInRange(5,50);
		m_hitPoints	= randomIntInRange(5,40);
		break;
	case 3: // medium mob
		m_attack	= randomIntInRange(10,60);
		m_defense	= randomIntInRange(10,60);
		m_hitPoints	= randomIntInRange(10,50);
		break;
	case 4: // medium mob
		m_attack	= randomIntInRange(30,70);
		m_defense	= randomIntInRange(30,70);
		m_hitPoints	= randomIntInRange(10,75);
		break;
	case 5: // hard mob
		m_attack	= randomIntInRange(40,80);
		m_defense	= randomIntInRange(40,80);
		m_hitPoints	= randomIntInRange(20,75);
		break;
	default: // player
		m_attack	= randomIntInRange(20,90);
		m_defense	= randomIntInRange(20,90);
		m_hitPoints	= randomIntInRange(30,75);
		break;
	}
	m_maxHP = m_hitPoints;  // set the max hp
	m_level = lvl;			// set the level used for drawing the mob
	m_row = r;				// set row
	m_col = c;				// set column
}

// Name says it all checks if the character is at Zero HP
bool Character::isDead() 
{
	return (m_hitPoints == 0) ? true : false;
}