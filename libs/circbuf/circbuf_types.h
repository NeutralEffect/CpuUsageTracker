#ifndef CIRCBUF_TYPES_H_INCLUDED
#define CIRCBUF_TYPES_H_INCLUDED
#include <stdint.h>
#include <stddef.h>


typedef char Byte_t;


struct CircularBuffer
{
	void* buffer;
	uint32_t capacity;
	uint32_t readOffset;
	uint32_t itemCount;
	size_t itemSize;
};


#endif // !CIRCBUF_TYPES_H_INCLUDED
