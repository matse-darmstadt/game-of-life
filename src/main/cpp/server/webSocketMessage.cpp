#include "webSocketMessage.h"

#include <boost/asio.hpp>

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
	ulong length = getPayloadLength();

	char* tmp = reinterpret_cast<char*>(alloca(length));

	if (length <= 125) {
		uchar* payload = reinterpret_cast<uchar*>(data) + 6;
		uchar* mask = reinterpret_cast<uchar*>(data) + 2;
		for (uint i = 0; i != length; i++)
			tmp[i] = payload[i] ^ mask[i % 4];
	}

	if (length == 126) {
		uchar* payload = reinterpret_cast<uchar*>(data) + 8;
		uchar* mask = reinterpret_cast<uchar*>(data) + 2;
		for (uint i = 0; i != length; i++)
			tmp[i] = payload[i] ^ mask[i % 4];
	}

	if (length == 127) {
		uchar* payload = reinterpret_cast<uchar*>(data) + 14;
		uchar* mask = reinterpret_cast<uchar*>(data) + 2;
		for (uint i = 0; i != length; i++)
			tmp[i] = payload[i] ^ mask[i % 4];
	}

	return string(tmp, length);
}
