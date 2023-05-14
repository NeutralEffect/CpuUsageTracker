/**
 * \file circbuf_types.h
 * Private type definitions for usage in circular buffer implementation.
*/
#ifndef CIRCBUF_TYPES_H_INCLUDED
#define CIRCBUF_TYPES_H_INCLUDED
#include <stdint.h>
#include <stddef.h>


/**
 * Single-byte type.
*/
typedef char Byte_t;


/**
 * Circular buffer control structure.
*/
struct CircularBuffer
{
	/**
	 * Pointer to data buffer.
	*/
	void* buffer;

	/**
	 * Maximum item capacity of this circular buffer.
	*/
	uint32_t capacity;

	/**
	 * Current read offset.
	*/
	uint32_t readOffset;
	
	/**
	 * Number of items currently held within buffer.
	*/
	uint32_t itemCount;

	/**
	 * Size of single item in bytes.
	*/
	size_t itemSize;
};


#endif // !CIRCBUF_TYPES_H_INCLUDED
