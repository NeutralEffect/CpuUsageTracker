#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED
#include "circbuf.h"
#include <threads.h>


typedef struct AnalyzerThreadParams
{
	mtx_t* inputDataMutex;
	CircularBuffer_t* inputDataBuffer;
	mtx_t* outputDataMutex;
	CircularBuffer_t* outputDataBuffer;
}
AnalyzerThreadParams_t;


int AnalyzerThread(void* param);


#endif // !ANALYZER_H_INCLUDED