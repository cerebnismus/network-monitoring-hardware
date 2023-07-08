/*
 * Copyright (c) 2023 Oguzhan Ince
 * Arduino RAW Socket for WizNet5100-based Ethernet shield
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// Tested in Ethernet@2.0.0 -> /src/socketRaw.cpp
// TODO: Ethernet ok Rawnet ?


#include <Arduino.h>
#include "Ethernet.h"
#include "utility/w5100.h"

#if ARDUINO >= 156 && !defined(ARDUINO_ARCH_PIC32)
extern void yield(void);
#else
#define yield()
#endif

// TODO: randomize this when not using DHCP, but how?
static uint16_t local_port = 49152; // 49152 to 65535

typedef struct
{
	uint16_t RX_RSR; // Number of bytes received
	uint16_t RX_RD;	 // Address to read
	uint16_t TX_FSR; // Free space ready for transmit
	uint8_t RX_inc;	 // how much have we advanced RX_RD
} socketstate_t;

static socketstate_t state[MAX_SOCK_NUM];

static uint16_t getSnTX_FSR(uint8_t s);
static uint16_t getSnRX_RSR(uint8_t s);
static void write_data(uint8_t s, uint16_t offset, const uint8_t *data, uint16_t len);
static void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len);


/**
 * @brief	This function used to create in RAW mode
 * @return	1 for success else 0.
 */
void socketRawBegin(uint8_t s, const uint8_t *data, uint16_t len)
{
	uint8_t protocol = SnSR::IPRAW // __SOCKET_REGISTER8(SnPROTO, 0x0014) IP RAW Mode
	uint8_t s, status[MAX_SOCK_NUM], chip, maxindex = MAX_SOCK_NUM chip = W5100.getChip();
	// uint16_t port = 7 											// if needed, otherwise do not use port

	// first check hardware compatibility
	chip = W5100.getChip();
	if (!chip) return MAX_SOCK_NUM; // immediate error if no hardware detected
#if MAX_SOCK_NUM > 4
	if (chip == 51) maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
	Serial.printf("W5100 socket begin, protocol=%d\n", protocol);
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	// look at all the hardware sockets, use any that are closed (unused)
	for (s=0; s < maxindex; s++) {
		status[s] = W5100.readSnSR(s);
		if (status[s] == SnSR::CLOSED) goto makesocket;
	}
	//Serial.printf("W5000socket step2\n");
	// as a last resort, forcibly close any already closing
	for (s=0; s < maxindex; s++) {
		uint8_t stat = status[s];
		if (stat == SnSR::LAST_ACK) goto closemakesocket;
		if (stat == SnSR::TIME_WAIT) goto closemakesocket;
		if (stat == SnSR::FIN_WAIT) goto closemakesocket;
		if (stat == SnSR::CLOSING) goto closemakesocket;
	}
#if 0
	Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
	SPI.endTransaction();
	return MAX_SOCK_NUM; // all sockets are in use
closemakesocket:
	//Serial.printf("W5000socket close\n");
	W5100.execCmdSn(s, Sock_CLOSE);
makesocket:
	//Serial.printf("W5000socket %d\n", s);
	EthernetServer::server_port[s] = 0;
	delayMicroseconds(200); //Is this needed?
	W5100.writeSnMR(s, protocol);
	W5100.writeSnIR(s, 0xFF);

	/*
	if (port > 0) {
		W5100.writeSnPORT(s, port);
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		W5100.writeSnPORT(s, local_port);
	}
	*/

	W5100.execCmdSn(s, Sock_OPEN);
	state[s].RX_RSR = 0;
	state[s].RX_RD  = W5100.readSnRX_RD(s); // always zero?
	state[s].RX_inc = 0;
	state[s].TX_FSR = 0;
	Serial.printf("W5100 socket protocol=%d, RX_RD=%d\n", W5100.readSnMR(s), state[s].RX_RD);
	SPI.endTransaction();
	return s;








	uint16_t ptr = W5100.readSnTX_WR(s);
	uint16_t offset = ptr & W5100.SMASK;
	uint16_t dstAddr = offset + W5100.SBASE[s];

	if (offset + len > W5100.SSIZE)
	{
		// Wrap around circular buffer
		uint16_t size = W5100.SSIZE - offset;
		W5100.write(dstAddr, data, size);
		W5100.write(W5100.SBASE[s], data + size, len - size);
	}
	else
	{
		W5100.write(dstAddr, data, len);
	}

	ptr += len;
	W5100.writeSnTX_WR(s, ptr);

}


uint16_t EthernetClass::socketRawSendAvailable(uint8_t s)
{
	uint8_t status=0;
	uint16_t freesize=0;
	uint16_t freesizeval=0;
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	freesize = getSnTX_FSR(s);
	status = W5100.readSnSR(s);
	SPI.endTransaction();
	if ((status == SnSR::ESTABLISHED) || (status == SnSR::CLOSE_WAIT)) {
		Serial.printf("W5100 socket freesize=%d\n", freesize);
		freesizeval = (freesize & 0xff) << 8 | (freesize & 0xff00) >> 8;
		Serial.printf("W5100 socket freesizeval=%d\n", freesizeval);
		return freesize, freesizeval
	}
	return 0;
}