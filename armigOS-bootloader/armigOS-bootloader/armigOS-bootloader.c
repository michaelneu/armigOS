#include "flash.h"
#include "interrupt.h"
#include "parser.h"
#include "ringbuffer.h"
#include "uart.h"
#include "utils.h"
#include <stdbool.h>

inline void wait_for_ringbuffer()
{
	while (!ringbuffer_can_be_read())
	{
		// busy wait for buffer to fill
	}
}

/**
 * Intel hex file line structure: 
 * 
 *             record
 *              type
 *               |
 *      data     |
 *     length    |   checksum
 *       |       |      |
 *       v       v      v
 *    ": 00 0000 00 0_0 00 \n"
 *     ^      ^      ^
 *     |      |      |
 * delimiter  |    data
 *            |
 *         address
 */
typedef enum {
	DIGEST_LENGTH,
	DIGEST_ADDRESS,
	DIGEST_RECORD_TYPE,
	DIGEST_DATA,
	DIGEST_CHECKSUM
} DIGEST_STATE;

typedef enum {
	RECORD_DATA = 0,
	RECORD_EOF = 1,
	RECORD_EXTENDED_SEGMENT_ADDRESS = 2,
	RECORD_START_SEGMENT_ADDRESS = 3,
	RECORD_EXTENDED_LINEAR_ADDRESS = 4,
	RECORD_START_LINEAR_ADRESS = 5
} RECORD_TYPE;

bool program_device()
{
	char pair[2];
	
	uint8_t current_byte,
			checksum = 0,
			data_length = 0,
			data_current_index = 0,
			buffer[SPM_PAGESIZE],
			buffer_index = 0;
			
	uint16_t address = 0;
	
	int32_t page_address = 0;
	
	bool new_address_required = true;
	
	DIGEST_STATE state = DIGEST_LENGTH;
	RECORD_TYPE record_type = RECORD_DATA;
	
	flash_erase_full();

	while (true)
	{
		wait_for_ringbuffer();
		
		pair[0] = ringbuffer_get_char();
		
		switch (pair[0])
		{
			case ':':
			case '\n':
			state = DIGEST_LENGTH;
			checksum = 0;
			break;
			
			default:
			wait_for_ringbuffer();
			
			pair[1] = ringbuffer_get_char();
			current_byte = byte_string_to_byte(pair);
			
			checksum += current_byte;
			
			switch (state)
			{
				case DIGEST_LENGTH:
				data_length = current_byte;
				state = DIGEST_ADDRESS;
				break;
				
				case DIGEST_ADDRESS:
				address = current_byte;
				
				wait_for_ringbuffer();
				pair[0] = ringbuffer_get_char();
				
				wait_for_ringbuffer();
				pair[1] = ringbuffer_get_char();
				
				current_byte = byte_string_to_byte(pair);
				checksum += current_byte;
				
				address = (address << 8) + current_byte;
				
				if (new_address_required)
				{
					new_address_required = false;
					page_address = address - address % SPM_PAGESIZE;
				}
				
				state = DIGEST_RECORD_TYPE;
				break;
				
				case DIGEST_RECORD_TYPE:
				record_type = current_byte;
				
				if (record_type == RECORD_EOF)
				{
					for (; buffer_index < SPM_PAGESIZE; buffer_index++)
					{
						buffer[buffer_index] = 0xFF;
					}
					
					flash_program_page(page_address, buffer);
					new_address_required = true;
					
					return true;
				}
				else
				{
					if (data_length == 0)
					{
						state = DIGEST_CHECKSUM;
					}
					else
					{
						data_current_index = 0;
						state = DIGEST_DATA;
					}
					break;
				}
				
				case DIGEST_DATA:
				buffer[buffer_index] = current_byte;
				buffer_index++;
				
				data_current_index++;
				
				if (data_current_index == data_length)
				{
					state = DIGEST_CHECKSUM;
				}
				break;
				
				case DIGEST_CHECKSUM:
				if (checksum != 0)
				{
					uart_send_string("\n\nChecksum mismatch!\n");
					
					return false;
				}
				else if (buffer_index == SPM_PAGESIZE)
				{
					flash_program_page(page_address, buffer);
					new_address_required = true;
					
					buffer_index = 0;
				}
				
				break;
			}
			break;
		}
	}
}

int main(void)
{
	int i;
	
	interrupt_init();
	uart_init(BAUD_9600);
	
	uart_send(FLOW_XON);
	uart_clear_terminal();
	
	// await programming input
	for (i = 0; i < 50; i++)
	{
		delay_ms(100);
		
		if (ringbuffer_can_be_read() && ringbuffer_get_char() == 'p')
		{
			uart_send_string("\npaste an Intel hex file:\n");
			
			if (!program_device())
			{
				uart_send_string("\nerror processing hex file.\n");
			}
			
			break;
		}
	}
	
	uart_clear_terminal();
	
	uart_send_string("\nstarting flashed program\n");
	interrupt_reset();
	
	// start flashed program
	void (*start)(void) = 0x0;
	start();
	
	return 0;
}