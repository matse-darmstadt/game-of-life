#pragma once

#include <vector>
#include <assert.h>
#include <string>

#include "Position.h"

using namespace std;

class Board
{
private:
	const uint rangeX, rangeY;
	Board* west;
	Board* east;
	Board* north;
	Board* south;

	uint prevTurn;
	bool* prevBoardOfLife;
	bool* boardOfLife;

	vector<Position> populatedFields;

	Board(uint rangeX, uint rangeY);

	void setPopulatedFields(vector<uint> populatedX, vector<uint> populatedY);

	void killAllFields();

	void reviveField(uint x, uint y);

	bool isAlive(int x, int y, uint turn);

	uint countNeighbors(uint x, uint y, uint turn);

	bool willBeAlive(uint x, uint y, uint turn);

	static int getNum(const char* jc);

public:

	static Board* fromJson(string json);

	void setPopulatedFields(string json);


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

	vector<Position> calculateNextStep(uint turn);

	std::string toJson();

};

