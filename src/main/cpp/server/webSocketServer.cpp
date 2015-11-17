#include "webSocketServer.h"

webSocketServer::webSocketServer(boost::asio::io_service& ioService)
	: acceptor(ioService, tcp::endpoint(tcp::v4(), 1234))
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
		newConnection->test();
		newConnection->start();
	}

	startAccept();
}
