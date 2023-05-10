#include "circbuf.h"
#include "circbuf_types.h"
#include <stdlib.h>
#include <string.h>


#define offsetBy(basePtr, offset, unitSize) ((Byte_t*)basePtr + offset * unitSize)


static inline uint32_t uint32Min(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}


static inline uint32_t offsetBackward(uint32_t capacity, uint32_t index, uint32_t offset)
{
	if (index >= offset)
	{
		return index - offset;
	}
	else
	{
		return capacity - (offset - index) % capacity;
	}
}


static inline uint32_t offsetForward(uint32_t capacity, uint32_t index, uint32_t offset)
{
	return (index + offset) % capacity;
}


CircularBuffer_t* CircularBuffer_create(size_t itemSize, uint32_t capacity)
{
	CircularBuffer_t* self = malloc(sizeof(CircularBuffer_t));

	if (NULL == self)
	{
		return NULL;
	}

	self->buffer = malloc(itemSize * capacity);

	if (NULL == self->buffer)
	{
		free(self);
		return NULL;
	}

	self->capacity = capacity;
	self->itemSize = itemSize;
	self->readOffset = 0u;
	self->itemCount = 0u;
	return self;
}


void CircularBuffer_destroy(CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return;
	}

	free(self->buffer);
	free(self);
}


void CircularBuffer_clear(CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return;
	}

	self->itemCount = 0u;
	self->readOffset = 0u;
}


bool CircularBuffer_tryWrite(CircularBuffer_t* self, const void* itemPtr)
{
	if ( (NULL == self) || (NULL == itemPtr) || (self->itemCount >= self->capacity) )
	{
		return false;
	}
	
	CircularBuffer_write(self, itemPtr);

	return true;
}


void CircularBuffer_write(CircularBuffer_t* self, const void* itemPtr)
{
	if ( (NULL == self) || (NULL == itemPtr) )
	{
		return;
	}

	uint32_t writeOffset = self->readOffset + self->itemCount;

	if (writeOffset >= self->capacity)
	{
		writeOffset -= self->capacity;
	}

	if ( (writeOffset == self->readOffset) && (0u < self->itemCount))
	{
		self->readOffset = (writeOffset + 1u < self->capacity) ? writeOffset + 1u : 0u;
	}
	else
	{
		++self->itemCount;
	}

	void* writePtr = offsetBy(self->buffer, writeOffset, self->itemSize);

	memcpy(writePtr, itemPtr, self->itemSize);
}


bool CircularBuffer_read(CircularBuffer_t* self, void* itemOutPtr)
{
	if ( (NULL == self) || (NULL == itemOutPtr) || (1u > self->itemCount) )
		return false;

	CircularBuffer_peek(self, itemOutPtr);
	--self->itemCount;

	if (++self->readOffset == self->capacity)
	{
		self->readOffset = 0u;
	}

	return true;
}


bool CircularBuffer_peek(const CircularBuffer_t* self, void* itemOutPtr)
{
	if ( (NULL == self) || (NULL == itemOutPtr) || (1u > self->itemCount) )
	{
		return false;
	}

	const void* readPtr = ((Byte_t*)self->buffer) + self->readOffset * self->itemSize;

	memcpy(itemOutPtr, readPtr, self->itemSize);

	return false;
}


uint32_t CircularBuffer_tryWriteMany(CircularBuffer_t* self, const void* inputBuffer, uint32_t itemCount)
{
	if ( (NULL == self) || (NULL == inputBuffer) || (0 == itemCount) )
	{
		return 0u;
	}

	uint32_t availableCapacity = self->capacity - self->itemCount;
	uint32_t writeCount = uint32Min(itemCount, availableCapacity);
	CircularBuffer_writeMany(self, inputBuffer, writeCount);
	return writeCount;
}


void CircularBuffer_writeMany(CircularBuffer_t* self, const void* inputBuffer, uint32_t itemCount)
{
	if ( (NULL == self) || (NULL == inputBuffer) || (1u > itemCount) )
	{
		return;
	}

	const size_t itemSize = CircularBuffer_getItemSize(self);

	while (itemCount-- > 0u)
	{
		CircularBuffer_write(self, inputBuffer);
		inputBuffer = (const char*)inputBuffer + itemSize;
	}
}


uint32_t CircularBuffer_readMany(CircularBuffer_t* self, void* outputBuffer, uint32_t itemCount)
{
	if ( (NULL == self) ||  (NULL == outputBuffer) || (1u > itemCount) )
	{
		return 0u;
	}

	uint32_t itemsReadCount = 0u;
	const size_t itemSize = CircularBuffer_getItemSize(self);

	while ( (itemsReadCount < itemCount) && (CircularBuffer_read(self, outputBuffer)) )
	{
		outputBuffer = (Byte_t*)outputBuffer + itemSize;
		++itemsReadCount;
	}

	return itemsReadCount;
}


uint32_t CircularBuffer_peekMany(const CircularBuffer_t* self, void* outputBuffer, uint32_t itemCount)
{
	if ( (NULL == self) || (NULL == outputBuffer) || (1u > itemCount) )
	{
		return 0u;
	}

	uint32_t itemsPeekCount = 0u;
	const size_t itemSize = CircularBuffer_getItemSize(self);

	while ( (itemsPeekCount < itemCount) && (CircularBuffer_peek(self, outputBuffer)) )
	{
		outputBuffer = (Byte_t*)outputBuffer + itemSize;
		++itemsPeekCount;
	}

	return itemsPeekCount;
}


uint32_t CircularBuffer_getCapacity(const CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return 0u;
	}

	return self->capacity;
}


uint32_t CircularBuffer_getItemCount(const CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return 0u;
	}

	return self->itemCount;
}


bool CircularBuffer_isEmpty(const CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return true;
	}

	return 0u == self->itemCount;
}


bool CircularBuffer_isFull(const CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return false;
	}

	return self->itemCount == self->capacity;
}


size_t CircularBuffer_getItemSize(const CircularBuffer_t* self)
{
	if (NULL == self)
	{
		return 0u;
	}

	return self->itemSize;
}