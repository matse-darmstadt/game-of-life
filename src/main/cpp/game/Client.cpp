#include "Client.h"

Client::Client(std::function<void(std::string&&)> sendDataFunction): playground(nullptr), sendDataFunction(sendDataFunction)
{

}


Client::~Client(void)
{
}

void Client::setPopulateFields(std::string json)
{
	if(playground == nullptr)
	{
		playground = Board::fromJson(json);
	}
	else
	{
		playground->setPopulatedFields(json);
	}
}

void Client::renderAndSend()
{
	if(playground != nullptr)
	{
		playground->calculateNextStep();
		sendDataFunction(playground->toJson());
	}
}


