/*
 * switches.h
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include <stdint.h>

#ifndef SWITCHES_H_
#define SWITCHES_H_

#define Switches_Base (*(volatile uint8_t *)(0x00002000))

/**
 * Reads the lower byte of the switches SW[7..0]
 */
uint8_t Switches_GetValue();

#endif /* SWITCHES_H_ */
