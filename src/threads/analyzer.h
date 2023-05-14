#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED
#include <threads.h>
#include "circbuf.h"
#include "cpuusage.h"

/**
 * Paramters required by AnalyzerThread() function.
*/
typedef struct AnalyzerThreadParams
{
	/**
	 * Mutex to lock on while reading data from input buffer.
	*/
	mtx_t* inputMutex;

	/**
	 * Buffer to read incoming data from. This parameter must point to CircularBuffer_t
	 * structure shared with reader thread and capable of holding at least one ProcStat_t structure.
	*/
	CircularBuffer_t* inputBuffer;

	/**
	 * Mutex to lock on while writing data to output buffer.
	*/
	mtx_t* outputMutex;

	/**
	 * Output buffer to periodically write calculated usage statistics into.
	 * Must be of equal or bigger size than that retrieved by CpuUsageInfo_size() function.
	*/
	CpuUsageInfo_t* outputBuffer;
}
AnalyzerThreadParams_t;


/**
 * \brief Thread function for analyzing data from /proc/stat file
 * and calculating usage statistics for logical processor.
 * \details Thread will periodically read data from inputBuffer and use it to calculate
 * usage statistics for logical processors, subsequently writing them into outputBuffer.
 * \param params Pointer to valid ReaderThreadParams_t structure.
*/
int AnalyzerThread(void* params);


#endif // !ANALYZER_H_INCLUDED
