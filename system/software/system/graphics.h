/*
 * Graphics.h
 *
 *  Created on: Jan 31, 2017
 *      Author: Andy Lin
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#define TRUE 	1
#define FALSE 	0
#define XRES 	800
#define YRES 	480

#define NUMROWS				4
#define NUMCOLUMNS			3

#define FIELDSTARTX			(800 - ((SQUAREWIDTH * (NUMROWS - 1)) + (SPACESIZE * (NUMROWS - 2))))/ 2
#define FIELDSTARTY 		35
#define FIELDENDX			FIELDSTARTX + (SQUAREWIDTH * (NUMROWS - 1)) + (SPACESIZE * (NUMROWS - 2))
#define FIELDENDY			100
#define FIELDSPACE			8

#define CODELENGTH			4
#define MASTERCODELENGTH 	6
#define PHONENUMLENGTH 		10

#define CODEINDEXLEN		((FIELDENDX - FIELDSTARTX) - (FIELDSPACE * (CODELENGTH + 1))) / CODELENGTH
#define PHONEINDEXLEN		((FIELDENDX - FIELDSTARTX) - (FIELDSPACE * (PHONENUMLENGTH + 1))) / PHONENUMLENGTH
#define MASTERINDEXLEN		((FIELDENDX - FIELDSTARTX) - (FIELDSPACE * (MASTERCODELENGTH + 1))) / MASTERCODELENGTH

#define SPACESIZE			25
#define SQUAREWIDTH			150
#define SQUAREHEIGHT		60

#define REGULAR_BUTTON		0
#define HOME_BUTTON			1

/*******************************************************************************************
 ** Line Draw Commands for the Command Register
 *******************************************************************************************/
#define DrawHLine				1
#define DrawVLine				2
#define DrawLine				3
#define PutAPixel				0xA
#define GetAPixel				0xB
#define ProgramPaletteColour   	0x10

/*******************************************************************************************
 ** This macro pauses until the graphics chip status register indicates that it is idle
 *******************************************************************************************/
#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

/*******************************************************************************************
 ** Command Register Addresses
 *******************************************************************************************/
#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsX1Reg   			(*(volatile unsigned short int *)(0x84000002))
#define GraphicsY1Reg   			(*(volatile unsigned short int *)(0x84000004))
#define GraphicsX2Reg   			(*(volatile unsigned short int *)(0x84000006))
#define GraphicsY2Reg				(*(volatile unsigned short int *)(0x84000008))
#define GraphicsColourReg			(*(volatile unsigned short int *)(0x8400000E))
#define GraphicsBackGroundColourReg (*(volatile unsigned short int *)(0x84000010))

#define NUMPAD_SIZE 12
#define NUMPAD_DELETE 11
#define NUMPAD_ENTER 9

struct Rectangle {
	int x1;
	int x2;

	int y1;
	int y2;

	int x_length;
	int y_length;

	int colour;
	char *text;
	int type;
} typedef Rectangle_t;

Rectangle_t* numpad[12];
Rectangle_t* request_code_rect;
Rectangle_t* sign_up_rect;
Rectangle_t* log_in_rect;
Rectangle_t* home_button_rect;
int graphics_field_cursor;


/*******************************************************************************************
 ** Graphics Functions
 *******************************************************************************************/
void Graphics_WriteAPixel(int x, int y, int Colour);
int Graphics_ReadAPixel(int x, int y);
void Graphics_ProgramPalette(int PaletteNumber, int RGB);
void Graphics_DrawHorizontalLine(int x1, int x2, int y, int Colour);
void Graphics_DrawVerticalLine(int y1, int y2, int x, int Colour);
void Graphics_DrawStraightLine(int x1, int y1, int x2, int y2, int Colour);
void Graphics_DrawCircle(int x0, int y0, int radius, int colour);
void Graphics_DrawRectangle(Rectangle_t*);
void Graphics_PointsDrawRectangle(int x1, int y1, int x2, int y2, int Colour);
void Graphics_DrawText(int x, int y, int colour, int backgroundColour, char *text, int n, int erase);
Rectangle_t* Graphics_Init_Rectangle(int x1, int y1, int x2, int y2, int colour, int type, const char *text);
void Graphics_Destroy_Rectangle(Rectangle_t* rect);
void Graphics_ClearScreen(int Colour);
void Graphics_DrawLineDemo();
void Graphics_DrawWelcomeScreen();
void Graphics_DrawInitializationScreen();
void Graphics_DrawUnlockScreen();
void Graphics_DrawLockScreen();
void Graphics_DrawMenu();
void Graphics_DrawField();
void Graphics_DrawSquare(int index, int colour, int codeIndexLength);
int Graphics_GetNumberPressed(int point_x, int point_y, int codeLength, int array[], int codeIndexLength);
int Graphics_ButtonNumToNum(int button);
int Graphics_InRectangle(int point_x, int point_y, Rectangle_t* rect);
int Graphics_RequestCodeTouched(int point_x, int point_y, Rectangle_t *rect);
float Graphics_AreaRect(Rectangle_t* rect);
float Graphics_AreaTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
Rectangle_t* Graphics_InitializeRectangleCentered(int xLength, int yPos, int yLength, int xBoundLeft, int xBoundRight, int colour, char *text);
void Graphics_InitializeHomeButton();
void Graphics_DrawHomeButton();
void Graphics_DrawHomeIcon(int xStart, int yStart);
void Graphics_DrawMasterUnlockMenu();
void Graphics_InitializeNumberPad();
void Graphics_DrawTextCentered(int y, int xBoundLeft, int xBoundRight, int colour, int backgroundColour, char *text, int n, int erase);
void Graphics_DrawNumPad();
void Graphics_DrawPhoneNumberMenu();
void Graphics_DrawMasterCodeMenu();
void Graphics_DrawUserEnteredDigit(int index, int digitVal, int colour, int codeIndexLength);

void Graphics_Font5x7(int x, int y, int fontcolour, int backgroundcolour,int c, int Erase);
void Graphics_Font10x14(int x, int y, int colour, int backgroundcolour, int c, int Erase);
#endif /* GRAPHICS_H_ */
