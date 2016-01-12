#pragma once

#include "Board.h"

#include <functional>

class Board;

class Client
{
private:
	std::function<void(std::string&&)> sendDataFunction;
public:
	Client(std::function<void(std::string&&)> sendDataFunction);
	~Client(void);		//Destruktor

	std::string getBoardJson();

	void setPopulateFields(std::string json);

	void renderAndSend();

	Board* playground;
};

