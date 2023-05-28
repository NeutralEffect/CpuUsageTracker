#include "circbuf.h"
#include <assert.h>


#define TEST_CBUF_CAPACITY 10


static const double TEST_DATA[] =
{
	1001.0, 1002.0, 1003.0, 1004.0, 1005.0, 1006.0, 1007.0, 1008.0, 1009.0, 1010.0,
	2001.0, 2002.0, 2003.0, 2004.0, 2005.0, 2006.0, 2007.0, 2008.0, 2009.0, 2010.0,
};

static const size_t TEST_DATA_LENGTH = sizeof TEST_DATA / sizeof *TEST_DATA;


static void test_CircularBuffer_write(void)
{
	CircularBuffer_t* cbuf = CircularBuffer_create(sizeof(double), TEST_CBUF_CAPACITY);
	assert(NULL != cbuf); // Circular buffer couldn't be created

	assert(0 == CircularBuffer_getItemCount(cbuf)); // Circular buffer is not initially clear

	CircularBuffer_write(cbuf, &TEST_DATA[0]);

	assert(0 != CircularBuffer_getItemCount(cbuf)); // Circular buffer is empty despite writing an item to it

	assert(1 == CircularBuffer_getItemCount(cbuf)); // There are more items in circular buffer than one

	CircularBuffer_writeMany(cbuf, TEST_DATA + 1, TEST_CBUF_CAPACITY - 1);

	assert(CircularBuffer_getItemCount(cbuf) == CircularBuffer_getCapacity(cbuf)); // Circular buffer is not full despite writing enough items to fill it's capacity

	CircularBuffer_write(cbuf, &TEST_DATA[TEST_DATA_LENGTH - 1]);

	assert(CircularBuffer_getItemCount(cbuf) == CircularBuffer_getCapacity(cbuf)); // An item has not been overwritten

	CircularBuffer_destroy(cbuf);
}


static void test_CircularBuffer_tryWrite(void)
{
	CircularBuffer_t* cbuf = CircularBuffer_create(sizeof(double), TEST_CBUF_CAPACITY);
	assert(NULL != cbuf); // Circular buffer couldn't be created

	assert(CircularBuffer_isEmpty(cbuf)); // Circular buffer is not created empty

	assert(CircularBuffer_tryWrite(cbuf, &TEST_DATA[0])); // Item has not been written into circular buffer despite it being empty

	assert((TEST_CBUF_CAPACITY - 1) == CircularBuffer_tryWriteMany(cbuf, TEST_DATA + 1, TEST_CBUF_CAPACITY - 1)); // Amount of items written to circular buffer is different from the amount that has been requested

	assert(CircularBuffer_isFull(cbuf)); // Circular buffer is not full

	assert(!CircularBuffer_tryWrite(cbuf, &TEST_DATA[TEST_DATA_LENGTH - 1])); // An item has been written to buffer despite it being at maximum capacity

	CircularBuffer_destroy(cbuf);
}


static void test_CircularBuffer_read(void)
{
	CircularBuffer_t* cbuf = CircularBuffer_create(sizeof(double), TEST_CBUF_CAPACITY);
	assert(NULL != cbuf); // Circular buffer couldn't be created

	double item = 501.0;

	assert(CircularBuffer_isEmpty(cbuf)); // Circular buffer is not created empty

	assert(!CircularBuffer_read(cbuf, &item)); // An item has been read from empty circular buffer

	CircularBuffer_write(cbuf, &TEST_DATA[TEST_DATA_LENGTH - 1]);

	assert(1 == CircularBuffer_getItemCount(cbuf)); // There is different amount of items inside circular buffer than has been written into it

	assert(CircularBuffer_read(cbuf, &item)); // An item couldn't be read from circular buffer despite it not being empty

	assert(item != 501.0); // No data has been read from circular buffer

	assert(TEST_DATA[TEST_DATA_LENGTH - 1] == item); // Data read from buffer differs from data written into it

	assert(CircularBuffer_isEmpty(cbuf)); // Circular buffer is not empty despite reading only item that has been written into it

	assert(!CircularBuffer_read(cbuf, &item)); // An item has been read from empty circular buffer

	assert(TEST_DATA[TEST_DATA_LENGTH - 1] == item); // An output buffer has been altered after a failed read from circular buffer

	CircularBuffer_destroy(cbuf);
}


static void test_CircularBuffer_peek(void)
{
	CircularBuffer_t* cbuf = CircularBuffer_create(sizeof(double), TEST_CBUF_CAPACITY);
	assert(NULL != cbuf); // Circular buffer couldn't be created

	assert(CircularBuffer_isEmpty(cbuf)); // Circular buffer is not created empty

	CircularBuffer_writeMany(cbuf, &TEST_DATA[TEST_DATA_LENGTH - 5], 5);

	uint32_t itemCount = CircularBuffer_getItemCount(cbuf);

	assert(5 == itemCount); // Circular buffer contains different amount of items than the amount of items written

	double item = 501.0;

	assert(CircularBuffer_peek(cbuf, &item)); // Peek failed despite circular buffer containing items

	assert(501.0 != item); // Peek did not alter the output buffer

	assert(TEST_DATA[TEST_DATA_LENGTH - 5] == item); // Value in output buffer is different from expected

	CircularBuffer_clear(cbuf);

	item = 501.0;

	assert(!CircularBuffer_peek(cbuf, &item)); // Peek on empty circular buffer succeeded

	assert(501.0 == item); // Failed peek altered the value of output buffer

	CircularBuffer_destroy(cbuf);
}


int main()
{
	test_CircularBuffer_write();
	test_CircularBuffer_read();
	test_CircularBuffer_tryWrite();
	test_CircularBuffer_peek();
	return 0;
}