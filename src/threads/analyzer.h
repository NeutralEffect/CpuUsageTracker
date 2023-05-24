/**
 * \file analyzer.h
 * Analyzer thread interface.
*/
#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED
#include "sync.h"
#include "circbuf.h"
#include "cpuusage.h"

/**
 * Paramters required by AnalyzerThread() function.
*/
typedef struct AnalyzerThreadParams
{
	/**
	 * Mutex to lock on while reading data from input buffer.
	 * This parameter should be shared with reader thread.
	*/
	MutexHandle_t inMtx;

	/**
	 * Condition variable to wait on while no data is available in the input buffer.
	 * This parameter should be shared with reader thread.
	*/
	CondVarHandle_t inNotEmptyCv;

	/**
	 * Condition variable to signal once data has been consumed from input buffer.
	 * This parameter should be shared with reader thread.
	*/
	CondVarHandle_t inNotFullCv;

	/**
	 * Buffer to read incoming data from.
	 * Underlying CircularBuffer_t structure must be able to hold at least one ProcStat_t structure.
	 * This parameter should be shared with reader thread.
	*/
	CircularBuffer_t* inBuf;

	/**
	 * Mutex to lock on while writing data to output buffer.
	 * This parameter should be shared with printer thread.
	*/
	MutexHandle_t outMtx;

	/** 
	 * Condition variable to signal once data has been made available to other threads through output buffer.
	 * This parameter should be shared with printer thread.
	*/
	CondVarHandle_t outNotEmptyCv;

	/**
	 * Condition variable to wait on while data is being consumed from output buffer.
	 * This parameter should be shared with printer thread.
	*/
	CondVarHandle_t outNotFullCv;

	/**
	 * Output buffer to periodically write calculated usage statistics into.
	 * Must be of equal or bigger size than that retrieved by CpuUsageInfo_size() function.
	 * This parameter should be shared with printer thread.
	*/
	CpuUsageInfo_t* outBuf;
}
AnalyzerThreadParams_t;


/**
 * \brief Thread function for analyzing data from /proc/stat file
 * and calculating usage statistics for logical processor.
 * \details Thread will periodically read data from input buffer (inBuf) and use it to calculate
 * usage statistics for logical processors, subsequently writing them into output buffer (outBuf).
 * \param params Pointer to valid ReaderThreadParams_t structure.
*/
int AnalyzerThread(void* params);


#endif // !ANALYZER_H_INCLUDED
