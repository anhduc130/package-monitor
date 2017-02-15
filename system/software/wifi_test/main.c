/*
 * main.c
 *
 *  Created on: Jan 21, 2017
 *      Author: David Jung
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "serial.h"


int main() {
	// Draw the number pad
	printf("Starting Program\n");

	// Initialize Hardware
	Wifi_Init();

	// Send wifi commands
	Wifi_SendCommand("print(\"Hello\")\r\n");
	int bytesread = Wifi_ReadResponse();
	Wifi_CopyBuffer(bytesread);
	Wifi_PrintResponse();
	// Send wifi commands
	Wifi_SendCommand("print(\"Hello\")\r\n");
	bytesread = Wifi_ReadResponse();
	Wifi_CopyBuffer(bytesread);
	Wifi_PrintResponse();
	// Send wifi commands
	Wifi_SendCommand("print(\"Hello\")\r\n");
	bytesread = Wifi_ReadResponse();
	Wifi_CopyBuffer(bytesread);
	Wifi_PrintResponse();

	// Send wifi commands
	Wifi_SendCommand("dofile(\"system.lua\")\r\n");
	bytesread = Wifi_ReadResponse();
	Wifi_CopyBuffer(bytesread);
	Wifi_PrintResponse();

	Wifi_SendCommand("check_wifi()\r\n");
	Wifi_CopyBuffer(bytesread);
	Wifi_PrintResponse();

	return 0;
}
