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

#include "colours.h"
#include "serial.h"
#include "switches.h"
#include "leds.h"
#include "fonts.h"
#include "graphics.h"
#include "security.h"

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


int test() {
	// Draw the number pad

	printf("Starting Program\n");

	char print_buf[512];

	// Initialize Hardware
	Wifi_Init();
	TS_Init();

	int Security_State = STATE_DRAW_WELCOME_SCREEN;

	Wifi_SendCommand("dofile(\"system.lua\")\r\n");
	int num_reads = Wifi_WaitReady(print_buf);
	int j = 0;
	for (; j < num_reads; j++) {
		printf(print_buf[j]);
	}
	printf("Lua file loaded\n");

	Point p;

	while (1) {
		switch (Security_State) {
		case STATE_DRAW_WELCOME_SCREEN:
			Graphics_DrawWelcomeScreen();
			Security_State = STATE_WELCOME_SCREEN;
			break;
		case STATE_WELCOME_SCREEN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_RequestCodeTouched(p.x, p.y)) {
				printf("Generating new security code\n");
				// Generate a new code
				Security_GenerateCode();
				printf("Security Code: %d,%d,%d,%d\n", Security_Code[0],
						Security_Code[1], Security_Code[2], Security_Code[3]);
				// Send the code to our phone
				char buf[256];
				snprintf(buf, sizeof buf,
						"send_sms(\"+14387000752\",\"+17789524378\",\"Your New Code is %d,%d,%d,%d\")\r\n",
						Security_Code[0], Security_Code[1], Security_Code[2],
						Security_Code[3]);

				Wifi_SendCommand(buf);
				num_reads = Wifi_WaitReady(print_buf);
				int j = 0;
				for (; j < num_reads; j++) {
					printf(print_buf[j]);
				}
				usleep(1000000);
				printf("User Code: %d,%d,%d,%d\n", User_Input[0], User_Input[1],
						User_Input[2], User_Input[3]);
				Security_State = STATE_DRAW_NUMPAD;
			}
			break;
		case STATE_DRAW_NUMPAD:
			Graphics_DrawMenu();
			Security_State = STATE_NUMPAD;
			break;
		case STATE_NUMPAD:
			TS_WaitForRelease();
			p = TS_GetRelease();
			int button = Graphics_GetNumPad(p.x, p.y);
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button index: %d\n", button);

			if (button == NUMPAD_ENTER && graphics_field_cursor == CODE_LENGTH) {
				if (Security_CheckCode()) {
					Security_State = STATE_DRAW_UNLOCK_SCREEN;
				} else {
					Security_State = STATE_DRAW_LOCK_SCREEN;
				}
			} else if (button == NUMPAD_DELETE && graphics_field_cursor >= 0) {
				int j;
				// Todo handle in security
				for (j = 0; j < CODE_LENGTH; j++) {
					User_Input[j] = -1;
				}
				printf("User Code Delete: %d,%d,%d,%d\n", User_Input[0],
						User_Input[1], User_Input[2], User_Input[3]);
			} else if (graphics_field_cursor <= CODE_LENGTH) {
				// Todo handle in security
				User_Input[graphics_field_cursor] = Graphics_ButtonNumToNum(
						button);
				printf("User Code: %d,%d,%d,%d\n", User_Input[0], User_Input[1],
						User_Input[2], User_Input[3]);
			}
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_DRAW_LOCK_SCREEN:
			Graphics_DrawLockScreen();
			Security_State = STATE_LOCK_SCREEN;
			break;
		case STATE_LOCK_SCREEN:
			break;
		case STATE_DRAW_UNLOCK_SCREEN:
			Graphics_DrawUnlockScreen();
			Security_State = STATE_UNLOCK_SCREEN;
			break;
		case STATE_UNLOCK_SCREEN:
			break;
		}
	}

	Leds_Celebrate();

	return 0;
}
