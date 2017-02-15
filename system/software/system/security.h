/*
 * security.h
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#define STATE_WELCOME_SCREEN		0
#define STATE_DRAW_NUMPAD 			1
#define STATE_ENTER_CODE   			2
#define STATE_LOCKED_OUT   			3
#define STATE_OPEN         			4
#define STATE_DRAW_WELCOME_SCREEN 	5
#define STATE_NUMPAD				6
#define STATE_DRAW_LOCK_SCREEN		7
#define STATE_DRAW_UNLOCK_SCREEN	8
#define STATE_LOCK_SCREEN			9
#define STATE_UNLOCK_SCREEN			10

#define CODE_LENGTH 4

void Security_DeleteCode();
int Security_CheckCode();
int Security_Code[CODE_LENGTH];
int User_Input[CODE_LENGTH];

#endif /* SECURITY_H_ */
