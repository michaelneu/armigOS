#ifndef _FLASH_H
#define _FLASH_H

// Write a buffer to a flash page
void boot_program_page(uint32_t page, uint8_t *buffer);

#endif