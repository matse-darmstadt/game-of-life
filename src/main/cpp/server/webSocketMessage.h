#pragma once
#ifndef WEB_SOCKET_MESSAGE
#define WEB_SOCKET_MESSAGE

#include <string>

#define ulong unsigned long long
#define uchar unsigned char
#define uint unsigned int
#define ushort unsigned short

using namespace std;

class webSocketMessage
{
public:
	webSocketMessage(char* data);

	ulong getPayloadLength();

	string getPayload();

	char* data;

	ulong getLength();

	ulong getHeaderLength();

	void setPayload(string payload);

	bool isMasked();

	void setMasked(bool masked);

	void setPayloadLength(ulong length);

	void cOutFlags();
};

#endif
