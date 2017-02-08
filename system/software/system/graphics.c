#include "graphics.h"
#include "colours.h"
#include "fonts.h"
#include "security.h"
#include <stdlib.h>

/**********************************************************************
 * This function writes a single pixel to the x,y coords specified in the specified colour
 * Note colour is a palette number (0-255) not a 24 bit RGB value
 **********************************************************************/
void Graphics_WriteAPixel(int x, int y, int Colour) {
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x;
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = PutAPixel;
}

/*****************************************************************************************
 * This function read a single pixel from x,y coords specified and returns its colour
 * Note returned colour is a palette number (0-255) not a 24 bit RGB value
 ******************************************************************************************/
int Graphics_ReadAPixel(int x, int y) {
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x;
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;

	WAIT_FOR_GRAPHICS;
	return (int) (GraphicsColourReg);
}

/****************************************************************************************************
 ** Subroutine to program a hardware (graphics chip) palette number with an RGB value
 ** e.g. ProgramPalette(RED, 0x00FF0000) ;
 ****************************************************************************************************/
void Graphics_ProgramPalette(int PaletteNumber, int RGB) {
	WAIT_FOR_GRAPHICS;
	GraphicsColourReg = PaletteNumber;
	GraphicsX1Reg = RGB >> 16;
	GraphicsY1Reg = RGB;
	GraphicsCommandReg = ProgramPaletteColour;
}

/*****************************************************************************************
 *	Function to draw a horizontal line from x1 to x2 with the given colour.
 *
 ******************************************************************************************/
void Graphics_DrawHorizontalLine(int x1, int x2, int y, int Colour) {
	if (x2 < x1) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y;
	GraphicsY2Reg = y;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawHLine;
}

/*****************************************************************************************
 *	Function to draw a vertical line from y1 to y2 with the given colour.
 *
 ******************************************************************************************/
