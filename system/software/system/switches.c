/*
 * switches.c
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include "switches.h"
#include <stdint.h>

uint8_t Switches_GetValue() {
	return Switches_Base;
}
