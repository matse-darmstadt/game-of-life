/*
 ============================================================================
 Name        : TCPServer
 Author      : Jonas Kunze (kunze.jonas@gmail.com)
 Description : A simple synchronous TCP server reading up to 255 Bytes and
 responding with a small message in native C
 ============================================================================
 */

#include "Server.h"

#include <signal.h>
#include <iostream>

const int PORT_TEST = 1233;

using namespace std;

Server srv(PORT_TEST);

void signalHandler(int signal) {
	cout << "Received signal " << signal << endl;
	srv.shutDown();
	exit (signal);
}

int main(int argc, char *argv[]) {

	signal(SIGINT, signalHandler);
	signal(SIGKILL, signalHandler);
	signal(SIGQUIT, signalHandler);

	srv.test();

	srv.start();

	return 0;
}
