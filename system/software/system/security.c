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
	for(i=0;i<CODE_LENGTH;i++) {
		// Security code is randomized
		Security_Code[i] = rand() % 10;
		// Input code all set to -1
		User_Input[i] = -1;
	}
}

int Security_CheckCode() {
	int i;

	for(i=0;i<CODE_LENGTH;i++) {
		if(User_Input[i] != Security_Code[i]) {
			// Codes do not match
			return 0;
		}
	}
	return 1;
}
