#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <Windows.h>
#include <vector>
#include <cassert>

#include "Board.h"


using namespace std;

void simpleTest() 
{
	vector<uint> livingX, livingY, firstHalfX, secondHalfX, firstHalfY, secondHalfY;
	uint size = 10;
	Board emptyBoard(size,size);
	assert(emptyBoard.calculateNextStep().size()==0);

	for(uint i = 0; i <size; i++)
	{
		for(uint j = 0; j <size; j++)
		{
			livingX.push_back(i);
			livingY.push_back(j);
		}
	}


	Board boardFullOfLive(size,size);
	boardFullOfLive.setPopulatedBoards(livingX, livingY);
	assert(boardFullOfLive.calculateNextStep().size()==4);
}

void twoBoardsTest1()
{
	vector<uint> firstHalfX, secondHalfX, firstHalfY, secondHalfY;
	uint size = 2;

	firstHalfX.push_back(1);
	firstHalfY.push_back(0);
	firstHalfX.push_back(1);
	firstHalfY.push_back(1);
	secondHalfX.push_back(0);
	secondHalfY.push_back(0);
	secondHalfX.push_back(0);
	secondHalfY.push_back(1);

	Board firstHalfOfLive(size, size);
	firstHalfOfLive.setPopulatedBoards(firstHalfX, firstHalfY);
	Board secondHalfOfLive(size,size);
	secondHalfOfLive.setPopulatedBoards(secondHalfX, secondHalfY);

	assert(firstHalfOfLive.calculateNextStep().size()==0);
	assert(secondHalfOfLive.calculateNextStep().size()==0);

	firstHalfOfLive.setPopulatedBoards(firstHalfX,firstHalfY);
	secondHalfOfLive.setPopulatedBoards(secondHalfX,secondHalfY);

	firstHalfOfLive.setEastBoard(&secondHalfOfLive);
	secondHalfOfLive.setWestBoard(&firstHalfOfLive);

	assert(firstHalfOfLive.calculateNextStep().size()==2);
	assert(secondHalfOfLive.calculateNextStep().size()==2);
}

void twoBoardsTest2()
{
	vector<uint> firstHalfX, secondHalfX, firstHalfY, secondHalfY;
	uint size = 2;

	for(uint i = 0; i <size; i++)
	{
		for(uint j = 0; j <size; j++)
		{
			firstHalfX.push_back(i);
			firstHalfY.push_back(j);
			secondHalfX.push_back(i);
			secondHalfY.push_back(j);
		}
	}
		// Boards zum Testen erstellen
	Board firstHalfOfLive(size, size);
	firstHalfOfLive.setPopulatedBoards(firstHalfX, firstHalfY);
	Board secondHalfOfLive(size,size);
	secondHalfOfLive.setPopulatedBoards(secondHalfX, secondHalfY);
		//Nachprüfen ob nach einem Schritt die Ecken noch "leben"
	assert(secondHalfOfLive.calculateNextStep().size()==4);
	assert(firstHalfOfLive.calculateNextStep().size()==4);	
		//ursprungszustand wieder herstellen
	secondHalfOfLive.setPopulatedBoards(secondHalfX,secondHalfY);
	firstHalfOfLive.setPopulatedBoards(firstHalfX,firstHalfY);
		//Die Nachbarn festlegen
	secondHalfOfLive.setWestBoard(&firstHalfOfLive);
	firstHalfOfLive.setEastBoard(&secondHalfOfLive);
		//nachprüfen ob nur noch die vier "Ecken" des großen Felds "leben"
	auto survivedPositions = secondHalfOfLive.calculateNextStep();
	assert(survivedPositions.size()==2);
	assert(firstHalfOfLive.calculateNextStep().size()==2);
}



void boardTests()						// unit Tests
{
	twoBoardsTest2();
}

/* uint main(uint argc, char* argv[])
{
	//boardTests();

	string testJSON = "{\"width\":1254612,\"height\":3273765,\"populatedFields\":[1,2,3,4]}";

	Board::fromJson(testJSON);

	std::getchar();

	return 0;
}*/

