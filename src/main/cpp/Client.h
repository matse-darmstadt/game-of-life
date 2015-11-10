#pragma once
#include "Board.h"

class Client
{
public:
	int myAdress;

	void send(Board F);

	Client(void);
	~Client(void);		//Destruktor
};

