#pragma once

#include <sys/types.h>
#include <bitset>
#include <iostream>
#include <netinet/in.h>

#define uchar unsigned char
#define OPCODE_TEXT 0x1
#define OPCODE_DISCONNECT 0x8

using namespace std;



struct WebSocketHeader {
	uchar Flags :4;uchar OpCode :4;uchar Mask :1;uchar PayloadLength :7;

	uchar getOpCode()
	{
		uchar* data = reinterpret_cast<uchar*>(this);
		return *data & 0x0F; // opcode is in the lowest 4 bits
	}

	void print() {
		for (int i = 0; i != 8; i++) {
			bitset<8> flags(*(reinterpret_cast<char*>(this) + i));
			cout << flags << "\t";

			if ((i + 1) % 4 == 0) {
				cout << endl;
			}
		}
		cout << endl;
	}



	ulong getPayloadLength() {
		uchar* data = reinterpret_cast<uchar*>(this);
		ushort length = *(data + 1) & ((1 << 7) - 1);
		if (length == 126) {
			ushort extended_length = *reinterpret_cast<ushort*>(this) + 1;
			length = ntohs(extended_length);
		}
		if (length == 127) {
			uint super_extended_length = *reinterpret_cast<uint*>(this) + 1;
			length = ntohl(super_extended_length);
		}

		return length;
	}

	string getPayload() {
		ulong length = getPayloadLength();
		char data[length];
		if (length <= 125) {
			uchar* payload = reinterpret_cast<uchar*>(this) + 6;
			uchar* mask = reinterpret_cast<uchar*>(this) + 2;
			for (uint i = 0; i != sizeof(data); i++) {
				data[i] = payload[i] ^ mask[i % 4];
			}
		}
		if (length == 126) {
			uchar* payload = reinterpret_cast<uchar*>(this) + 8;
			uchar* mask = reinterpret_cast<uchar*>(this) + 2;
			for (uint i = 0; i != sizeof(data); i++) {
				data[i] = payload[i] ^ mask[i % 4];
			}
		}
		if (length == 127) {
			uchar* payload = reinterpret_cast<uchar*>(this) + 14;
			uchar* mask = reinterpret_cast<uchar*>(this) + 2;
			for (uint i = 0; i != sizeof(data); i++) {
				data[i] = payload[i] ^ mask[i % 4];
			}
		}
		return string(data, sizeof(data));
	}
}__attribute__((__packed__));
