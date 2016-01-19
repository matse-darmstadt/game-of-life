#include "Board.h"
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>


Board::Board(uint rangeX, uint rangeY):
	rangeX(rangeX),
	rangeY(rangeY),
	west(nullptr), east(nullptr), north(nullptr), south(nullptr)
{


	//new alloziert speicher im Heap <---------------------------
	boardOfLife = new bool*[rangeX];			// boardOfLife ist pointer zu einem array von pointern
	for(uint x = 0; x <rangeX; x++)
	{
		boardOfLife[x] = new bool[rangeY];		// boardOfLife[x] ist ein array von pointern
		for(uint y = 0; y <rangeY; y++)
		{
			boardOfLife[x][y] = false;
		}
	}
}

Board* Board::fromJson(string json) {
	const char* json_c = json.c_str();
	// json format: 		{"width":90,"height":97,"populatedFields":[0,0,1,0,3,0,0,1]}
	// alternate format:	{"populatedFields":[0,0,1,0,3,0,0,1],"height":97,"width":90}
	int width = -1, height = -1;
	do
	{
		json_c = strchr(json_c, '"') + 1; // opening quotes
		if (strncmp(json_c, "width", 5) == 0)
			width = getNum(json_c);
		if (strncmp(json_c, "height", 6) == 0)
			height = getNum(json_c);
		json_c = strchr(json_c, '"') + 1; // closing quotes
	} while (width == -1 || height == -1);
	// reading the array of coordinates on the board
	Board* newBoard = new Board(width, height);
	newBoard->setPopulatedFields(json);
	return newBoard;
}

int Board::getNum(const char* jc)
{
	jc = strchr(jc, ':') + 1;
	return atoi(jc);
}

void Board::setPopulatedFields(string json) {
	killAllFields();
	const char* json_c = json.c_str();
	json_c = strchr(json_c, '[')+1;
	if(*json_c != ']'){
		do{
			int x = atoi(json_c);
			json_c = strchr(json_c, ',')+1;
			int y = atoi(json_c);
			reviveField(x,y);
			// jump to next ,
			// if it does not exist we are done with the array (json_c will be null)
			json_c = strchr(json_c, ',')+1;
		} while(json_c-1!=NULL);
	}
}

void Board::killAllFields()
{
	for(uint x = 0; x <rangeX; x++)
	{
		for(uint y = 0; y <rangeY; y++)
		{
			boardOfLife[x][y]=false;
		}
	}
}

void Board::reviveField(uint x,uint y)
{
	if(x<rangeX || x>=0 ||y<rangeY || y>=0)
	{
		boardOfLife[x][y]=true;
	}
	else
	{
		cerr << "tried to revive field (" <<  x <<","<< y <<"), outside of range:  y =" << rangeY <<", x =" <<rangeX<< endl;
	}
}


/**
 * generates a string like following:
 * {"width":<width>,"height":<height>,"populatedFields":[x1,y1,x2,y2,...]}
 */
string Board::toJson()
{
					// cString: ...\0
					// stringStream: <--- gut f�r einen String der erweitert werden soll (allozieren)
	stringstream jsonStream;

	jsonStream<< "{\"width\":" << rangeX<< ",\"height\":" << rangeY << ",\"populatedFields\":[";
	bool isFirstPosition = true;
	for(Position& position: populatedFields)
	{
		if(isFirstPosition){						//Trick um etwas nur das erste mal zu tun!
			isFirstPosition=false;
		}
		else
		{
			jsonStream << ',' ;
		}
		jsonStream << position.x << ',' << position.y;
	}

	jsonStream<<"]}";

	return jsonStream.str();
}

void Board::setPopulatedFields(vector<uint> populatedX, vector<uint> populatedY)
	{
		assert(populatedX.size()==populatedY.size());
		for(uint boxNumber = 0; boxNumber<populatedX.size(); boxNumber++)
		{
			uint x = populatedX[boxNumber];
			uint y = populatedY[boxNumber];
			boardOfLife[x][y] = true;
		}
	}

void Board::setWestBoard(Board* westBoard)
	{
		west = westBoard;
	}

void Board::setEastBoard(Board* eastBoard)
	{
		east = eastBoard;
	}

void Board::setSouthBoard(Board* southBoard)
{
		south = southBoard;
	}

void Board::setNorthBoard(Board* northBoard)
	{
		north = northBoard;
	}

Board* Board::getWestBoard()
{
	return west;
}

Board* Board::getEastBoard()
{
	return east;
}

Board* Board::getNorthBoard()
{
	return north;
}

Board* Board::getSouthBoard()
{
	return south;
}

uint Board::getRangeX()
{
	return rangeX;
}

uint Board::getRangeY()
{
	return rangeY;
}

vector<Position> Board::calculateNextStep()
{
	populatedFields.clear();

	vector<Position> dyingFields;
	for(uint x = 0; x <rangeX; x++)
	{
		for(uint y = 0; y <rangeY; y++)
		{
			if(willBeAlive(x,y)){
				populatedFields.emplace_back(x,y);
			} else {
				dyingFields.emplace_back(x,y);
			}
		}
	}

	// kill all fields that died this turn
	for(auto& field: dyingFields)
	{
		boardOfLife[field.x][field.y] = false;
	}

	// revive all fields that stayed alive or were animated this turn
	for (auto& field : populatedFields)
	{
		boardOfLife[field.x][field.y] = true;
	}

	return populatedFields;
}

bool Board::isAlive(int x, int y)
	{
		if(x<0)
		{
			if(west!=nullptr){
				return west->isAlive(west->rangeX-1,y);
			}
			return false;
		}
		if(y<0)
		{
			if(north!=nullptr){
				return north->isAlive(x,north->rangeY-1);
			}
			return false;
		}
		if(x>(int)rangeX-1)
		{
			if(east!=nullptr){
				return east->isAlive(0,y);
			}
			return false;
		}
		if(y>(int)rangeY-1)
		{
			if(south!=nullptr){
				return south->isAlive(x,0);
			}
			return false;
		}
		return boardOfLife[x][y];
	}

uint Board::countNeighbors(uint x, uint y)
	{
		uint countingNeighbors = 0;
		for(int dx = -1; dx<2; dx++){
			for(int dy = -1; dy<2; dy++){
				if((dx==0 && dy==0)==false)			// oder (dx!=0 || dy!=0)
				{
					if(isAlive(x+dx,y+dy))
					{
						countingNeighbors++;
					}
				}
			}
		}
		return countingNeighbors;
	}

bool Board::willBeAlive(uint x, uint y)						//
	{
		uint numberOfNeighbors = countNeighbors(x,y);
		if(numberOfNeighbors<2 || numberOfNeighbors>3)
		{
			return false;
		}
		if(numberOfNeighbors == 3)
		{
			return true;
		}
		return boardOfLife[x][y];
	}

// Methoden sollten immer den Status �ndern (logik implementierern) ODER den aktuallen Status ausgeben, nicht beides!
// Eine Methode sollte im Idealfall nur eine Sache tun!
