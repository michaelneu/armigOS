#ifndef _FLASH_H
#define _FLASH_H

#include <avr/boot.h>

// Write a buffer to a flash page
void flash_program_page(uint32_t page, uint8_t *buffer);

#endif