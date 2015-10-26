/*
 ============================================================================
 Name        : TCPServer
 Author      : Jonas Kunze (kunze.jonas@gmail.com)
 Description : A simple synchronous TCP server reading up to 255 Bytes and
 responding with a small message in native C
 ============================================================================
 */

#include "Server.h"
#include <iostream>

const int PORT_TEST = 1235;

using namespace std;

int main(int argc, char *argv[]) {


	Server srv(PORT_TEST);

	srv.test();

	srv.start();


	return 0;
}
