#pragma once
#ifndef WEB_SOCKET_CONNECTION
#define WEB_SOCKET_CONNECTION

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "webSocketMessage.h"

using boost::asio::ip::tcp;

using namespace std;

class webSocketConnection
	: public boost::enable_shared_from_this<webSocketConnection>
{
public:
	typedef boost::shared_ptr<webSocketConnection> webSocketConnectionPointer;

	static webSocketConnectionPointer create(boost::asio::io_service& ioService);

	tcp::socket& getSocket();

	void start();

	void test();

private:
	webSocketConnection(boost::asio::io_service& ioService);

	void handleWrite(const boost::system::error_code& error, size_t length);

	void handleRead(const boost::system::error_code& error, size_t length);

	uint generateInitialResponse(char* requestHeader);

	string generateAcceptKey(string requestKey);

	string getRequestKey(char* requestHeader);

	tcp::socket socket;

	char buffer[9000];
};

#endif WEB_SOCKET_CONNECTION
