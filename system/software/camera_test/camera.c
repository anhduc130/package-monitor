/*
 * camera.c
 *
 *  Created on: Feb 4, 2017
 *      Author: David
 */

#include <stdint.h>
#include "camera.h"

void Camera_Init(void) {
	// Reset
	Camera_Control = 0x03;

	usleep(100000);

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Camera_Control = 0x15;

	// set 38400 Baud
	Camera_Baud = 0x03;

	usleep(100000);
}

int reset() {
  uint8_t args[] = {0x0};
  common_init();
  return runCommand(VC0706_RESET, args, 1, 5, false);
}


int setImageSize(uint8_t x) {
  uint8_t args[] = {0x05, 0x04, 0x01, 0x00, 0x19, x};

  return runCommand(VC0706_WRITE_DATA, args, sizeof(args), 5, false);
}


int takePicture(void) {
	frameptr = 0;
	return cameraFrameBuffCtrl(VC0706_STOPCURRENTFRAME);
}

uint8_t *readPicture(uint8_t n) {
	uint8_t args[] = { 0x0C, 0x0, 0x0A, 0, 0, frameptr >> 8, frameptr & 0xFF, 0,
			0, 0, n, CAMERADELAY >> 8, CAMERADELAY & 0xFF };

	if (!runCommand(VC0706_READ_FBUF, args, sizeof(args), 5, 0))
		return 0;

	// read into the buffer PACKETLEN!
	if (readResponse(n + 5, CAMERADELAY) == 0)
		return 0;

	frameptr += n;

	return camerabuff;
}

uint8_t cameraFrameBuffCtrl(uint8_t command) {
	uint8_t args[] = { 0x1, command };
	return runCommand(VC0706_FBUF_CTRL, args, sizeof(args), 5, true);
}

uint32_t frameLength(void) {
  uint8_t args[] = {0x01, 0x00};
  if (!runCommand(VC0706_GET_FBUF_LEN, args, sizeof(args), 9, true))
    return 0;

  uint32_t len;
  len = camerabuff[5];
  len <<= 8;
  len |= camerabuff[6];
  len <<= 8;
  len |= camerabuff[7];
  len <<= 8;
  len |= camerabuff[8];

  return len;
}

uint8_t readResponse(uint8_t numbytes, uint8_t timeout) {
	//uint8_t counter = 0;
	bufferLen = 0;
	uint8_t counter = 0;
	uint8_t avail = available();

	int i = 0;
	while(1) {
		while((Camera_Status & 0x01) != 0x01) {}
		camerabuff[bufferLen++] = Camera_RxData;
		if(bufferLen == numbytes)
			break;
		if(timeout == counter)
			break;
	}

	printf("Num bytes %u\n", numbytes);
	printf("Timeout %u\n", timeout);
	printf("Avail %u\n",avail);

	for(i = 0; i <numbytes;i++) {
		printf("%x",camerabuff[i]);
	}
	printf("\n");

	if(timeout == counter) {
			printf("Timed out!\n");
	}
	printf("Read %u byte(s)\n", bufferLen);

	return bufferLen;
}

uint8_t readRx() {
	// poll Rx bit in 6850 status register. Wait for it to become '1' or we've timed out
	while ((Camera_Status & 0x01) != 0x01) {}
	uint8_t val = Camera_RxData;
	return val;
}

uint8_t available(void) {
  return bufferLen;
}

void writeTx(uint8_t val) {
	// poll tx bit in 6850 status register. Wait for it to become '2'
	while ((Camera_Status & 0x02) != 0x02) {}
	Camera_TxData = val;
}

void common_init(void) {
	frameptr = 0;
	bufferLen = 0;
	serialNum = 0;
}

uint8_t runCommand(uint8_t cmd, uint8_t *args, uint8_t argn, uint8_t resplen,
		int flushflag) {
	// flush out anything in the buffer?
	if (flushflag) {
		readResponse(100, 10);
	}

	sendCommand(cmd, args, argn);
	if (readResponse(resplen, 200) != resplen)
		return false;
	return true;
}

void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn) {
	writeTx((uint8_t) 0x56);
	writeTx((uint8_t) serialNum);
	writeTx((uint8_t) cmd);
	int i;
	for (i = 0; i < argn; i++) {
		writeTx((uint8_t) args[i]);
	}
}
