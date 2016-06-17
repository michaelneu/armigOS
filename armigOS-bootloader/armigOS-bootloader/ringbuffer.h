#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <stdbool.h>

#ifndef RINGBUFFER_LENGTH
	#define RINGBUFFER_LENGTH 16
#endif

#ifndef RINGBUFFER_THRESHOLD_WRITE
	#define RINGBUFFER_THRESHOLD_WRITE 10
#endif

// Check if the ringbuffer is near its threshold. 
bool ringbuffer_almost_full();

// Append a character to the ringbuffer. 
void ringbuffer_append(char character);

// Check if the ringbuffer has unread contents.
bool ringbuffer_can_be_read();

// Receive a character from the ringbuffer.
char ringbuffer_get_char();

#endif