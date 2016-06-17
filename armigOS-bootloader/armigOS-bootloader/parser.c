#include "parser.h"
#include <stdbool.h>

#define IN_RANGE(c, from, to) (c >= from && c <= to)
#define TO_BYTE(c) (IN_RANGE(c, '0', '9') ? c - '0' : (c - 'A' + 10))

int8_t byte_string_to_byte(char string[])
{
	int8_t byte = -1;
	
	if (string[0] != ':' && string[1] != ':')
	{
		byte = TO_BYTE(string[0]);
		byte = (byte << 4) + TO_BYTE(string[1]);
	}
	
	return byte;
}