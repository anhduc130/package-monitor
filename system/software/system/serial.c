/*
 * touchscreen.c
 *
 *  Created on: Jan 21, 2017
 *      Author: David
 */

#include <stdio.h>
#include "serial.h"
#define isascii(c)  ((c & ~0x7F) == 0)

/*****************************************************************************
**  START OF TOUCHSCREEN
*****************************************************************************/
void TS_Init(void)
{
	// Reset
	TouchScreen_Control = 0x03;

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	TouchScreen_Control = 0x15;

	// set 9600 Baud
	TouchScreen_Baud = 0x07;

	// send touchscreen controller an "enable touch" command
	TS_EnableTouch();
}

void TS_DisableTouch( void )
{
	// send touchscreen controller a "disable touch" command
	TS_WriteTx(0x55);
	TS_WriteTx(0x01);
	TS_WriteTx(0x13);
}

void TS_EnableTouch( void )
{
	// send touchscreen controller an "enable touch" command
	TS_WriteTx(0x55);
	TS_WriteTx(0x01);
	TS_WriteTx(0x12);
}


void TS_WaitForTouch()
{
	while(TS_ReadRx() != (char)0x81);
}

void TS_WaitForRelease()
{
	while(TS_ReadRx() != (char)0x80);
}


Point TS_GetPress(void)
{
	Point p1;
	// wait for a pen down command then return the X,Y coord of the point
	// calibrated correctly so that it maps to a pixel on screen
	u_int x_cord_lower;
	u_int x_cord_upper;

	u_int y_cord_lower;
	u_int y_cord_upper;

	x_cord_lower = TS_ReadRx();
	x_cord_upper = TS_ReadRx();
	y_cord_lower = TS_ReadRx();
	y_cord_upper = TS_ReadRx();

	p1.x = (double)((x_cord_upper << 7) | x_cord_lower)/4095.0 * 800;
	p1.y = (double)((y_cord_upper << 7) | y_cord_lower)/4095.0 * 450;

	/*
	 * Change it to -1 so that we can check later if the touch values are valid
	 */
	if(p1.x > 800 || p1.y > 450) {
		p1.x = -1;
		p1.y = -1;
	}

	return p1;
}


char TS_ReadRx(void) {
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while((TouchScreen_Status & 0x01) != 0x01) {}
	char val = TouchScreen_RxData;
	return val;
}

void TS_WriteTx(char val) {
	// poll tx bit in 6850 status register. Wait for it to become '2'
	while((TouchScreen_Status & 0x02) != 0x02) {}
	TouchScreen_TxData = val;
}


Point TS_GetRelease(void)
{
	Point p1;
	// wait for a pen up command then return the X,Y coord of the point
	u_int x_cord_lower;
	u_int x_cord_upper;

	u_int y_cord_lower;
	u_int y_cord_upper;

	x_cord_lower = TS_ReadRx();
	x_cord_upper = TS_ReadRx();
	y_cord_lower = TS_ReadRx();
	y_cord_upper = TS_ReadRx();

	p1.x = (double)((x_cord_upper << 7) | x_cord_lower)/4095.0 * 800;
	p1.y = (double)((y_cord_upper << 7) | y_cord_lower)/4095.0 * 450;

	/*
	 * Change it to -1 so that we can check later if the touch values are valid
	 */
	if(p1.x > 800 || p1.y > 450) {
		p1.x = -1;
		p1.y = -1;
	}
	// calibrated correctly so that it maps to a pixel on screen
	return p1;
}
/*****************************************************************************
**  END OF TOUCHSCREEN
*****************************************************************************/


/*****************************************************************************
**  START OF RS232
*****************************************************************************/
void Init_RS232(void) {
	// Reset
	RS232_Control = 0x03;

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	RS232_Control = 0x15;

	// program baud rate generator to use 115k baud
	RS232_Baud = BAUD_115;

}

int putcharRS232(int c) {
	// poll Tx bit in 6850 status register. Wait for it to become '1'
	while((RS232_Status & 0x02) != 0x02) {}
	// write 'c' to the 6850 TxData register to output the character

	RS232_TxData = c;

	return c;
}

int getcharRS232( void ) {
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while((RS232_Status & 0x01) != 0x01) {}
	// read received character from 6850 RxData register
	return RS232_RxData;
}

int RS232TestForReceivedData(void) {
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
	return ((RS232_Status & 0x1) == 0x01);
}
/*****************************************************************************
**  END OF RS232
*****************************************************************************/

/*****************************************************************************
**  START OF WIFI
*****************************************************************************/

/*****************************************************************************
** Initialise wifi controller
*****************************************************************************/
void Wifi_Init(void) {
	// Reset
	Wifi_Control = 0x03;

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Wifi_Control = 0x15;

	// set 115200 Baud
	Wifi_Baud = 0x01;

	usleep(500000);
}


/*****************************************************************************
* Wait until ready for command
*****************************************************************************/
void Wifi_WaitReady(void) {
	char val = Wifi_ReadRx();
	if(isascii(val)) {
		printf("%c",val);
	}
	// keep reading the incoming data
	while(val != '>') {
		val = Wifi_ReadRx();
		if(isascii(val)) {
			printf("%c",val);
		}
	}
	Wifi_SendCommand("\r\n");
	printf('\n');
}


/*****************************************************************************
* Send a command should end in \r\n
*****************************************************************************/
void Wifi_SendCommand(const char * command) {
	int cur_1=0;
	while(command[cur_1] != '\n') {
		Wifi_WriteTx(command[cur_1]);
		printf("%c",command[cur_1]);
		cur_1++;
	}
	Wifi_WriteTx(command[cur_1]);
}


/*****************************************************************************
* Read Rx
*****************************************************************************/
char Wifi_ReadRx(void) {
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while((Wifi_Status & 0x01) != 0x01) {}
	char val = Wifi_RxData;
	return val;
}


/*****************************************************************************
* Write Tx
*****************************************************************************/
void Wifi_WriteTx(char val) {
	// poll tx bit in 6850 status register. Wait for it to become '2'
	while((Wifi_Status & 0x02) != 0x02) {}
	Wifi_TxData = val;
}

/*****************************************************************************
**  END OF WIFI
*****************************************************************************/
