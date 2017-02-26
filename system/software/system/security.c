/*
 * security.c
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include <stdlib.h>
#include "graphics.h"
#include "serial.h"
#include "security.h"
#include <ctype.h>

void Security_GenerateCode() {
	// Generates a random code
	srand(time(NULL));

	rand() % 10;

	int i;
	for (i = 0; i < CODE_LENGTH; i++) {
		// Security code is randomized
		Security_Code[i] = rand() % 10;
		// Input code all set to -1
		Security_Code_Input[i] = -1;
	}
}

/*******************************************************************************************
 ** Check the inputed security code
 *******************************************************************************************/
int Security_CheckCode() {
	int i;

	for (i = 0; i < CODE_LENGTH; i++) {
		if (Security_Code_Input[i] != Security_Code[i]) {
			// Codes do not match
			return 0;
		}
	}
	return 1;
}

/*******************************************************************************************
 ** Register the user to id 1
 *******************************************************************************************/
void Security_RegisterOwner() {
	printf("Sending Put Request\n");

	char master_code[16];
	int i = 0;
	int index = 0;
	for (i = 0; i < 6; i++)
		index += snprintf(&master_code[index], 16 - index, "%d",
				Master_Code_Input[i]);

	char phone_num[16];
	i = 0;
	index = 0;
	for (i = 0; i < 10; i++)
		index += snprintf(&phone_num[index], 16 - index, "%d",
				Master_Phone_Number[i]);

	// Send out the new code for this box
	Wifi_EnsurePut(1, master_code, "0000", "false", phone_num);
	printf("Master Code Sent\n");
}

/*******************************************************************************************
 ** Obtain the passwords and phone number for this box, hard coded to id 1
 *******************************************************************************************/
int Security_ObtainValues() {
	printf("Sending Get Request\n");
	// Send out the new code for this box
	Wifi_EnsureGet(1);

	// Json buf should now be valid

	// Parse the master pw
	char *masterPw = malloc(sizeof(char) * MASTERCODELENGTH);
	Wifi_ParseMasterPw(jsonbuf, masterPw);
	if (!isdigit(masterPw[MASTERCODELENGTH-1])) {
		return EPWMAS; // master password not found
	}
	printf("Master pw: %s\n", masterPw);

	// Parse the pw
	char* tempPw = malloc(sizeof(char)*CODELENGTH);
	Wifi_ParsePw(jsonbuf, tempPw);
	if (!isdigit(tempPw[CODELENGTH-1])) {
		return EPW; // password not found
	}
	printf("Temp pw: %s\n", tempPw);

	// Parse the phone number
	char * phoneNum = malloc(sizeof(char)*PHONENUMLENGTH);
	Wifi_ParsePhoneNumber(jsonbuf, phoneNum);
	if (!isdigit(phoneNum[PHONENUMLENGTH-1])) {
		return EPHN; // phone number not found
	}
	printf("Phone Number: %s\n", phoneNum);

	// Assign the values to the variables
	int i;
	// Store the temporary password
	for (i = 0; i < CODELENGTH; i++) {
		Security_Code[i] = tempPw[i]  - '0';
	}
	// Store the master password
	for (i = 0; i < MASTERCODELENGTH; i++) {
		Master_Code[i] = masterPw[i] - '0';
	}
	// Store the master phone number
	for (i = 0; i < PHONENUMLENGTH; i++) {
		Master_Phone_Number[i] = phoneNum[i] - '0';
	}

	free(masterPw);
	free(tempPw);
	free(phoneNum);

	printf("Master After Obtaining values Code: %d,%d,%d,%d,%d,%d\n", Master_Code[0],
				Master_Code[1], Master_Code[2], Master_Code[3],
				Master_Code[4], Master_Code[5]);
	printf("Obtained Get Request\n");
	return 0;
}

