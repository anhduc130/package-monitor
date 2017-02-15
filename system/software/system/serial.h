/*
 * serial.h
 *
 *  Created on: Jan 21, 2017
 *      Author: David
 */

#ifndef SERIAL_H_
#define SERIAL_H_

/*
 * Baud rates in k
 */
#define BAUD_230 (char)0x00;
#define BAUD_115 (char)0x01;
#define BAUD_57  (char)0x02;
#define BAUD_38  (char)0x03;
#define BAUD_19  (char)0x04;
#define BAUD_9   (char)0x05;


/*****************************************************************************
**  START OF RS232
*****************************************************************************/
#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))

void Init_RS232(void);

int putcharRS232(int c);

int getcharRS232( void );

int RS232TestForReceivedData(void);
/*****************************************************************************
**  END OF RS232
*****************************************************************************/


/*****************************************************************************
**  START OF TOUCHSCREEN
*****************************************************************************/
#define TouchScreen_Control (*(volatile unsigned char *)(0x84000230))
#define TouchScreen_Status (*(volatile unsigned char *)(0x84000230))
#define TouchScreen_TxData (*(volatile unsigned char *)(0x84000232))
#define TouchScreen_RxData (*(volatile unsigned char *)(0x84000232))
#define TouchScreen_Baud (*(volatile unsigned char *)(0x84000234))


/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;

/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void TS_Init(void);

/*****************************************************************************
** Disable Touch
*****************************************************************************/
void TS_DisableTouch( void );

/*****************************************************************************
** Add delay touch up to prevent debouncing
*****************************************************************************/
void TS_DelayTouchUp( void );

/*****************************************************************************
** Enable Touch
*****************************************************************************/
void TS_EnableTouch( void );

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void TS_WaitForTouch();

/*****************************************************************************
** wait for screen to be released
*****************************************************************************/
void TS_WaitForRelease();

/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*
* returns point {x=-1,y=-1} if invalid
*****************************************************************************/
Point TS_GetPress(void);

/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*
*  point {x=-1,y=-1} if invalid
*****************************************************************************/
Point TS_GetRelease(void);

/*****************************************************************************
* Read Rx
*****************************************************************************/
char TS_ReadRx(void);

/*****************************************************************************
* Write Tx
*****************************************************************************/
void TS_WriteTx(char val);

/*****************************************************************************
**  END OF TOUCHSCREEN
*****************************************************************************/

/*****************************************************************************
**  START OF WIFI
*****************************************************************************/
#define Wifi_Control (*(volatile unsigned char *)(0x84000240))
#define Wifi_Status (*(volatile unsigned char *)(0x84000240))
#define Wifi_TxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_RxData (*(volatile unsigned char *)(0x84000242))
#define Wifi_Baud (*(volatile unsigned char *)(0x84000244))

#define PW_INDEX        1
#define PWMASTER_INDEX  2
#define PHONENUM_INDEX  3
#define CONFIRM_INDEX   4
#define NPARAMS         5

#define EJSON   1 // json object not found
#define EPHN    2 // phone number not found
#define EPW     3 // password not found
#define EPWMAS  4 // master password not found
#define EPCNF   5 // confirmed not found

/*****************************************************************************
** Initialise wifi controller
*****************************************************************************/
void Wifi_Init(void);

/*****************************************************************************
* Wait until ready for command
*****************************************************************************/
void Wifi_WaitReady(void);

/*****************************************************************************
* Send a command should end in \r\n
*****************************************************************************/
void Wifi_SendCommand(const char * command);

/*****************************************************************************
* Read Rx
*****************************************************************************/
char Wifi_ReadRx(void);

/*****************************************************************************
* Write Tx
*****************************************************************************/
void Wifi_WriteTx(char val);

/*****************************************************************************
* Parse out the json from response
*****************************************************************************/
void Wifi_ExtractJson(char *src, char *dst);

/*****************************************************************************
* Parse out the phone number from response
*****************************************************************************/
void Wifi_ParsePhoneNumber(char *src, char *dst);

/*****************************************************************************
* Parse out the password from response
*****************************************************************************/
void Wifi_ParsePw(char *src, char *dst);

/*****************************************************************************
* Parse out the master password from response
*****************************************************************************/
void Wifi_ParseMasterPw(char *src, char *dst);

/*****************************************************************************
* Parse out confirmed from response
*****************************************************************************/
void Wifi_ParseConfirmed(char *src, char *dst);

/*****************************************************************************
**  END OF WIFI
*****************************************************************************/
#endif /* SERIAL_H_ */
