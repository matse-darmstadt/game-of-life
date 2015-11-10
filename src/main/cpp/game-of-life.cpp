/*
 ============================================================================
 Name        : TCPServer
 Author      : Jonas Kunze (kunze.jonas@gmail.com)
 Description : A simple synchronous TCP server reading up to 255 Bytes and
 responding with a small message in native C
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

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
	serv_addr.sin_port = htons(1234); // The port the server will be listening to
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}
	while (true) {
		listen(sockfd, 5);

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			error("ERROR on accept");
		}

		// Receive some message up to 255 Bytes (Byte 255 stays '\0')
		bzero(buffer, 256);
		int n = read(newsockfd, buffer, 255);
		if (n < 0) {
			error("ERROR reading from socket");
		}
		printf("Server received: %s\n", buffer);

		// Response to the message
		n = write(newsockfd, "Some message", 12);
		if (n < 0) {
			error("ERROR writing to socket");
		}
		close(newsockfd);
	}

	close(sockfd);
	return 0;
}
