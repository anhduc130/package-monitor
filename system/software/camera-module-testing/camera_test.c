/*
 * main.c
 *
 *  Created on: Jan 21, 2017
 *      Author: David Jung
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "camera.h"

int main() {
	// Draw the number pad

	printf("Starting Camera Testing\n");

	// Initialize Hardware
	Camera_Init();

	usleep(100000);

	// Try to locate the camera
	if (reset()) {
		printf("Camera Found\n");
	} else {
		printf("No camera found\n");
		return 1;
	}

	setImageSize(VC0706_320x240);        // medium

	printf("Taking picture in 1 second...\n");
	usleep(100000);

	if (takePicture()) {
		printf("Picture taken!\n");
	} else {
		printf("Failed to capture\n");
		return 2;
	}

	uint32_t jpglen = frameLength();
	printf("Jpeg Length %u\n", jpglen);

	int numcols = 0;
	int counter = 0;
	while (jpglen > 0) {
		// read 32 bytes at a time;
		uint8_t *buffer;
		uint8_t bytesToRead = 32 < jpglen ? 32 : jpglen; // change 32 to 64 for a speedup but may not work with all setups!
		buffer = readPicture(bytesToRead);

		int i;
		for (i = 0; i < jpglen; i++) {
			printf("%x,", buffer[i]);
			counter++;
			if (counter % 320 == 0) {
				printf("\n");
			}

		}
		jpglen -= bytesToRead;
	}

	return 0;
}
