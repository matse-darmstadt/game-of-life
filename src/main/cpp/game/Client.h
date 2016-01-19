#pragma once

#include "Board.h"

#include <functional>

class Board;

class Client
{
private:
	std::function<void(std::string&&)> sendDataFunction;

	std::function<void()> onReady;
	void setPopulateFields(std::string json);
	bool paused;

public:
	Client(std::function<void(std::string&&)> sendDataFunction, std::function<void()> onReady);

	~Client(void);		//Destruktor

	std::string getBoardJson();

	void handleMessage(std::string msg);

	bool isPaused();

	void renderAndSend(uint turn);

	Board* playground;
};

