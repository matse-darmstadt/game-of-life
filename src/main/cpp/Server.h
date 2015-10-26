/*
 * server.h
 *
 *  Created on: Oct 26, 2015
 *      Author: dan
 */

#ifndef MAIN_CPP_SERVER_H_
#define MAIN_CPP_SERVER_H_

#include <string>

using namespace std;

class Server {
public:
	Server(int port);
	virtual ~Server();
	void start();
	void test();
private:
	void error(const char *msg);
	int sockfd;
	string getKey(string header);
	string generateAcceptKey(string requestKey);
	string generateInitResponse(string requestHeader);
};

#endif /* MAIN_CPP_SERVER_H_ */
