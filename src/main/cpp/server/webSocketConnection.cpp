#include "webSocketConnection.h"

#include "../libs/sha1.h"
#include "../libs/base64.h"
#include <iostream>

using namespace std;

const char SOCKET_KEY_TITLE[]  = "Sec-WebSocket-Key: ";
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

webSocketConnection::webSocketConnection(boost::asio::io_service& ioService)
	: socket(ioService) { }

#pragma region Connection intializing

void webSocketConnection::start()
{
	auto self = shared_from_this();

	socket.async_read_some(boost::asio::buffer(readBuffer, sizeof(readBuffer)),
		[this, self](boost::system::error_code error, std::size_t length)
	{
		if (error)
			return;

		readBuffer[length] = '\0';

		//cout << "REQUEST\r\n-------------------------\r\n" << string(readBuffer, length) << "\r\n";

		uint responseSize = generateInitialResponse(readBuffer);

		boost::asio::async_write(socket, boost::asio::buffer(readBuffer, responseSize),
			boost::bind(&webSocketConnection::handleInitialWrite, self,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	});
}

string webSocketConnection::getRequestKey(char* requestHeader)
{
	char* key = strstr(requestHeader, SOCKET_KEY_TITLE);

	if (key == NULL)
		return "ERROR";

	key += sizeof(SOCKET_KEY_TITLE);

	return string(key - 1, KEY_LENGTH);
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

uint webSocketConnection::generateInitialResponse(char* requestHeader)
{
	string requestKey = getRequestKey(requestHeader);

	if (requestKey == "ERROR")
		return 0;

	string response = "HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: " + generateAcceptKey(requestKey) + "\r\n"
		//"Sec-Websocket-Protocol: chat\r\n" // <- not included, if not present in request
		"Accept-Encoding:\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		//"Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
		//"Access-Control-Allow-Headers: Content-Type\r\n"
		//"Access-Control-Request-Headers: X-Requested-With, accept, content-type\r\n"
		"\r\n";

	memcpy(requestHeader, response.c_str(), response.size());

	return response.size();
}

void webSocketConnection::handleInitialWrite(const boost::system::error_code& error, size_t length)
{
	if (error)
		return;

	//cout << "RESPONSE\r\n-------------------------\r\n" << string(readBuffer, length) << "\r\n";

	socket.async_read_some(boost::asio::buffer(readBuffer, sizeof(readBuffer)),
		boost::bind(&webSocketConnection::handleRead, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

//	webSocketMessage msg(buffer);
//
//	msg.setMasked(false);
//
//	msg.setPayload("HELLO");
//
//	boost::asio::async_write(socket, boost::asio::buffer(buffer, msg.getLength()),
//		boost::bind(&webSocketConnection::handleWrite, shared_from_this(),
//		boost::asio::placeholders::error,
//		boost::asio::placeholders::bytes_transferred));
}

#pragma endregion

#pragma region Lifecycle

void webSocketConnection::handleRead(const boost::system::error_code& error, size_t length)
{
	if (error)
		return;

	webSocketMessage msg(readBuffer);

	//msg.cOutFlags();

	// here we get our board or a client request to pause
	//cout << "Received data: "<<	msg.getPayload() << endl;

	// use the callback function on the payload and send it back to the client
	// or do nothing on PAUSE
	readCallBack(msg.getPayload());

	socket.async_read_some(boost::asio::buffer(readBuffer, sizeof(readBuffer)),
		boost::bind(&webSocketConnection::handleRead, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void webSocketConnection::handleWrite(const boost::system::error_code& error, size_t length)
{
	if (error)
		return;

	webSocketMessage msg(writeBuffer);

	//cout << "Send data: " << msg.getPayload() << endl;
//
//	socket.async_read_some(boost::asio::buffer(buffer, sizeof(buffer)),
//		boost::bind(&webSocketConnection::handleRead, shared_from_this(),
//		boost::asio::placeholders::error,
//		boost::asio::placeholders::bytes_transferred));
}

void webSocketConnection::writeMsg(string payload)
{
	webSocketMessage msg(writeBuffer);

	//msg.setMasked(false);
	msg.setResponseHeader();

	msg.setPayload(payload);

	//msg.cOutFlags();

	//cout << "Try to send data: " << msg.getPayload() << endl;

	boost::asio::async_write(socket, boost::asio::buffer(writeBuffer, msg.getLength()),
			boost::bind(&webSocketConnection::handleWrite, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

#pragma endregion

//void webSocketConnection::test() {
//	char* EXAMPLE_REQUEST = "GET /chat HTTP/1.1\r\n"
//		"Host: server.example.com\r\n"
//		"Upgrade: websocket\r\n"
//		"Connection: Upgrade\r\n"
//		"Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
//		"Sec-WebSocket-Protocol: chat, superchat\r\n"
//		"Sec-WebSocket-Version: 13\r\n"
//		"Origin: http://example.com\r\n";
//	string EXAMPLE_REQUEST_KEY = "x3JJHMbDL1EzLkh9GBhXDw==";
//	string EXAMPLE_ACCEPT_KEY = "HSmrc0sMlYUkAGmm5OPpG2HaGWk=";
//	assert(getRequestKey(EXAMPLE_REQUEST) == EXAMPLE_REQUEST_KEY);
//
//	assert(generateAcceptKey(EXAMPLE_REQUEST_KEY) == EXAMPLE_ACCEPT_KEY);
//
//	char testBuffer[100];
//
//	webSocketMessage msg(testBuffer);
//
//	msg.setMasked(true);
//
//	assert(msg.isMasked());
//
//	msg.setMasked(false);
//
//	assert(msg.isMasked() == false);
//
//	msg.setPayload("HANS");
//
//	assert(msg.getPayload() == "HANS");
//
//	std::cout << "all tests successful" << std::endl;
//
//}
