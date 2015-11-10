#include "Position.h"

Position::Position(uint x, uint y)	//"Position::" gibt an, dass sich
{
	this->x = x;
	this->y = y;
}

bool Position::operator==(Position other)
{
	return 
		this->x == other.x
		&&
		this->y == other.y; 
}

// Operator definieren
Position Position::operator+ (Position other)
{
	return	Position (this->x + other.x, this->y + other.y);
}