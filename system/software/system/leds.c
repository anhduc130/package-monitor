/*
 * leds.c
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include <stdint.h>
#include <stdlib.h>

#include "leds.h"

void Leds_WriteVal(uint8_t val) {
	Leds_Base = val;
}

void Leds_Celebrate() {
	int i = 0;
	for(i=0;i<1024;i++) {
		Leds_Base = 0xff;
		usleep(100000);
		Leds_Base = 0x00;
		usleep(100000);
	}

	for(;i<1024;i++) {
		Leds_Base = i;
		usleep(100000);
	}
}


