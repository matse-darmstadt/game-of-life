#include "Client.h"

Client::Client(std::function<void(std::string&&)> sendDataFunction, std::function<void()> onReady)
	: playground(nullptr), sendDataFunction(sendDataFunction), onReady(onReady), paused(true)
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

		onReady();
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
void Client::handleMessage(std::string msg)
{
	if (msg.compare("PAUSE") == 0)
	{
		paused = true;
	}
	else
	{
		paused = false;
		setPopulateFields(msg);
	}
}

bool Client::isPaused()
{
	return paused;
}

