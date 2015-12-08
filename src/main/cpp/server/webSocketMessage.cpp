#include "webSocketMessage.h"
#ifdef __LINUX
#include <endian.h>
#define htonll htobe64
#else
#include <WinSock2.h>
#endif
#include <boost/asio.hpp>
#include <bitset>
#include <iostream>

webSocketMessage::webSocketMessage(char* data)
	: data(data) { }

ulong webSocketMessage::getPayloadLength()
{
	uchar* tmp = reinterpret_cast<uchar*>(data);

	ushort length = *(tmp + 1) & ((1 << 7) - 1);

	if (length == 126) {
		ushort extended_length = *reinterpret_cast<ushort*>(data) + 1;
		length = ntohs(extended_length);
	}

	if (length == 127) {
		uint super_extended_length = *reinterpret_cast<uint*>(data) + 1;
		length = ntohl(super_extended_length);
	}

	return length;
}

string webSocketMessage::getPayload()
{
	if (!isMasked())
		return string(data + getHeaderLength(), getPayloadLength());

	ulong length = getPayloadLength();

	char* tmp = reinterpret_cast<char*>(alloca(length));

	uchar* payload = reinterpret_cast<uchar*>(data) + getHeaderLength();
	uchar* mask = reinterpret_cast<uchar*>(data) + 2;
	for (uint i = 0; i != length; i++)
		tmp[i] = payload[i] ^ mask[i % 4];

	return string(tmp, length);
}

ulong webSocketMessage::getHeaderLength()
{
	uchar* tmp = reinterpret_cast<uchar*>(data);

	ushort payloadLength = *(tmp + 1) & ((1 << 7) - 1);
	ushort headerLength = 2; // flags (4 bit) + opcode (4 bit) + mask flag + payload length (7 bit)

	if (isMasked())
		headerLength += 4;

	if (payloadLength == 126)
		return headerLength + 2;

	if (payloadLength == 127)
		return headerLength + 8;

	return headerLength;
}

ulong webSocketMessage::getLength()
{
	return getPayloadLength() + getHeaderLength();
}

void webSocketMessage::setPayloadLength(ulong length)
{
	uchar payloadLength = length;
	if (length > 0xffff)
	{
		payloadLength = 127;
		ulong& xtendedLength = *reinterpret_cast<ulong*>(data + 2);
		xtendedLength = htonll(length);
	}
	else if (length > 125)
	{
		payloadLength = 126;
		ushort& xtendedLength = *reinterpret_cast<ushort*>(data + 2);
		xtendedLength = htons(length);
	}

	*(data + 1) = (isMasked() << 7) | payloadLength;
}

bool webSocketMessage::isMasked()
{
	char secondByte = *(data + 1);

	return secondByte & (1 << 7);
}

void webSocketMessage::setMasked(bool masked)
{
	char& secondByte = *(data+1);

	secondByte = secondByte & (0xff >> 1);

	secondByte = secondByte | (masked << 7);
}

void webSocketMessage::setPayload(string payload)
{
	setPayloadLength(payload.size() + 1);

	memcpy(data + getHeaderLength(), payload.c_str(), payload.size() + 1);
}

void webSocketMessage::cOutFlags()
{
	short firstWord = *reinterpret_cast<short*>(data);

	cout << bitset<16>(firstWord) << endl;
}
