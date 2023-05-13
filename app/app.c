#include <stdio.h>
#include <threads.h>
#include <time.h>
#include <stdlib.h>
#include "thread_utils.h"
#include "circbuf.h"
#include "sighandlers.h"
#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "cpuusage.h"
#include "procstat.h"
#include "cpucount.h"
#include "log.h"



int main()
{
	RegisterSigintHandler();
	RegisterSigtermHandler();
	CpuCount_init();
	SetLogLevel(LLEVEL_ERROR);

	mtx_t procStatMtx;
	mtx_t usageInfoMtx;
	mtx_init(&procStatMtx, mtx_timed);
	mtx_init(&usageInfoMtx, mtx_timed);
	CircularBuffer_t* procStatCbuf = CircularBuffer_create(ProcStat_size(), 10u);
	CpuUsageInfo_t* usageInfoBuffer = malloc(CpuUsageInfo_size());

	thrd_t readerThrd;
	thrd_t analyzerThrd;
	thrd_t printerThrd;

	ReaderThreadParams_t readerThrdParams = 
	{
		.mutex 		= &procStatMtx,
		.buffer = procStatCbuf
	};
	thrd_create(&readerThrd, ReaderThread, &readerThrdParams);

	AnalyzerThreadParams_t analyzerThrdParams = 
	{
		.inputMutex 	= &procStatMtx,
		.inputBuffer 	= procStatCbuf,
		.outputMutex	= &usageInfoMtx,
		.outputBuffer	= usageInfoBuffer
	};
	thrd_create(&analyzerThrd, AnalyzerThread, &analyzerThrdParams);

	PrinterThreadParams_t printerThrdParams =
	{
		.mutex = &usageInfoMtx,
		.buffer = usageInfoBuffer
	};
	thrd_create(&printerThrd, PrinterThread, &printerThrdParams);

	int readerResult;
	int analyzerResult;
	int printerResult;
	thrd_join(readerThrd, &readerResult);
	thrd_join(analyzerThrd, &analyzerResult);
	thrd_join(printerThrd, &printerResult);

	free(usageInfoBuffer);
	CircularBuffer_destroy(procStatCbuf);
}