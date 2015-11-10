#pragma once
#ifndef WEB_SOCKET_SERVER
#define WEB_SOCKET_SERVER

#include "webSocketConnection.h"

class webSocketServer
{
public:
	webSocketServer(boost::asio::io_service& ioService);

private:
	void startAccept();

	void handleAccept(webSocketConnection::webSocketConnectionPointer newConnection,
		const boost::system::error_code& error);

	tcp::acceptor acceptor;
};

#endif WEB_SOCKET_SERVER
