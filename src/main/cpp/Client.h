#pragma once

#include <functional>

class Board;

class Client
{
private:
	Board* playground;
	std::function<void(std::string)> sendDataFunction;
public:
	Client(std::function<void(std::string)> sendDataFunction);
	~Client(void);		//Destruktor

	std::string getBoardJson();

	void setPopulateFields(std::string json);

	void renderAndSend();
};

