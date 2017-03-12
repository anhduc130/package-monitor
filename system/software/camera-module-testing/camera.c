/*
 * camera.c
 *
 *  Created on: Feb 4, 2017
 *      Author: Andrew Bui
 */

#include <stdint.h>
#include "camera.h"

void Camera_Init(void) {
	// Reset
	Camera_Control = 0x03;

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Camera_Control = 0x15;

	// set 38400 Baud
	Camera_Baud = 0x03; // 0x03 is for 38400 and 0x07 is for 9600
	usleep(100000);
}

int reset(void) {
	int i;
	uint8_t firstData;
	printf("Reset the camera:\n");
	uint8_t send[4] = {
	      0x56,
	      0x00,
	      0x26,
	      0x00
	};
	uint8_t receive[4];
	writeTx(0x56);
	writeTx(0x00);
	writeTx(0x26);
	writeTx(0x00);
	/*
	for (i = 0; i < 4; i++) {
	    writeTx(send[i]);
	}
	*/
	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 4; i++){
				receive[i] = readRx();
			}
			break;
		}
	}
	/*
	for (i = 0; i < 4; i++){
	  printf("data: %02X\n", receive[i]);
	}
	*/
	return 1;
}

int changeImageSize(void){
	int i;
	uint8_t firstData;
	printf("Change size:\n");
	uint8_t send[5] = {
	      0x56,
	      0x00,
	      0x54,
	      0x01,
	      0x22
	};
	uint8_t receive[5];
	for (i = 0; i < 5; i++) {
	    writeTx(send[i]);
	}
	
	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 5; i++){
				receive[i] = readRx();
			}
			break;
		}
	}
	for (i = 0; i < 5; i++){
	  printf("data: %02X\n", receive[i]);
	}
	
	return 1;
}

int takePicture(void){
	int i;
	uint8_t firstData;
	printf("Take picture now:\n");
	uint8_t send[5] = {
	      0x56,
	      0x00,
	      0x36,
	      0x01,
	      0x00
	};

	uint8_t receive[5];
	writeTx(0x56);
	writeTx(0x00);
	writeTx(0x36);
	writeTx(0x01);
	writeTx(0x00);
	/*
	for (i = 0; i < 5; i++) {
	      writeTx(send[i]);
	}
	*/
	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 5; i++){
				receive[i] = readRx();
			}
			break;
		}
	}
	/*
	for (i = 0; i < 5; i++){
	  printf("data: %02X\n", receive[i]);
	}
	*/
	return 1;
}

int changeBaudRate(void){
	int i;
	uint8_t firstData;
	printf("Change baud rate: \n");
	uint8_t send[7] = {
	      0x56,
	      0x00,
	      0x24,
	      0x03,
	      0x01,
	      0x2A,
	      0xF2
	};

	uint8_t receive[5];

	for (i = 0; i < 7; i++) {
	      writeTx(send[i]);
	}

	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 5; i++){
				receive[i] = readRx();
			}
			break;
		}
	}
	
	for (i = 0; i < 5; i++){
	  printf("data: %02X\n", receive[i]);
	}
	
}

int getImageLength(void){
	int i;
	uint8_t firstData, data;
	printf("Get length: \n");
	//flushOut();
	uint8_t send[5] = {
	      0x56,
	      0x00,
	      0x34,
	      0x01,
	      0x00
	};

	uint8_t receive[9];

	writeTx(0x56);
	writeTx(0x00);
	writeTx(0x34);
	writeTx(0x01);
	writeTx(0x00);
	/*
	for (i = 0; i < 5; i++) {
	      writeTx(send[i]);
	}
	*/
	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 9; i++){
				data = readRx();
				usleep(1);
				receive[i] = data;
			}
			break;
		}
	}
	/*
	for (i = 0; i < 9; i++){
	  printf("data: %02X\n", receive[i]);
	}
	*/
	imageLength[0] = receive[7];
	imageLength[1] = receive[8];
	//printf("Image length: %02X %02X\n", imageLength[0], imageLength[1]);
	
	return 1;
}

int readImage(void){
	int i;
	uint8_t firstData, data;

	uint8_t send[16] = {
	      0x56,
	      0x00,
	      0x32,
	      0x0C,
	      0x00,
	      0x0A,
	      0x00,
	      0x00,
	      0x00, // MH
	      0x00, // ML
	      0x00,
	      0x00,
	      imageLength[0], // MSB length
	      imageLength[1], // LSB length
	      0x00, // Spacing 0x0b
	      0x0A  // Spacing 0xba
	};
	
	uint8_t receive[20000];
	writeTx(0x56);
	writeTx(0x00);
	writeTx(0x32);
	writeTx(0x0C);
	writeTx(0x00);
	writeTx(0x0A);
	writeTx(0x00);
	writeTx(0x00);
	writeTx(0x00);
	writeTx(0x00);
	writeTx(0x00);
	writeTx(0x00);
	writeTx(imageLength[0]);
	writeTx(imageLength[1]);
	writeTx(0x00);
	writeTx(0x0A);
	/*
	for (i = 0; i < 16; i++) {
	      writeTx(send[i]);
	}
	*/
	int flag = 0;
	int count;
	i = 0;
	imageLengthInt = 0;
	while(1){
		data = readRx();
		usleep(1);
		receive[0] = data;
		i = 1;
		while(1){
			data = readRx();
			usleep(1);
			receive[i] = data;
			if(receive[i-1] == 0xFF && receive[i] == 0xD9){
				/*
				if(flag == 1){
					count = i;
					break;
				}
				else{
					flag = 1;
				}
				*/
				count = i;
				break;
			}
			i++;
		}
		break;
	}
	/*
	for(i = 0; i <= imageLengthInt; i++){
		cameraImage[i] = receive[i];
	}*/
	for (i = 0; i <= count; i++){
		/*
		if (receive[i] < 0x10){
			printf("0");
		}*/
    	printf("%02X", (uint8_t) receive[i]);
	}
	

	printf("\nDONE at %d\n", count);
	return 1;
}

int stopTakingImage(void){
	int i;
	uint8_t firstData;
	printf("\nStop taking image: \n");
	uint8_t send[5] = {
	      0x56,
	      0x00,
	      0x36,
	      0x01,
	      0x02
	};

	uint8_t receive[5];

	for (i = 0; i < 5; i++) {
	      writeTx(send[i]);
	}

	while(1){
		firstData = readRx();
		if(firstData == 0x76){
			receive[0] = firstData;
			for(i = 1; i < 5; i++){
				receive[i] = readRx();
			}
			break;
		}
	}
	/*
	for (i = 0; i < 5; i++){
	  	printf("data: %x\n", receive[i]);
	}
	*/
	return 1;
}

uint8_t readRx(void) {
	// poll Rx bit in 6850 status register. Wait for it to become '1' or we've timed out
	while ((Camera_Status & 0x01) != 0x01) {}
	uint8_t val = Camera_RxData;
	return val;
}

void writeTx(uint8_t val) {
	// poll tx bit in 6850 status register. Wait for it to become '2'
	while ((Camera_Status & 0x02) != 0x02) {}
	Camera_TxData = val;
}
