/**
 * \file reader.h
 * Reader thread interface.
*/
#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED
#include "sync_types.h"
#include "circbuf.h"


/**
 * Paramters required by ReaderThread() function.
*/
typedef struct ReaderThreadParams
{
	/** Mutex to be locked on while attempting writes to buffer. */
	mtx_t* mutex;

	/**
	 * Buffer to hold outgoing data. Underlying CircularBuffer_t structure must be able to hold at least
	 * one ProcStat_t structure. This parameter should be shared with analyzer thread.
	*/
	CircularBuffer_t* buffer;
}
ReaderThreadParams_t;


/**
 * \brief Thread function for reading /proc/stat file and parsing it's content
 * into ProcStat_t structure.
 * \details Thread will periodically read and parse /proc/stat file and write the result
 * into circular buffer provided through params. Those writes will be only performed if provided
 * mutex is available, as it will be locked for their entire duration.
 * \param params Pointer to valid ReaderThreadParams_t structure.
*/
int ReaderThread(void* params);


#endif // !READER_H_INCLUDED
