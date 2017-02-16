/*
 * security.h
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#include "graphics.h"

//#define STATE_WELCOME_SCREEN		0
//#define STATE_DRAW_NUMPAD 			1
//#define STATE_ENTER_CODE   			2
//#define STATE_LOCKED_OUT   			3
//#define STATE_OPEN         			4
//#define STATE_DRAW_WELCOME_SCREEN 	5
//#define STATE_NUMPAD				6
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
#define STATE_USER_PHONENUM			15
#define STATE_WAIT_APPROVED			16

#define CODE_LENGTH 4

void Security_DeleteCode();
int Security_CheckCode();

/**
 * Sends a put request to our server with the user information
 */
void Security_RegisterOwner();
int Security_ObtainValues();
int Security_WaitApproved();
void Security_SendConfirmedSMS();

volatile int Security_Code[CODELENGTH]; // temporary password
volatile int Security_Code_Input[CODELENGTH]; // temporary password current input
volatile int User_Phone_Number[PHONENUMLENGTH];

volatile int Master_Phone_Number[PHONENUMLENGTH]; // owner's phone number
volatile int Master_Code[MASTERCODELENGTH]; // owner's master code
volatile int Master_Code_Input[MASTERCODELENGTH]; // owner's master code input


#endif /* SECURITY_H_ */
