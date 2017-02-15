/*
 * main.c
 *
 *  Created on: Jan 21, 2017
 *      Author: David Jung
 */

#include <stdio.h>

#include "serial.h"


int main()
{
	// initialise touchscreen
	Wifi_Init();

	printf("Starting wifi\n");

	int i;
	for(i=0;i<10;i++) {
		Wifi_SendCommand("\r\n");
	}
	Wifi_WaitReady();
	Wifi_SendCommand("dofile(\"system.lua\")\r\n");
	Wifi_WaitReady();
	Wifi_SendCommand("check_wifi()\r\n");
	Wifi_WaitReady();
	Wifi_SendCommand("build_post_request(HOST, URI, \"\")\r\n");
	Wifi_WaitReady();
	Wifi_SendCommand("send_sms(\"+14387000752\",\"+17789524378\",\"Hello!\")\r\n");
	Wifi_WaitReady();

	return 0;
}
