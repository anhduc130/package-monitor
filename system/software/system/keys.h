/*
 * keys.h
 *
 *  Created on: Feb 1, 2017
 *      Author: r3z9a
 */
#include <stdint.h>

#ifndef KEYS_H_
#define KEYS_H_

#define Keys_Base (*(volatile uint8_t *)(0x00002060))

/**
 * Reads the keys
 */
uint8_t Keys_GetValue();

#endif /* KEYS_H_ */
