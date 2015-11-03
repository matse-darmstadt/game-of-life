/*
 * server.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: dan
 */

#include "Server.h"
#include "WebsocketMessage.h"

#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <iostream>

#include "lib/sha1.h"
#include "lib/base64.h"

Server::Server(int port) :
		running(true) {

	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	/*
	 * Tell Linux that we want to listen to a TCP socket with the port number 1234
	 * and any IP address
	 */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port); // The port the server will be listening to
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

}

Server::~Server() {
}

void Server::error(const char *msg) {
	perror(msg);
	exit(1);
}

void Server::start() {
	int newsockfd;
	socklen_t clilen;
	char buffer[9000];
	struct sockaddr_in cli_addr;
	while (running) {
		listen(sockfd, 5);

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			error("ERROR on accept");
			continue;
		}

		printf("Client connected");
		bzero(buffer, sizeof(buffer));
		int n = read(newsockfd, buffer, sizeof(buffer));
		std::string response;
		if (n < 0) {
			error("ERROR reading from socket");
			goto disconnect;
		}
		printf("\nServer received: %s\n", buffer);

		// Response to the message
		response = generateInitResponse(buffer);
		printf("\nSecond response sent: %s\n", response.c_str());
		if (response == "ERROR") {
			error("ERROR invalid request key");
			goto disconnect;
		}
		n = write(newsockfd, response.c_str(), response.length());
		//n = write(newsockfd, buffer, sizeof(buffer));
		if (n < 0) {
			error("ERROR writing to socket");
			goto disconnect;
		}

		while (running) {
			n = read(newsockfd, buffer, sizeof(buffer));
			if (n == 0) {
				break;
			}
			if (n < 0) {
				error("ERROR reading from socket");
				break;
			}
			buffer[n] = '\0';
			printf("Received message:\n %s\n", buffer);

			WebSocketHeader* wsh = reinterpret_cast<WebSocketHeader*>(buffer);
			wsh->print();

			if (wsh->getOpCode() == OPCODE_TEXT) {
				cout << "\npayload length: " << (int) wsh->getPayloadLength()
						<< endl;
				cout << "\npayload: " << wsh->getPayload() << endl;
			} else if (wsh->getOpCode() == OPCODE_DISCONNECT) {
				goto disconnect;
			}

		}
		disconnect: close(newsockfd);
		printf("Client disconnected\n");
	}

	close(sockfd);
}

void Server::shutDown() {
	running = false;

	close(sockfd);
}

const string SOCKET_KEY_TITLE = "Sec-WebSocket-Key: ";
const size_t KEY_LENGTH = 24;
const string WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void Server::test() {
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
	assert(getKey(EXAMPLE_REQUEST) == EXAMPLE_REQUEST_KEY);

	assert(generateAcceptKey(EXAMPLE_REQUEST_KEY) == EXAMPLE_ACCEPT_KEY);
	std::cout << "all tests successful" << std::endl;

}

string Server::getKey(string header) {
	size_t found = header.find(SOCKET_KEY_TITLE);
	if (found == std::string::npos)
		return "ERROR";
	found += SOCKET_KEY_TITLE.length();
	return header.substr(found, KEY_LENGTH);
}

string Server::generateAcceptKey(string requestKey) {
	requestKey += WEBSOCKET_GUID;
	unsigned long size = 20;
	unsigned char buffer[size];
	sha1::calc(requestKey.c_str(), requestKey.length(), buffer);

	size_t encoded_size = 0;
	char* encoded = base64_encode(buffer, size, encoded_size);
	string acceptKey(encoded, encoded_size);
	free(encoded);
	return acceptKey;
}

string Server::generateInitResponse(string header) {
	string requestKey = getKey(header);
	if (requestKey == "ERROR")
		return "ERROR";
	string acceptKey = generateAcceptKey(requestKey);

	string response = "HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: ";

	response += acceptKey;
	response +=
			"\r\n"
					"Access-Control-Allow-Origin: *\r\n"
					"Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
					"Access-Control-Allow-Headers: Content-Type\r\n"
					"Access-Control-Request-Headers: X-Requested-With, accept, content-type\r\n\r\n";

	return response;
}
