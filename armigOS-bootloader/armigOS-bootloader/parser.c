#include "parser.h"
#include <stdbool.h>

inline bool char_in_range(char c, char from, char to)
{
	return c >= from && c <= to;
}

inline int8_t char_hex_to_number(char c)
{
	if (char_in_range(c, '0', '9'))
	{
		return c - '0';
	}
	else
	{
		return c - 'A' + 10;
	}
}

int8_t byte_string_to_byte(char string[])
{
	int8_t byte = -1;
	
	if (string[0] != ':' && string[1] != ':')
	{
		byte = char_hex_to_number(string[0]);
		byte = (byte << 4) + char_hex_to_number(string[1]);
	}
	
	return byte;
}