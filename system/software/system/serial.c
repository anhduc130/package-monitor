/*
 * touchscreen.c
 *
 *  Created on: Jan 21, 2017
 *      Author: David
 */
#define _GNU_SOURCE
#include <stdio.h>
#include "serial.h"
#include <string.h>

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

#define BUF_SIZE 512
#define JSON_SIZE 256
// Buffer to read the output
unsigned char rbuf[BUF_SIZE];
// Buffer containing copy
unsigned char cbuf[BUF_SIZE];
// Buffer containing json data
unsigned char jsonbuf[256];

/*****************************************************************************
** Initialise wifi controller
*****************************************************************************/
void Wifi_Init(void) {
	// Reset
	Wifi_Control = 0x03;

	// set up 6850 Control Register to utilise a divide by 16 clock,
	// set RTS low, use 8 bits of data, no parity, 1 stop bit,
	// transmitter interrupt disabled
	Wifi_Control = 0x55;

	// set 115200 Baud
	Wifi_Baud = 0x1;

	usleep(100000);

	int i;
	for(i=0;i<10;i++) {
		Wifi_SendCommand(" \r\n");
	}



	printf("Wifi inititalised\n");
}


/*****************************************************************************
* Read wifi response into buf for n bytes
*****************************************************************************/
int Wifi_ReadResponse() {
	int i;
	char data;
	for(i=0;i<BUF_SIZE;i++) {
		data = Wifi_ReadRx();
		if(isascii(data)) {
			rbuf[i] = data;
			if(data == '>')
				break;
		}
	}
	rbuf[i]='\0';
	printf("%s\n",jsonbuf);
	return i;
}

/*****************************************************************************
* Wifi print response
*****************************************************************************/
int Wifi_PrintResponse() {
	printf("%s",rbuf);
}

/*****************************************************************************
* Copy over nbytes from rbuf to to cbuf
* *****************************************************************************/
void Wifi_CopyBuffer(int nbytes) {
	// Clear the original dest
	Wifi_ClearBuffer(cbuf);
	int i;
	for(i=0;i<nbytes;i++) {
		cbuf[i] = rbuf[i];
	}
}

/*****************************************************************************
* Clear buffers BUF_SIZE
******************************************************************************/
void Wifi_ClearBuffer(unsigned char* buf) {
	int i;
	for(i=0;i<BUF_SIZE;i++) {
		buf[i] = '\0';
	}
}

/*****************************************************************************
* Keeps sending check wifi until we have connected
******************************************************************************/
void Wifi_EnsureConnection() {
	Wifi_SendCommand("check_wifi()\r\n");
	Wifi_ReadResponse();
	printf("%s\n",rbuf);
	while(strchr(rbuf,'!') == NULL) {
		// Go to sleep and try again in a bit
		usleep(500000);
		Wifi_SendCommand("check_wifi()\r\n");
		Wifi_ReadResponse();
		printf("%s\n",rbuf);
	}
}

/*****************************************************************************
* Keep sending get requests until we get a response
******************************************************************************/
void Wifi_EnsureGet(int pk) {
	char buf[16];
	snprintf(buf, sizeof buf, "send_get(%d)\r\n", pk);
	Wifi_SendCommand(buf);
	Wifi_ReadResponse();
	while(Wifi_ExtractJson(rbuf, jsonbuf) == EJSON && strlen(jsonbuf) != 79) {
		Wifi_SendCommand(buf);
		Wifi_ReadResponse();
	}

	printf("JSON GET BUFFER BELOW:\n");
	printf("%s\n",jsonbuf);
}

/*****************************************************************************
* Keep sending post requests until we get a response
* Should only be used when registering
******************************************************************************/
void Wifi_EnsurePut(int pk, const char *masterpw, const char *pw, const char *isconfirmed, const char *phonenum) {
	char buf[64];
	snprintf(buf, sizeof buf, "send_put(%d,\"%s\",\"%s\",\"%s\",\"%s\")\r\n", pk, masterpw, pw, isconfirmed, phonenum);
	Wifi_SendCommand(buf);
	Wifi_ReadResponse();
	while(Wifi_ExtractJson(rbuf, jsonbuf) == EJSON && strlen(jsonbuf) != 79) {
		Wifi_SendCommand(buf);
		Wifi_ReadResponse();
	}

	printf("JSON PUT BUFFER BELOW:\n");
	printf("%s\n",jsonbuf);
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
	// Clear the response buffer before each command
	Wifi_ClearBuffer(rbuf);
	int cur_1=0;
	while(command[cur_1] != '\n') {
		Wifi_WriteTx(command[cur_1]);
		cur_1++;
	}
	Wifi_WriteTx(command[cur_1]);
}


