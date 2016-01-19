#include "PixelPest.h"
#include "server/webSocketServer.h"
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/bind.hpp>
#include <iostream>

int main()
{
	PixelPest server;
	return 0;
}

PixelPest::PixelPest()
	: timer(io_service), turn(1), timerInterval(50)
{
	clients.reserve(1024);

	webSocketServer server(io_service, std::bind(&PixelPest::onNewConnection, this, placeholders::_1));

	timer.expires_from_now(boost::posix_time::milliseconds(timerInterval));
	timer.async_wait(boost::bind(&PixelPest::logicLoop, this));

	//TEST
	testBoard();

	io_service.run();
}

void PixelPest::logicLoop()
{
	timer.expires_from_now(boost::posix_time::milliseconds(timerInterval));
	timer.async_wait(boost::bind(&PixelPest::logicLoop, this));

	if (!isPaused())
	{
		for (auto& client: clients)
		{
			client.renderAndSend(turn);
		}
		turn++;
	}
}

std::function<void(std::string&&)> PixelPest::onNewConnection(std::function<void(std::string&&)> writeCallback)
{
	Client newClient(writeCallback, std::bind(&PixelPest::onClientReady, this));

	clients.push_back(newClient);

	return std::bind(&PixelPest::handleMessage, this, placeholders::_1, &clients[clients.size() - 1]);
}

void PixelPest::handleMessage(std::string msg, Client* client)
{
	std::string prefix = msg.substr(0, 2);

	if (prefix == "--")
	{
		std::string command = msg.substr(2, 3);

		if (command == "FPS")
		{
			float fps = atof(msg.substr(6).c_str());

			timerInterval = 1000 / fps;
		}
	}
	else
		client->handleMessage(msg);
}

void PixelPest::onClientReady()
{
	assignNeighbours();
}

void PixelPest::assignNeighbours()
{
	int size = clients.size();

	for (int i = 0; i < size; i++)
	{
		if (clients[i].playground == NULL)
			return;

		int left = i - 1;
		if (left == -1)
			left = size - 1;

		int right = i + 1;
		if (right == size)
			right = 0;

		clients[i].playground->setWestBoard(clients[left].playground);
		clients[i].playground->setEastBoard(clients[right].playground);
	}
}

bool PixelPest::isPaused()
{
	for (int i = 0; i < clients.size(); i++)
		if (clients[i].isPaused())
			return true;

	return false;
}

void PixelPest::testBoard()
{
	Board* test = Board::fromJson("{\"width\":90,\"height\":97,\"populatedFields\":[0,0,1,0,3,0,0,1]}");

	test->calculateNextStep(1);

	assert(test->toJson() == "{\"width\":90,\"height\":97,\"populatedFields\":[0,0,0,1,1,0,1,1]}");

	test->calculateNextStep(2);

	assert(test->toJson() == "{\"width\":90,\"height\":97,\"populatedFields\":[0,0,0,1,1,0,1,1]}");
}

