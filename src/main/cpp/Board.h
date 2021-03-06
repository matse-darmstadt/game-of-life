#pragma once

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <Windows.h>
#include <vector>

#include "Position.h"

#include <cassert>

using namespace std;

class Board
{
private:
	const uint rangeX, rangeY;
	Board* east;
	Board* south;
	Board* west;
	Board* north;

	bool** boardOfLife;
	
	vector<Position> populatedFields;
public:
	static void fromJson(string json);

	Board(uint rangeX, uint rangeY, vector<uint> populatedX, vector<uint> populatedY);

	void setPopulatedBoards(vector<uint> populatedX, vector<uint> populatedY);
	

	// Wir setzen die Verkn�pfungen zwischen den Feldern (Bildschirmen)
	void setWestBoard(Board* westBoard);

	void setEastBoard(Board* eastBoard);
	
	void setSouthBoard(Board* southBoard);

	void setNorthBoard(Board* northBoard);

	Board* getWestBoard();
	
	Board* getEastBoard();
	
	Board* getNorthBoard();
	
	Board* getSouthBoard();


	uint getRangeX();

	uint getRangeY();
	
	vector<Position> calculateNextStep();

	std::string toJson();

private:
	bool isAlive(int x, int y);
	
	uint countNeighbors(uint x, uint y);

	bool willBeAlive(uint x, uint y);

};

