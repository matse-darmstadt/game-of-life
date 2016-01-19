#pragma once
#ifndef WEB_SOCKET_SERVER
#define WEB_SOCKET_SERVER

#include "webSocketConnection.h"

class webSocketServer
{
public:
	webSocketServer(boost::asio::io_service& ioService, std::function<std::function<void(std::string&&)>(std::function<void(std::string&&)>)> onNewConnectionCallback);

private:
	void startAccept();

	void handleAccept(webSocketConnection::webSocketConnectionPointer newConnection,
		const boost::system::error_code& error);

	tcp::acceptor acceptor;

	std::function<std::function<void(std::string&&)>(std::function<void(std::string&&)>)> onNewConnectionCallback;

	std::vector<webSocketConnection::webSocketConnectionPointer> connections;
};

#endif WEB_SOCKET_SERVER