void Graphics_DrawVerticalLine(int y1, int y2, int x, int Colour) {
	if (y2 < y1) {
		int temp = y1;
		y1 = y2;
		y2 = temp;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x;
	GraphicsX2Reg = x;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawVLine;
}

/*****************************************************************************************
 *	Function to draw a line from x1, y1 to x2, y2 with the given colour.
 *
 ******************************************************************************************/
void Graphics_DrawStraightLine(int x1, int y1, int x2, int y2, int Colour) {
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawLine;
}

/*****************************************************************************************
 *	Function to draw a circle given an x coordinate, y coordinate and the radius.
 *
 ******************************************************************************************/
void Graphics_DrawCircle(int x0, int y0, int radius) {
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y) {
		Graphics_WriteAPixel(x0 + x, y0 + y, BLACK);
		Graphics_WriteAPixel(x0 + y, y0 + x, BLACK);
		Graphics_WriteAPixel(x0 - y, y0 + x, BLACK);
		Graphics_WriteAPixel(x0 - x, y0 + y, BLACK);
		Graphics_WriteAPixel(x0 - x, y0 - y, BLACK);
		Graphics_WriteAPixel(x0 - y, y0 - x, BLACK);
		Graphics_WriteAPixel(x0 + y, y0 - x, BLACK);
		Graphics_WriteAPixel(x0 + x, y0 - y, BLACK);

		if (err <= 0) {
			y += 1;
			err += 2 * y + 1;
		}
		if (err > 0) {
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

/*****************************************************************************************
 *	Function to draw a rectangle given a Rectangle_t object.
 *
 ******************************************************************************************/
void Graphics_DrawRectangle(Rectangle_t *rect) {

	int x1 = rect->x1;
	int y1 = rect->y1;
	int x2 = rect->x2;
	int y2 = rect->y2;
	int colour = rect->colour;

	while (y1 <= y2) {
		Graphics_DrawHorizontalLine(x1, x2, y1, colour);
		y1++;
	}

	if (rect->text != NULL){
		printf(rect->text);
		Graphics_DrawText(x1 + 20, y1 - 35, WHITE, GRAY, rect->text, strlen(rect->text),0);
	}
}

/*****************************************************************************************
 *	Function to draw a rectangle given the x and y coordinates and a colour.
 *
 ******************************************************************************************/
void Graphics_PointsDrawRectangle(int x1, int y1, int x2, int y2, int Colour) {
	while (y1 <= y2) {
		Graphics_DrawHorizontalLine(x1, x2, y1, Colour);
		y1++;
	}
}

/*****************************************************************************************
 *	Function to draw text at the given coordinate in the given colour.
 *
 ******************************************************************************************/
void Graphics_DrawText(int x, int y, int colour, int backgroundColour, char *text, int n, int erase){
	int i;
	int xPos = x;
	for (i = 0; i < n; i++){
		Graphics_Font10x14(xPos, y, colour, backgroundColour, text[i], erase);
		xPos = xPos + 12;
	}
}

/*****************************************************************************************
 *	Initializes and returns a rectangle given the four points, a colour and text for the rectangle
 *
 ******************************************************************************************/
Rectangle_t* Graphics_Init_Rectangle(int x1, int y1, int x2, int y2, int colour, const char *text) {
	Rectangle_t* rect = malloc(sizeof(Rectangle_t));

	rect->x1 = x1;
	rect->x2 = x2;
	rect->y1 = y1;
	rect->y2 = y2;

	rect->colour = colour;
	rect->x_length = x2 - x1;
	rect->y_length = y2 - y1;
	rect->text = strdup(text);

	return rect;
}

/*****************************************************************************************
 *	Destroys the given rectangle
 *
 ******************************************************************************************/
void Graphics_Destroy_Rectangle(Rectangle_t* rect) {
	free(rect);
}

/*****************************************************************************************
 *	Clears the screen with the given colour
 *
 ******************************************************************************************/
void Graphics_ClearScreen(int Colour) {
	int i;
	for (i = 0; i < 480; i++) {
		Graphics_DrawHorizontalLine(0, 800, i, Colour);
	}
}

/*****************************************************************************************
 *	Function to display the line demo
 *
 ******************************************************************************************/
void Graphics_DrawLineDemo() {
	Graphics_DrawHorizontalLine(0, 800, 240, MAGENTA);
	Graphics_DrawVerticalLine(0, 480, 400, MAGENTA);
	Graphics_DrawStraightLine(0, 0, 800, 480, MAGENTA);
	Graphics_DrawStraightLine(0, 480, 800, 0, MAGENTA);
}

/*****************************************************************************************
 *	Function to draw the welcome screen GUI.
 *
 ******************************************************************************************/
void Graphics_DrawWelcomeScreen(){
	char requestLabel[] = "Welcome, please request a code:";
	Graphics_ClearScreen(BLACK);
	Graphics_DrawText(200, 100, WHITE, BLACK, requestLabel, strlen(requestLabel), 0);
	request_code_rect = Graphics_Init_Rectangle(300, 150, 300 + SQUAREWIDTH, 150 + SQUAREHEIGHT, GREEN, "Click");
	Graphics_DrawRectangle(request_code_rect);
}

/*****************************************************************************************
 *	Function to draw the unlock screen GUI.
 *
 ******************************************************************************************/
void Graphics_DrawUnlockScreen(){
	Graphics_ClearScreen(GREEN);
	char successLabel[] = "Successfully Unlocked!";
	Graphics_DrawText(300, 200, WHITE, BLACK, successLabel, strlen(successLabel), 0);

}

/*****************************************************************************************
 *	Function to draw the lock screen GUI.
 *
 ******************************************************************************************/
void Graphics_DrawLockScreen(){
	Graphics_ClearScreen(RED);
	char failureLabel[] = "Incorrect Pin";
	Graphics_DrawText(300, 200, WHITE, BLACK, failureLabel, strlen(failureLabel), 0);
}

/*****************************************************************************************
 *	Function to draw the main menu GUI.
 *
 ******************************************************************************************/
void Graphics_DrawMenu() {
	int i, j, k;
	int xPosStart, xPosEnd = 0;
	int yPosStart = 125;
	int yPosEnd = yPosStart + SQUAREHEIGHT;

	char menuLabel[] = "Please enter the access code:";
	char *text;
	graphics_field_cursor = 0;

	Graphics_ClearScreen(BLACK);

	Graphics_DrawField();
	Graphics_DrawText(FIELDSTARTX + 15, FIELDSTARTY + 5, BLUE, WHITE, menuLabel, strlen(menuLabel),0);

	k = 0;

	for (j = 0; j < 4; j++) {
		xPosStart = (800 - ((SQUAREWIDTH * 3) + (SPACESIZE * 2))) / 2;
		xPosEnd = xPosStart + SQUAREWIDTH;
		for (i = 0; i < 3; i++) {
			int colour = GRAY;
			if(j == 3 && i == 0){
				colour = GREEN;
				text = "Submit";
			}
			else if (j == 3 && i == 2){
				colour = RED;
				text = "Clear";
			}
			else {
				char num[1];
				sprintf(num, "%d", Graphics_ButtonNumToNum(k));
				text = num;
			}

			Rectangle_t* rect = Graphics_Init_Rectangle(xPosStart, yPosStart, xPosEnd, yPosEnd, colour, text);

			numpad[k] = rect;

			Graphics_DrawRectangle(rect);
			xPosStart = xPosEnd + SPACESIZE;
			xPosEnd = xPosStart + SQUAREWIDTH;

			k++;
		}
		yPosStart = yPosEnd + SPACESIZE;
		yPosEnd = yPosStart + SQUAREHEIGHT;
	}
}

/*****************************************************************************************
 *	Function to draw the white field in our number pad GUI.
 *
 ******************************************************************************************/
void Graphics_DrawField() {
	int i;
	int xStart = FIELDSTARTX + FIELDSPACE;

	Graphics_PointsDrawRectangle(FIELDSTARTX, FIELDSTARTY, FIELDENDX, FIELDENDY, WHITE);

	for (i = 0; i < 4; i++) {
		Graphics_DrawHorizontalLine(xStart, xStart + INDEXSIZE, FIELDENDY - 10, BLACK);
		xStart = xStart + INDEXSIZE + FIELDSPACE;
	}
}

/*****************************************************************************************
 *	Function to draw a rectangle in our number pad GUI.
 *
 ******************************************************************************************/
void Graphics_DrawSquare(int index, int colour) {
	int lineStart = FIELDSTARTX + FIELDSPACE;
	while (index > 0){
		lineStart = lineStart + INDEXSIZE + FIELDSPACE;
		index--;
	}

	Graphics_PointsDrawRectangle(lineStart + 45, FIELDSTARTY + 30, lineStart + INDEXSIZE - 45, FIELDSTARTY + 60, colour);
}

int Graphics_GetNumPad(int point_x, int point_y) {
	int i;
	for(i=0;i<NUMPAD_SIZE;i++){
		if(Graphics_InRectangle(point_x,point_y,numpad[i])) {
			// Change the colour temporarily to show press
			numpad[i]->colour = BLACK;
			Graphics_DrawRectangle(numpad[i]);
			usleep(50000);

			if(i == NUMPAD_DELETE) {
				numpad[i]->colour = RED;
				// Decrement since we've deleted a character
				if(graphics_field_cursor > 0) {
					int j;
					for(j=0;j<CODE_LENGTH;j++) {
						graphics_field_cursor = 0;
						Graphics_DrawSquare(j,WHITE);
					}
				}
			} else if(i == NUMPAD_ENTER) {
				numpad[i]->colour = GREEN;
			} else {
				numpad[i]->colour = GRAY;
				// Increment since we've drawn a character
				if(graphics_field_cursor < CODE_LENGTH) {
					Graphics_DrawSquare(graphics_field_cursor,BLACK);
					graphics_field_cursor++;
				}
			}
			Graphics_DrawRectangle(numpad[i]);

			return i;
		}
	}
	return -1;
}

/*****************************************************************************************
 *	Function to convert a button index to it's corresponding number pad number.
 *
 ******************************************************************************************/
int Graphics_ButtonNumToNum(int button) {
	int num = button + 1;
	if(num == 11)
		return 0;
	return num;
}

/**
 * Returns 1 if point inside rect
 */
int Graphics_InRectangle(int point_x, int point_y, Rectangle_t* rect) {

//	if(point_x < rect->x2 && point_x > rect->x1 && point_y < rect->y2 && point_y > rect->y1)
//
//	float area = Graphics_AreaRect(rect);
//
//	// Left side to point area
//	float a1 = Graphics_AreaTriangle(point_x,point_y,rect->x1,rect->y1,rect->x1,rect->y2);
//	// Top side to point area
//	float a2 = Graphics_AreaTriangle(point_x,point_y,rect->x1,rect->y1,rect->x2,rect->y1);
//	// Right side to point area
//	float a3 = Graphics_AreaTriangle(point_x,point_y,rect->x2,rect->y2,rect->x2,rect->y1);
//	// Bottom side to point area
//	float a4 = Graphics_AreaTriangle(point_x,point_y,rect->x1,rect->y2,rect->x2,rect->y2);
//
//	//return (a1 + a2 + a3 + a4) <= area;
	return point_x < rect->x2 && point_x > rect->x1 && point_y < rect->y2 && point_y > rect->y1;
}

/**
 * Returns 1 if point inside request code rect
 */
int Graphics_RequestCodeTouched(int point_x, int point_y) {
	int isTouched = Graphics_InRectangle(point_x,point_y,request_code_rect);
	if(isTouched) {
		request_code_rect->colour = BLACK;
		Graphics_DrawRectangle(request_code_rect);
		usleep(50000);
		request_code_rect->colour = GREEN;
		Graphics_DrawRectangle(request_code_rect);
	}
	return isTouched;
}

/**
 * Returns the rough area of a rectangle given top left (x1,y1) and bottom right (x2,y2)
 */
float Graphics_AreaRect(Rectangle_t* rect) {
	int x1 = rect->x1;
	int y1 = rect->y1;
	int x2 = rect->x2;
	int y2 = rect->y2;

	float area = (y2 - y1)*(x2-x1) + (y1-y2)*(x1-x2);

	area = area < 0.0 ? area * -1.0 : area;

	area = 1.0/2.0 * area;

	return area;
}

/**
 * Returns the rough area of a rectangle triangle given 3 points
 */
float Graphics_AreaTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	int area = 1.0/2.0 * (x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2));

	area = area < 0 ? area * -1 : area;

	return area;
}
