#include "server/webSocketServer.h"
#include "PixelPest.h"
#include <iostream>

int main()
{
	//try
	//{
		boost::asio::io_service io_service;
		webSocketServer server(io_service);
		io_service.run();
	//}
	//catch (std::exception& e)
	//{
		//std::cerr << e.what() << std::endl;
	//}

	return 0;
}


PixelPest::PixelPest(void):running(true)
{
}


PixelPest::~PixelPest(void)
{
}

void PixelPest::run(){

	while(running) {

	}
}