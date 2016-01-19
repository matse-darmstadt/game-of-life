#pragma once

// selbst definierte Kurzschreibweise!
#define uint unsigned int

struct Position
{
	uint x, y;
	Position(uint x, uint y);

	bool operator==(Position other);

	// Operator definieren
	Position operator+ (Position other);
};

