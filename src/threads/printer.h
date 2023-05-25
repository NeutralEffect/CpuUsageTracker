/**
 * \file printer.h
 * Printer thread interface.
*/
#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED
#include "sync_types.h"
#include "circbuf.h"
#include "cpuusage.h"


/**
 * Paramters required by PrinterThread() function.
*/
typedef struct PrinterThreadParams
{
	/**
	 * Mutex to lock on while reading usage statistics from input buffer.
	 * This parameter should be shared with analyzer thread.
	*/
	MutexHandle_t inMtx;
	
	/**
	 * Condition variable to wait on while no data is available in the input buffer.
	 * This parameter should be shared with analyzer thread.
	*/
	CondVarHandle_t inNotEmptyCv;

	/**
	 * Condition variable to signal once data has been consumed from input buffer.
	 * This parameter should be shared with analyzer thread.
	*/
	CondVarHandle_t inNotFullCv;

	/**
	 * Input buffer to read CPU usage statistics from.
	 * Underlying CircularBuffer_t structure must be able to hold at least one CpuUsageInfo_t structure,
	 * size equal to that retrieved by CpuUsageInfo_size() function.
	 * This parameter should be shared with analyzer thread.
	*/
	CircularBuffer_t* inBuf;
}
PrinterThreadParams_t;


/**
 * \brief Thread function for retrieving information about CPU usage and printing it to standard output.
 * \details Thread will periodically read data from provided buffer and print it to
 * standard output using predefined format showing usage of every logical processor in percentages.
 * \param params Pointer to valid PrinterThreadParams_t structure.
*/
int PrinterThread(void* params);


#endif // !PRINTER_H_INCLUDED
