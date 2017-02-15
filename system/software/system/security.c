/*
 * security.c
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include <stdlib.h>
#include "security.h"

void Security_GenerateCode() {
	// Generates a random code
	srand(time(NULL));

	rand() % 10;

	int i;
	for (i = 0; i < CODE_LENGTH; i++) {
		// Security code is randomized
		Security_Code[i] = rand() % 10;
		// Input code all set to -1
		User_Input[i] = -1;
	}
}

int Security_CheckCode() {
	int i;

	for (i = 0; i < CODE_LENGTH; i++) {
		if (User_Input[i] != Security_Code[i]) {
			// Codes do not match
			return 0;
		}
	}
	return 1;
}

void Security_RegisterOwner() {
	printf("Sending Master Code\n");

	char master_code[16];
	int i = 0;
	int index = 0;
	for (i = 0; i < 6; i++)
		index += snprintf(&master_code[index], 16 - index, "%d",
				User_Master_Code[i]);

	char phone_num[16];
	i = 0;
	index = 0;
	for (i = 0; i < 10; i++)
		index += snprintf(&phone_num[index], 16 - index, "%d",
				User_Phone_Number_Input[i]);

	// Send out the new code for this box
	Wifi_EnsurePut(1, master_code, "0000", "false", phone_num);
	printf("Master Code Sent\n");
}
