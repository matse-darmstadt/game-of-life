#include "webSocketServer.h"
#include <iostream>

using namespace std;

webSocketServer::webSocketServer(boost::asio::io_service& ioService, std::function<std::function<void(std::string&&)>(std::function<void(std::string&&)>)> onNewConnectionCallback)
	: acceptor(ioService, tcp::endpoint(tcp::v4(), 1234)), onNewConnectionCallback(onNewConnectionCallback)
{
	startAccept();
}

void webSocketServer::startAccept()
{
	webSocketConnection::webSocketConnectionPointer newConnection =
		webSocketConnection::create(acceptor.get_io_service());

	acceptor.async_accept(newConnection->getSocket(),
		boost::bind(&webSocketServer::handleAccept, this, newConnection,
		boost::asio::placeholders::error));

}

void webSocketServer::handleAccept(webSocketConnection::webSocketConnectionPointer newConnection,
	const boost::system::error_code& error)
{
	if (!error)
	{
		//newConnection->test();
		//newConnection->setCallBack([=](string msg){ newConnection->writeMsg(msg); });

		std::function<void(std::string&&)> readCallback = onNewConnectionCallback(std::bind(&webSocketConnection::writeMsg, newConnection.get(), std::placeholders::_1));

		newConnection->setCallBack(readCallback);

		newConnection->start();

		connections.push_back(newConnection);
	}

	startAccept();
}
