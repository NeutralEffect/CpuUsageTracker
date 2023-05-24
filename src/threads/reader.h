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
	/** 
	 * Mutex to lock on while attempting writes to output buffer. 
	 * This parameter should be shared with analyzer thread.
	*/
	MutexHandle_t outMtx;

	/** 
	 * Condition variable to signal once data has been made available to other threads through output buffer.
	 * This parameter should be shared with analyzer thread.
	*/
	CondVarHandle_t outNotEmptyCv;

	/**
	 * Condition variable to wait on while data is being consumed from output buffer.
	 * This parameter should be shared with analyzer thread.
	*/
	CondVarHandle_t outNotFullCv;

	/**
	 * Buffer to write outgoing data to.
	 * Underlying CircularBuffer_t structure must be able to hold at least one ProcStat_t structure.
	 * This parameter should be shared with analyzer thread.
	*/
	CircularBuffer_t* outBuf;
}
ReaderThreadParams_t;


/**
 * \brief Thread function for reading /proc/stat file and parsing it's content
 * into ProcStat_t structure.
 * \details Thread will periodically read and parse /proc/stat file, subsequently writing the result
 * into output buffer (outBuf) provided through params. Those writes will be only performed if mutex
 * is successfully acquired and output buffer can hold the data.
 * \param params Pointer to valid ReaderThreadParams_t structure.
*/
int ReaderThread(void* params);


#endif // !READER_H_INCLUDED