/*****************************************************************************
* Read Rx
*****************************************************************************/
int Wifi_ReadRx(void) {
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	while((Wifi_Status & 0x01) != 0x01) {}
	return Wifi_RxData;
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
* Parse out the json from response
*****************************************************************************/
int Wifi_ExtractJson(char *src, char *dst) {
	// Figure out the the left and right indices of the
	char *leftbracket = strchr(src, '{');
	char *rightbracket = strchr(src, '}');

	if(leftbracket == NULL || rightbracket == NULL) {
		return EJSON;
	}
	int leftindex = (int)(leftbracket - src);
	int rightindex = (int)(rightbracket - src);

	int diff = rightindex - leftindex;

	// copy the substring
	memcpy(dst, &src[leftindex+1], diff-1);
	dst[diff] = '\0';
}

/*****************************************************************************
* Parse out the phone number from response
*****************************************************************************/
int Wifi_ParsePhoneNumber(char *src, char *dst) {
	char *copy = (char *)malloc(16);
	strcpy(copy, src);

	char *res[NPARAMS];
	char *p = strtok(copy, ",");
	int i = 0;

	while(p != NULL) {
  	res[i++] = p;
  	p = strtok(NULL,",");
	}

	char *colon = strchr(res[PHONENUM_INDEX],':');
	if(colon == NULL) {
  	free(copy);
  	return EPHN;
	}

	int colonIndex = (int)(colon - res[PHONENUM_INDEX]);
	memcpy(dst, res[PHONENUM_INDEX] + colonIndex + 2, 10);
	dst[10]='\0';
	return 0;
}

/*****************************************************************************
* Parse out the password from response
*****************************************************************************/
int Wifi_ParsePw(char *src, char *dst) {
	char *copy = malloc(sizeof(char)*4);
	strcpy(copy, src);

	char *res[NPARAMS];
	char *p = strtok(copy, ",");
	int i = 0;

	while(p != NULL) {
   	res[i++] = p;
   	p = strtok(NULL,",");
	}

	char *colon = strchr(res[PW_INDEX],':');
	if(colon == NULL)
   	return EPW;

	int colonIndex = (int)(colon - res[PW_INDEX]);
	memcpy(dst, res[PW_INDEX] + colonIndex + 2, 4);
	dst[4]='\0';
	return 0;
}

/*****************************************************************************
* Parse out the master password from response
*****************************************************************************/
int Wifi_ParseMasterPw(char *src, char *dst) {
	char *copy = malloc(sizeof(char)*6);
	strcpy(copy, src);

	char *res[NPARAMS];
	char *p = strtok(copy, ",");
	int i = 0;

	while(p != NULL) {
   	res[i++] = p;
  	p = strtok(NULL,",");
	}

	char *colon = strchr(res[PWMASTER_INDEX],':');
	if(colon == NULL)
  	return EPWMAS;

	int colonIndex = (int)(colon - res[PWMASTER_INDEX]);
	memcpy(dst, res[PWMASTER_INDEX] + colonIndex + 2, 6);
	dst[6]='\0';
	return 0;
}

/*****************************************************************************
* Parse out confirmed from response
*****************************************************************************/
int Wifi_ParseConfirmed(char *src, char *dst) {
	char *copy = malloc(sizeof(char)*5);
	strcpy(copy, src);

	char *res[NPARAMS];
	char *p = strtok(copy, ",");
	int i = 0;

	while(p != NULL) {
  	res[i++] = p;
  	p = strtok(NULL,",");
	}

	char *colon = strchr(res[CONFIRM_INDEX],':');
	if(colon == NULL)
  	return EPCNF;

	int colonIndex = (int)(colon - res[CONFIRM_INDEX]);
	memcpy(dst, res[CONFIRM_INDEX] + colonIndex + 1, 5);
	dst[5]='\0';
	return 0;
}

/*****************************************************************************
**  END OF WIFI
*****************************************************************************/
