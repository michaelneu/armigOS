#include "ringbuffer.h"
#include "utils.h"

static volatile int RINGBUFFER_READ_INDEX = 0,
					RINGBUFFER_WRITE_INDEX = 0,
					RINGBUFFER_CURRENT_SIZE = 0;

static volatile unsigned char RINGBUFFER[RINGBUFFER_LENGTH],
							  RINGBUFFER_RETURN_BUFFER[RINGBUFFER_LENGTH];

inline bool ringbuffer_almost_full()
{
	return RINGBUFFER_CURRENT_SIZE == RINGBUFFER_THRESHOLD_WRITE;
}

void ringbuffer_append(char c)
{
	RINGBUFFER[RINGBUFFER_WRITE_INDEX] = c;
	RINGBUFFER_CURRENT_SIZE++;
	
	round_robin(&RINGBUFFER_WRITE_INDEX, RINGBUFFER_LENGTH);
}

inline bool ringbuffer_can_be_read()
{
	return RINGBUFFER_CURRENT_SIZE > 0;
}

char ringbuffer_get_char()
{
	char c = RINGBUFFER[RINGBUFFER_READ_INDEX];
	
	RINGBUFFER_CURRENT_SIZE--;
	
	round_robin(&RINGBUFFER_READ_INDEX, RINGBUFFER_LENGTH);
	
	return c;
}