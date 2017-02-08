/*
 * leds.h
 *
 *  Created on: Feb 1, 2017
 *      Author: David
 */
#include <stdint.h>

#ifndef LEDS_H_
#define LEDS_H_

#define Leds_Base (*(volatile uint8_t *)(0x00002010))

/**
 * Writes a value to the switches
 */
void Leds_WriteVal(uint8_t val);

/**
 * Celebration led
 */
void Leds_Celebrate();

#endif /* LEDS_H_ */