/*****************************************************************************
* Sends an sms to the owner of the box
*****************************************************************************/
int Security_SendSMS() {
	char buf[128];
	volatile char phoneNum[11];
	volatile char userPhoneNum[11];
	int i;

	printf("Phone Number: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			User_Phone_Number[0], User_Phone_Number[1],
			User_Phone_Number[2], User_Phone_Number[3],
			User_Phone_Number[4], User_Phone_Number[5],
			User_Phone_Number[6], User_Phone_Number[7],
			User_Phone_Number[8], User_Phone_Number[9]);

	for(i=0;i<10;i++) {
		phoneNum[i] = Master_Phone_Number[i] + '0';
		userPhoneNum[i] = User_Phone_Number[i] + '0';
	}
	phoneNum[10] = '\0';
	userPhoneNum[10] = '\0';
	printf("%s\n",phoneNum);
	printf("%s\n",userPhoneNum);
	snprintf(buf, sizeof buf, "send_sms(\"+14387000752\",\"+1%s\",\"Hi a user with phone number %s is requesting to open the box!\")\r\n",phoneNum, userPhoneNum);
	printf("%s",buf);
	Wifi_SendCommand(buf);
	Wifi_ReadResponse();
}

void Security_SendConfirmedSMS() {
	char buf[128];
	volatile char userPhoneNum[11];
	volatile char tempPw[5];
	int i;

	printf("Phone Number: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			User_Phone_Number[0], User_Phone_Number[1],
			User_Phone_Number[2], User_Phone_Number[3],
			User_Phone_Number[4], User_Phone_Number[5],
			User_Phone_Number[6], User_Phone_Number[7],
			User_Phone_Number[8], User_Phone_Number[9]);

	for(i=0;i<10;i++) {
		userPhoneNum[i] = User_Phone_Number[i] + '0';
	}

	for(i=0;i<4;i++) {
		tempPw[i] = Security_Code[i] + '0';
	}
	userPhoneNum[10] = '\0';
	tempPw[4] = '\0';
	printf("%s\n",userPhoneNum);
	snprintf(buf, sizeof buf, "send_sms(\"+14387000752\",\"+1%s\",\"You have been approved the code is %s!\")\r\n", userPhoneNum, tempPw);
	printf("%s",buf);
	Wifi_SendCommand(buf);
	Wifi_ReadResponse();
}

/*******************************************************************************************
 ** Make sure the user has been confirmed to enter the code, they should recieve a text
 ** message of the code to unlock the box
 *******************************************************************************************/
int Security_WaitApproved() {
	// Wait until we get approved
	// Send out the code for this box
	Wifi_EnsureGet(1);
	// Json buf should now be valid
	char* isConfirmed = malloc(sizeof(char)*MASTERCODELENGTH);
	Wifi_ParseConfirmed(jsonbuf, isConfirmed);
	printf("%s\n",isConfirmed);

	int b = isConfirmed[0] == 't';

	free(isConfirmed);
	// Check if value is true
	return b;
}

/*******************************************************************************************
 ** Checks to see if the master code matches
 ** Returns 0 if it doesn't match 1 otherwise
 *******************************************************************************************/
int Security_CheckMasterCode() {
	int i;
	printf("Master Code Input: %d,%d,%d,%d,%d,%d\n", Master_Code_Input[0],
						Master_Code_Input[1], Master_Code_Input[2], Master_Code_Input[3],
						Master_Code_Input[4], Master_Code_Input[5]);
	printf("Master Code: %d,%d,%d,%d,%d,%d\n", Master_Code[0],
			Master_Code[1], Master_Code[2], Master_Code[3],
			Master_Code[4], Master_Code[5]);
	for (i = 0; i < MASTERCODELENGTH; i++) {
		if (Master_Code[i] != Master_Code_Input[i]) {
			// Codes do not match
			return 0;
		}
	}
	return 1;
}

/*******************************************************************************************
 ** Make sure the user has been confirmed to enter the code, they should receive a text
 ** message of the code to unlock the box
 **
 ** Returns 1 if already registered, 0 otherwise
 *******************************************************************************************/
int Security_CheckIsRegistered() {
	// Send out the code for this box
	Wifi_EnsureGet(1);
	// Json buf should now be valid
	char *masterPw = malloc(sizeof(char)*MASTERCODELENGTH);
	Wifi_ParseMasterPw(jsonbuf, masterPw);
	int isReg = masterPw[0] != 'a';
	free(masterPw);
	return isReg;
}

/*******************************************************************************************
 ** Sends an sms to the owner of this box with credentials of the person attempting to open
 *******************************************************************************************/
void Security_SendRequestOpen() {
	// Send user's phone number and request to open to the owner phone number
	// Store the user's phone number to text to it later
}
