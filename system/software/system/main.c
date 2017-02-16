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

#define STATE_DRAW_INIT_SCREEN		1
#define STATE_SIGN_UP				2
#define STATE_OWNER_PHONENUM		3
#define STATE_OWNER_MASTERCODE		4
#define STATE_SEND_MASTERCODE		5
#define STATE_SIGN_IN				6
#define STATE_ENTER_MASTER_CODE		7
#define STATE_REQUESTED_CODE		8
#define STATE_ENTER_CODE			9
#define STATE_DRAW_LOCK_SCREEN		10
#define STATE_LOCK_SCREEN			11
#define STATE_DRAW_UNLOCK_SCREEN	12
#define STATE_UNLOCK_SCREEN			13
#define STATE_IDLE					14

int main() {
	printf("Starting Program\n");

	// Initialize Hardware
	Wifi_Init();
	TS_Init();
	Graphics_Initialize();

	int isInitialized = 0;
	volatile int button;

	/**
	 * Send the wifi command
	 */
	Wifi_SendCommand("dofile(\"system.lua\")\r\n");
	Wifi_ReadResponse();

	printf("File Loaded\n");
	usleep(500000);

	// Ensure we are connected to wifi
	Wifi_EnsureConnection();

	printf("Wifi Connected!\n");
	usleep(500000);

	// Check if the box has been initialized already
	do {
		isInitialized = Security_CheckIsRegistered();
		usleep(20000);
	} while (isInitialized == EPWMAS);

	isInitialized = 0;

	if(isInitialized)
		printf("Box Already Initialized!\n");
	else
		printf("Box Not Initialized!\n");

	Point p;
	while (1) {
		switch (State) {
		case STATE_DRAW_INIT_SCREEN:
			if (isInitialized) {
				Graphics_DrawWelcomeScreen();
				State = STATE_SIGN_IN;
			} else {
				Graphics_DrawInitializationScreen();
				State = STATE_SIGN_UP;
			}
			break;
		/**
		 * This state draws the sign up screen
		 */
		case STATE_SIGN_UP:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_RectangleTouched(p.x, p.y, sign_up_rect)) {
				Graphics_DrawPhoneNumberMenu();
				graphics_field_cursor = 0;
				State = STATE_OWNER_PHONENUM;
			}
			break;
		/**
		 * This state handles the owner registering their phone number
		 */
		case STATE_OWNER_PHONENUM:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, PHONENUMLENGTH, Master_Phone_Number, PHONEINDEXLEN);
			if (button == NUMPAD_ENTER
					&& graphics_field_cursor == PHONENUMLENGTH) {
				Graphics_DrawMasterCodeMenu();
				graphics_field_cursor = 0;
				State = STATE_OWNER_MASTERCODE;
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Phone Number: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					Master_Phone_Number[0], Master_Phone_Number[1],
					Master_Phone_Number[2], Master_Phone_Number[3],
					Master_Phone_Number[4], Master_Phone_Number[5],
					Master_Phone_Number[6], Master_Phone_Number[7],
					Master_Phone_Number[8], Master_Phone_Number[9]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		/**
		 * This state handles the owner typing in their master code
		 */
		case STATE_OWNER_MASTERCODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, MASTERCODELENGTH,Master_Code_Input, MASTERINDEXLEN);
			if (button == NUMPAD_ENTER && graphics_field_cursor == MASTERCODELENGTH) {
				State = STATE_SEND_MASTERCODE;
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Master Code: %d,%d,%d,%d,%d,%d\n", Master_Code_Input[0],
					Master_Code_Input[1], Master_Code_Input[2], Master_Code_Input[3],
					Master_Code_Input[4], Master_Code_Input[5]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		/**
		 * This state sends the put request for the new user
		 */
		case STATE_SEND_MASTERCODE:
			// Send put request to register the owner
			Security_RegisterOwner();
			isInitialized = 1;
			State = STATE_DRAW_INIT_SCREEN;
			break;
		case STATE_SIGN_IN: {
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_RectangleTouched(p.x, p.y, log_in_rect)) {
				//Log In Button was pressed
				Graphics_DrawMasterCodeMenu();
				graphics_field_cursor = 0;
				// Get the values for this device
				while(Security_ObtainValues() != 0);
				State = STATE_ENTER_MASTER_CODE;
			}

			if (Graphics_RectangleTouched(p.x, p.y, request_code_rect)) {
				// Get the values for this device
				while(Security_ObtainValues() != 0);
				//Request Code Button was pressed
				State = STATE_REQUESTED_CODE;
			}
			break;
		}
		/**
		 * User has requested to recieve the code
		 */
		case STATE_REQUESTED_CODE:
			Graphics_DrawMenu();
			printf("Drawing the menu\n");
			graphics_field_cursor = 0;
			State = STATE_ENTER_CODE;
			break;
		/**
		 * For the owner as they enter the master code to open the box directly
		 */
		case STATE_ENTER_MASTER_CODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, MASTERCODELENGTH,Master_Code_Input, MASTERINDEXLEN);
			if (button == NUMPAD_ENTER && graphics_field_cursor == MASTERCODELENGTH) {
				//Check Against Master Code here.
				if(Security_CheckMasterCode()) {
					State = STATE_DRAW_UNLOCK_SCREEN;
				}
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Master Code: %d,%d,%d,%d,%d,%d\n", Master_Code_Input[0],
					Master_Code_Input[1], Master_Code_Input[2], Master_Code_Input[3],
					Master_Code_Input[4], Master_Code_Input[5]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		/**
		 * State to enter the code
		 */
		case STATE_ENTER_CODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			int button = Graphics_GetNumberPressed(p.x, p.y, CODELENGTH, Security_Code_Input, CODEINDEXLEN);
			if (button == NUMPAD_ENTER) {
				if (Security_CheckCode()) {
					State = STATE_DRAW_UNLOCK_SCREEN;
				} else {
					State = STATE_DRAW_LOCK_SCREEN;
				}
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("User Code: %d,%d,%d,%d\n", Security_Code_Input[0], Security_Code_Input[1],
					Security_Code_Input[2], Security_Code_Input[3]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_DRAW_LOCK_SCREEN:
			Graphics_DrawLockScreen();
			State = STATE_LOCK_SCREEN;
			break;
		case STATE_LOCK_SCREEN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_InRectangle(p.x, p.y, home_button_rect)) {
				State = STATE_DRAW_INIT_SCREEN;
			}
			break;
		case STATE_DRAW_UNLOCK_SCREEN:
			Graphics_DrawUnlockScreen();
			State = STATE_UNLOCK_SCREEN;
			break;
		case STATE_UNLOCK_SCREEN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_InRectangle(p.x, p.y, home_button_rect)) {
				State = STATE_DRAW_INIT_SCREEN;
			}
			break;
		case STATE_IDLE:
			printf("I'm in state idle...\n");
			break;
		default:
			printf("%d",State);
			printf("SHOUDLN'T BE HERE");
			break;
		}
	}
	Leds_Celebrate();

	return 0;
}
