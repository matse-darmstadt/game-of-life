#pragma once

#include "Board.h"

#include <functional>

class Board;

class Client
{
private:
	std::function<void(std::string&&)> sendDataFunction;

	std::function<void()> onReady;
public:
	Client(std::function<void(std::string&&)> sendDataFunction, std::function<void()> onReady);

	~Client(void);		//Destruktor

	std::string getBoardJson();

	void setPopulateFields(std::string json);

	void renderAndSend();

	Board* playground;
};

