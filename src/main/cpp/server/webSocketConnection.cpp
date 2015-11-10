﻿#include "webSocketConnection.h"

#include "../libs/sha1.h"
#include "../libs/base64.h"

const string SOCKET_KEY_TITLE = "Sec-WebSocket-Key: ";
const size_t KEY_LENGTH = 24;
const string WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

webSocketConnection::webSocketConnectionPointer webSocketConnection::create(boost::asio::io_service& ioService)
{
	return webSocketConnectionPointer(new webSocketConnection(ioService));
}

tcp::socket& webSocketConnection::getSocket()
{
	return socket;
}

void webSocketConnection::start()
{
	auto self = shared_from_this();

	socket.async_read_some(boost::asio::buffer(readBuffer, sizeof(readBuffer)),
		[this, self](boost::system::error_code error, std::size_t length)
	{
		if (error)
			return;

		cout << "Received message length: " << length << "\r\n";

		readBuffer[length] = '\0';

		cout << "\r\nRequest:\r\n" << readBuffer;

		writeData = generateInitialResponse(readBuffer);

		boost::asio::async_write(socket, boost::asio::buffer(writeData),
			boost::bind(&webSocketConnection::handleWrite, self,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	});
}

string webSocketConnection::getRequestKey(string requestHeader)
{
	size_t found = requestHeader.find(SOCKET_KEY_TITLE);

	if (found == std::string::npos)
		return "ERROR";

	found += SOCKET_KEY_TITLE.length();

	return requestHeader.substr(found, KEY_LENGTH);
}

string webSocketConnection::generateAcceptKey(string requestKey)
{
	requestKey += WEBSOCKET_GUID;

	const unsigned long size = 20;

	unsigned char buffer[size];

	sha1::calc(requestKey.c_str(), requestKey.length(), buffer);

	size_t encoded_size = 0;

	char* encoded = base64_encode(buffer, size, encoded_size);

	string acceptKey(encoded, encoded_size);

	free(encoded);

	return acceptKey;
}

string webSocketConnection::generateInitialResponse(string requestHeader)
{
	string requestKey = getRequestKey(requestHeader);

	if (requestKey == "ERROR")
		return "ERROR";

	string response = "HTTP / 1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: " + generateAcceptKey(requestKey) + "\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
		"Access-Control-Allow-Headers: Content-Type\r\n"
		"Access-Control-Request-Headers: X-Requested-With, accept, content-type\r\n\r\n";

	return response;
}

webSocketConnection::webSocketConnection(boost::asio::io_service& ioService)
	: socket(ioService)
{
}

void webSocketConnection::handleWrite(const boost::system::error_code& error, size_t length)
{
	if (error)
		return;

	cout << "\r\nResponse:\r\n" << writeData;

	socket.async_read_some(boost::asio::buffer(readBuffer, sizeof(readBuffer)),
		boost::bind(&webSocketConnection::handleRead, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void webSocketConnection::handleRead(const boost::system::error_code& error, size_t length)
{
	if (error)
		return;

	readBuffer[length] = '\0';

	cout << "\r\nRequest:\r\n" << readBuffer;

	writeData = generateInitialResponse(readBuffer);

	boost::asio::async_write(socket, boost::asio::buffer(writeData),
		boost::bind(&webSocketConnection::handleWrite, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void webSocketConnection::test() {
	string EXAMPLE_REQUEST = "GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
		"Sec-WebSocket-Protocol: chat, superchat\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"Origin: http://example.com\r\n";
	string EXAMPLE_REQUEST_KEY = "x3JJHMbDL1EzLkh9GBhXDw==";
	string EXAMPLE_ACCEPT_KEY = "HSmrc0sMlYUkAGmm5OPpG2HaGWk=";
	assert(getRequestKey(EXAMPLE_REQUEST) == EXAMPLE_REQUEST_KEY);

	assert(generateAcceptKey(EXAMPLE_REQUEST_KEY) == EXAMPLE_ACCEPT_KEY);
	std::cout << "all tests successful" << std::endl;

}