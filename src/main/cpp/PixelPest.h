#pragma once

//m�glichst wenig in Headerfiles einbinden!!
#include <functional>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "game/Client.h"

class PixelPest
{
private:
	std::vector<Client> clients;

	std::function<void(std::string&&)> onNewConnection(std::function<void(std::string&&)> writeCallback);

	void onClientReady();

	void assignNeighbours();

	void logicLoop();

	boost::asio::io_service io_service;

	boost::asio::deadline_timer timer;

	void handleMessage(std::string msg, Client* client);

	void testBoard();

	uint turn;

	uint timerInterval;

public:
	PixelPest();

	bool isPaused();
};

