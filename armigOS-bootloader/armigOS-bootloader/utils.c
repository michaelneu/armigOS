#include "utils.h"
#include <stdlib.h>

inline void delay_1_mys()
{
	asm volatile (
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
	);
}

void delay_ms(uint16_t count)
{
	uint16_t i, j;
	
	for (i = 0; i < count; i++)
	{
		// usually 1000 cycles, but there's "noise" of the for loop
		for (j = 0; j < 726; j++)
		{
			delay_1_mys();
		}
	}
}

inline void round_robin(volatile int *number, const int threshold)
{
	(*number)++;
	
	if (*number == threshold)
	{
		*number = 0;
	}
}