#ifndef CHARACTER_H
#define CHARACTER_H
// The character class represents a player or enemy in the game
// it contains all the attributes and functions that are used
// for combat in the game.

class Character
{
public:
	Character();

	bool attack(Character &defender);
	void addHP(int hp);
	void applyDamage(Character &defender);

	// Getters
	int getRow()		{ return m_row;		 };
	int getCol()		{ return m_col;		 };
	int getHP()			{ return m_hitPoints;};
	int getMaxHP()		{return m_maxHP;	 };
	int getATT()		{ return m_attack;	 };
	int getDEF()		{return m_defense;	 };
	int getkills()		{ return m_kills;	 };
	int getLevel()		{ return m_level;	 };
	int getPotions()	{ return m_potions;  };

	// Setters
	void setPotions(int p) { (p >= 0) ? m_potions = p : m_potions = 0; };
	void setRow(int r) { m_row = r; };
	void setCol(int c) { m_col = c; };

	// places a character with new stats at a location
	void spawnNew(int r, int c, int lvl);

	// returns if player is dead
	bool isDead();

private:
	int m_level;
	int m_potions;
	int m_maxHP;
	int m_hitPoints;
	int m_attack;
	int m_defense;
	int m_row;
	int m_col;
	int m_kills;

};

#endif