#include "PixelPest.h"
#include "server/webSocketServer.h"
#include <iostream>

int main()
{
	PixelPest server = PixelPest();

	return 0;
}

PixelPest::PixelPest()
{
	// start websocket server
	boost::asio::io_service io_service;
	webSocketServer server(io_service, std::bind(&PixelPest::onNewConnection, this, placeholders::_1));
	io_service.run();
}

std::function<void(std::string&&)> PixelPest::onNewConnection(std::function<void(std::string&&)> writeCallback)
{
	Client newClient = Client(writeCallback, std::bind(&PixelPest::onClientReady, this));

	clients.push_back(&newClient);

	return std::bind(&Client::setPopulateFields, newClient, placeholders::_1);
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
		int left = i - 1;
		if (left == -1)
			left = size - 1;

		int right = i + 1;
		if (right == size)
			right = 0;

		clients[i]->playground->setWestBoard(clients[left]->playground);
		clients[i]->playground->setEastBoard(clients[right]->playground);
	}
}
