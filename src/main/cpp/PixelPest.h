#pragma once

//möglichst wenig in Headerfiles einbinden!!
#include <functional>
#include <vector>
#include "game/Client.h"

class PixelPest
{
private:
	std::vector<Client> clients;

	std::function<void(std::string&&)> onNewConnection(std::function<void(std::string&&)> writeCallback);

	void asignNeighbours();

public:
	PixelPest();
};

