#include "flash.h"
#include "interrupt.h"
#include "parser.h"
#include "ringbuffer.h"
#include "uart.h"
#include "utils.h"

char read_character()
{
	if (ringbuffer_can_be_read())
	{
		uart_enable_flow();
		
		return ringbuffer_get_char();
	}
	else
	{
		return 0;
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

void program_device()
{
	char pair[2];
	int8_t current_byte, 
			data_bytes_to_read = 0, 
			address = 0, 
			record_type = 0, 
			checksum = 0;
		
	DIGEST_STATE state = DIGEST_LENGTH;
	
	uint8_t buffer[SPM_PAGESIZE], 
			buffer_index = 0;

	while (true)
	{
		pair[0] = read_character();
		
		switch (pair[0])
		{
			case 0:
			case ':':
			case '\n':
			state = DIGEST_LENGTH;
			break;
			
			default:
			pair[1] = read_character();
			current_byte = byte_string_to_byte(pair);
			
			switch (state)
			{
				case DIGEST_LENGTH:
				data_bytes_to_read = current_byte;
				state = DIGEST_ADDRESS;
				break;
				
				case DIGEST_ADDRESS:
				address = current_byte;
				pair[0] = read_character();
				pair[1] = read_character();
				address = (address << 4) + byte_string_to_byte(pair);
				state = DIGEST_RECORD_TYPE;
				break;
				
				case DIGEST_RECORD_TYPE:
				record_type = current_byte;
				
				if (record_type == RECORD_EOF)
				{
					return;
				}
				else
				{
					buffer_index = 0;
					state = DIGEST_DATA;
					break;
				}
				
				case DIGEST_DATA:
				if (data_bytes_to_read == 0)
				{
					data_bytes_to_read = buffer_index;
					
					for (buffer_index = buffer_index + 1; buffer_index < SPM_PAGESIZE; buffer_index++)
					{
						buffer[buffer_index] = 0;
					}
					
					// flash_program_page(address, buffer);
					state = DIGEST_CHECKSUM;
				}
				else
				{
					buffer[buffer_index] = current_byte;
					buffer_index++;
					
					data_bytes_to_read--;
				}
				break;
				
				case DIGEST_CHECKSUM:
				data_bytes_to_read = buffer_index;
				
				checksum = data_bytes_to_read + address + record_type;
				
				while (data_bytes_to_read >= 0)
				{
					checksum += buffer[data_bytes_to_read];
					data_bytes_to_read--;
				}
				
				checksum += current_byte;
				
				if (checksum != 0)
				{
					// checksum failed
					return;
				}
				
				break;
				
				default:
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
			program_device();
			
			break;
		}
	}
	
	uart_clear_terminal();
	
	// start flashed program
	void (*start)(void) = 0x0;
	start();
	
	return 0;
}