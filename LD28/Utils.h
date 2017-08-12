#include <random>

int randomUIntInRange(unsigned int min, unsigned int max)
{
	return min + (rand() % (int)(max - min + 1));
}

int randomIntInRange(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}
