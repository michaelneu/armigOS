#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stdbool.h>

// Wait for `ms` milliseconds
void delay_ms(uint16_t ms);

// Increment and apply round robin to a given address. 
void round_robin(volatile int *pointer, int threshold);

#endif