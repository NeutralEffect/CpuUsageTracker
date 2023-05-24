#include <stdio.h>
#include <threads.h>
#include <time.h>
#include <stdlib.h>
#include "sync.h"
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

	thrd_create(
		&watchdogThrd,
		WatchdogThread,
		NULL);

	thrd_create(
		&loggerThrd,
		LoggerThread,
		NULL);

	thrd_create(
		&readerThrd,
		ReaderThread,
		&(ReaderThreadParams_t)
		{
			.outMtx = &procStatMtx,
			.outBuf = procStatCbuf
		});

	thrd_create(
		&analyzerThrd, 
		AnalyzerThread,
		&(AnalyzerThreadParams_t)
		{
			.inMtx 	= &procStatMtx,
			.inBuf 	= procStatCbuf,
			.outMtx	= &usageInfoMtx,
			.outBuf	= usageInfoBuffer
		});

	thrd_create(
		&printerThrd,
		PrinterThread,
		&(PrinterThreadParams_t)
		{
			.inMtx 	= &usageInfoMtx,
			.inBuf = usageInfoBuffer
		});

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
