#include "flash.h"
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

inline void flash_erase_page(uint32_t page)
{
	boot_page_erase(page);
	boot_spm_busy_wait();
}

void flash_erase_full()
{
	uint32_t page;
	
	for (page = 0; page < 224; page += 128)
	{
		flash_erase_page(page);
	}
}

void flash_program_page(uint32_t page, uint8_t *buffer)
{
	uint16_t i;
	uint8_t sreg;
	
	sreg = SREG;
	cli();
	
	eeprom_busy_wait();
	
	for (i = 0; i < SPM_PAGESIZE; i += 2)
	{
		uint16_t word = *buffer++;
		
		word += (*buffer++) << 8;
		
		boot_page_fill(page + i, word);
	}
	
	boot_page_write(page);
	boot_spm_busy_wait();
	
	boot_rww_enable();
	
	SREG = sreg;
}