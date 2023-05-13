#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED
#include <threads.h>
#include "circbuf.h"
#include "cpuusage.h"


typedef struct AnalyzerThreadParams
{
	mtx_t* inputMutex;
	CircularBuffer_t* inputBuffer;
	mtx_t* outputMutex;
	CpuUsageInfo_t* outputBuffer;
}
AnalyzerThreadParams_t;


int AnalyzerThread(void* params);


#endif // !ANALYZER_H_INCLUDED