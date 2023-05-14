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
#include "watchdog.h"
#include "logger.h"
#include "cpuusage.h"
#include "procstat.h"
#include "cpucount.h"
#include "logger.h"


int main()
{
	RegisterSigintHandler();
	RegisterSigtermHandler();
	CpuCount_init();
	Logger_init();
	Watchdog_init();
	Logger_setLogLevel(LLEVEL_TRACE);

	mtx_t procStatMtx;
	mtx_t usageInfoMtx;
	mtx_init(&procStatMtx, mtx_timed);
	mtx_init(&usageInfoMtx, mtx_timed);
	CircularBuffer_t* procStatCbuf = CircularBuffer_create(ProcStat_size(), 10u);
	CpuUsageInfo_t* usageInfoBuffer = calloc(1, CpuUsageInfo_size());
	
	thrd_t watchdogThrd;
	thrd_t loggerThrd;
	thrd_t readerThrd;
	thrd_t analyzerThrd;
	thrd_t printerThrd;

	thrd_create(&watchdogThrd, WatchdogThread, NULL);

	thrd_create(&loggerThrd, LoggerThread, NULL);

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

	int watchdogResult;
	int loggerResult;
	int readerResult;
	int analyzerResult;
	int printerResult;
	thrd_join(watchdogThrd, &watchdogResult);
	thrd_join(loggerThrd, &loggerResult);
	thrd_join(readerThrd, &readerResult);
	thrd_join(analyzerThrd, &analyzerResult);
	thrd_join(printerThrd, &printerResult);
	mtx_destroy(&usageInfoMtx);
	mtx_destroy(&procStatMtx);

	free(usageInfoBuffer);
	CircularBuffer_destroy(procStatCbuf);

	return 0;
}
