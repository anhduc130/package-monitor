/*
 * main.c
 *
 *  Created on: Jan 21, 2017
 *      Author: Andrew Bui
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "camera.h"

int main() {
	printf("Starting Camera Testing\n");
	uint8_t byte;

	// Initialize the camera
	Camera_Init();

	while((Camera_Status & 0x01) == 0x01){
		byte = Camera_RxData;
	}

	//usleep(1000000);
	reset();

	/*
	usleep(10000);
	if(!changeImageSize()){
		printf("Error: Can't change the image size");
	}


	usleep(10000);
	if(!changeBaudRate()){
		printf("Error: Can't change the baud rate");
	}
	
	while((Camera_Status & 0x01) == 0x01){
		byte = Camera_RxData;
	}
	*/
	usleep(5000000);
	takePicture();

	while((Camera_Status & 0x01) == 0x01){
		byte = Camera_RxData;
	}

	usleep(1000000);
	getImageLength();

	while((Camera_Status & 0x01) == 0x01){
		byte = Camera_RxData;
	}
	
	usleep(100000);
	readImage();

	while((Camera_Status & 0x01) == 0x01){
		byte = Camera_RxData;
	}

	usleep(1000000);
	stopTakingImage();

	// Send the data to the server now
	/*
	int i;
	for (i = 0; i <= imageLengthInt; i++){
		if(cameraImage[i] < 0x10){
			printf("0");
		}
    	printf("%X ", (uint8_t) cameraImage[i]);
	}
	*/
	return 0;
}
