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

int main() {
	printf("Starting Program\n");

	// Initialize Hardware
//	Wifi_Init();
	TS_Init();
	Graphics_InitializeNumberPad();

	//int Security_State = STATE_DRAW_WELCOME_SCREEN;
	int Security_State = STATE_DRAW_INIT_SCREEN;
	int isInitialized = 0;
	int button;

//	Wifi_SendCommand("dofile(\"system.lua\")\r\n");
//	Wifi_WaitReady();
	printf("Lua file loaded\n");

	Point p;

	while (1) {
		switch (Security_State) {
//		case STATE_DRAW_WELCOME_SCREEN:
//			Graphics_DrawWelcomeScreen();
//			Security_State = STATE_WELCOME_SCREEN;
//			break;
//		case STATE_WELCOME_SCREEN:
//			TS_WaitForRelease();
//			p = TS_GetRelease();
//			if (Graphics_RectangleTouched(p.x, p.y, request_code_rect)) {
//				printf("Generating new security code\n");
//				// Generate a new code
//				Security_GenerateCode();
//				printf("Security Code: %d,%d,%d,%d\n", Security_Code[0],
//						Security_Code[1], Security_Code[2], Security_Code[3]);
//				// Send the code to our phone
//				char buf[256];
//				snprintf(buf, sizeof buf,
//						"send_sms(\"+14387000752\",\"+17789524378\",\"Your New Code is %d,%d,%d,%d\")\r\n",
//						Security_Code[0], Security_Code[1], Security_Code[2],
//						Security_Code[3]);
//
//				Wifi_SendCommand(buf);
//				Wifi_WaitReady();
//				usleep(1000000);
//				printf("User Code: %d,%d,%d,%d\n", User_Input[0], User_Input[1],
//						User_Input[2], User_Input[3]);
//				Security_State = STATE_DRAW_NUMPAD;
//			}
//			break;
//		case STATE_DRAW_NUMPAD:
//			Graphics_DrawMenu();
//			Security_State = STATE_NUMPAD;
//			break;
//		case STATE_NUMPAD:
//			TS_WaitForRelease();
//			p = TS_GetRelease();
//			int button = Graphics_GetNumberPressed(p.x, p.y);
//			printf("Coords: %d, %d\n", p.x, p.y);
//			printf("Button index: %d\n", button);
//
//			if (button == NUMPAD_ENTER && graphics_field_cursor == CODE_LENGTH) {
//				if (Security_CheckCode()) {
//					Security_State = STATE_DRAW_UNLOCK_SCREEN;
//				} else {
//					Security_State = STATE_DRAW_LOCK_SCREEN;
//				}
//			} else if (button == NUMPAD_DELETE && graphics_field_cursor >= 0) {
//				int j;
//				// Todo handle in security
//				for (j = 0; j < CODE_LENGTH; j++) {
//					User_Input[j] = -1;
//				}
//				graphics_field_cursor = 0;
//				printf("User Code Delete: %d,%d,%d,%d\n", User_Input[0],
//						User_Input[1], User_Input[2], User_Input[3]);
//			} else if (graphics_field_cursor <= CODE_LENGTH) {
//				// Todo handle in security
//				User_Input[graphics_field_cursor] = Graphics_ButtonNumToNum(
//						button);
//				printf("User Code: %d,%d,%d,%d\n", User_Input[0], User_Input[1],
//						User_Input[2], User_Input[3]);
//			}
//			printf("Cursor length: %d\n", graphics_field_cursor);
//			break;
		case STATE_DRAW_INIT_SCREEN:
			if (isInitialized) {
				Graphics_DrawWelcomeScreen();
				Security_State = STATE_SIGN_IN;
			} else {
				Graphics_DrawInitializationScreen();
				Security_State = STATE_SIGN_UP;
			}
			break;
		case STATE_SIGN_UP:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_RectangleTouched(p.x, p.y, sign_up_rect)) {
				Graphics_DrawPhoneNumberMenu();
				graphics_field_cursor = 0;
				Security_State = STATE_OWNER_PHONENUM;
			}
			break;
		case STATE_OWNER_PHONENUM:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, PHONENUMLENGTH,
					Phone_Number_Input, PHONEINDEXLEN);
			if (button == NUMPAD_ENTER
					&& graphics_field_cursor == PHONENUMLENGTH) {
				Graphics_DrawMasterCodeMenu();
				graphics_field_cursor = 0;
				Security_State = STATE_OWNER_MASTERCODE;
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				Security_State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Phone Number: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					Phone_Number_Input[0], Phone_Number_Input[1],
					Phone_Number_Input[2], Phone_Number_Input[3],
					Phone_Number_Input[4], Phone_Number_Input[5],
					Phone_Number_Input[6], Phone_Number_Input[7],
					Phone_Number_Input[8], Phone_Number_Input[9]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_OWNER_MASTERCODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, MASTERCODELENGTH,
					Master_Code, MASTERINDEXLEN);
			if (button == NUMPAD_ENTER
					&& graphics_field_cursor == MASTERCODELENGTH) {
				Security_State = STATE_SEND_MASTERCODE;
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				Security_State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Master Code: %d,%d,%d,%d,%d,%d\n", Master_Code[0],
					Master_Code[1], Master_Code[2], Master_Code[3],
					Master_Code[4], Master_Code[5]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_SEND_MASTERCODE:
			printf("Master code sent");
			isInitialized = 1;
			Security_State = STATE_DRAW_INIT_SCREEN;
			break;
		case STATE_SIGN_IN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_RectangleTouched(p.x, p.y, log_in_rect)) {
				//Log In Button was pressed
				Graphics_DrawMasterCodeMenu();
				graphics_field_cursor = 0;
				Security_State = STATE_ENTER_MASTER_CODE;
			}
			if (Graphics_RectangleTouched(p.x, p.y, request_code_rect)) {
				//Request Code Button was pressed

				Security_State = STATE_REQUESTED_CODE;
			}
			break;
		case STATE_REQUESTED_CODE:
			Graphics_DrawMenu();
			Security_State = STATE_ENTER_CODE;
			break;
		case STATE_ENTER_MASTER_CODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			button = Graphics_GetNumberPressed(p.x, p.y, MASTERCODELENGTH,
					Master_Code, MASTERINDEXLEN);
			if (button == NUMPAD_ENTER
					&& graphics_field_cursor == MASTERCODELENGTH) {
				//Check Against Master Code here.
			}
			if (Graphics_RectangleTouched(p.x, p.y, home_button_rect)) {
				Security_State = STATE_DRAW_INIT_SCREEN;
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("Master Code: %d,%d,%d,%d,%d,%d\n", Master_Code[0],
					Master_Code[1], Master_Code[2], Master_Code[3],
					Master_Code[4], Master_Code[5]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_ENTER_CODE:
			TS_WaitForRelease();
			p = TS_GetRelease();
			int button = Graphics_GetNumberPressed(p.x, p.y, CODELENGTH,
					User_Input, CODEINDEXLEN);
			if (button == NUMPAD_ENTER) {
				if (Security_CheckCode()) {
					Security_State = STATE_DRAW_UNLOCK_SCREEN;
				} else {
					Security_State = STATE_DRAW_LOCK_SCREEN;
				}
			}
			printf("Coords: %d, %d\n", p.x, p.y);
			printf("Button num: %d\n", Graphics_ButtonNumToNum(button));
			printf("User Code: %d,%d,%d,%d\n", User_Input[0], User_Input[1],
					User_Input[2], User_Input[3]);
			printf("Cursor length: %d\n", graphics_field_cursor);
			break;
		case STATE_DRAW_LOCK_SCREEN:
			Graphics_DrawLockScreen();
			Security_State = STATE_LOCK_SCREEN;
			break;
		case STATE_LOCK_SCREEN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_InRectangle(p.x, p.y, home_button_rect)) {
				Security_State = STATE_DRAW_INIT_SCREEN;
			}
			break;
		case STATE_DRAW_UNLOCK_SCREEN:
			Graphics_DrawUnlockScreen();
			Security_State = STATE_UNLOCK_SCREEN;
			break;
		case STATE_UNLOCK_SCREEN:
			TS_WaitForRelease();
			p = TS_GetRelease();
			if (Graphics_InRectangle(p.x, p.y, home_button_rect)) {
				Security_State = STATE_DRAW_INIT_SCREEN;
			}
			break;
		case STATE_IDLE:
			break;
		}
	}

	Leds_Celebrate();

	return 0;
}
