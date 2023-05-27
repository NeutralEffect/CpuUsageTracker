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
#include "threadctl.h"


#define PROCSTAT_CBUF_CAPACITY 10u
#define USAGEINFO_CBUF_CAPACITY 1u


int main()
{
	RegisterSigintHandler();
	RegisterSigtermHandler();
	CpuCount_init();
	ThreadInfo_init();
	Logger_init();
	Watchdog_init();

	Logger_setLogLevel(LLEVEL_DEBUG);

	mtx_t procStatMtx;
	mtx_t usageInfoMtx;
	mtx_init(&procStatMtx, mtx_timed);
	mtx_init(&usageInfoMtx, mtx_timed);
	cnd_t procStatNotEmptyCv;
	cnd_t procStatNotFullCv;
	cnd_t usageInfoNotEmptyCv;
	cnd_t usageInfoNotFullCv;
	cnd_init(&procStatNotEmptyCv);
	cnd_init(&procStatNotFullCv);
	cnd_init(&usageInfoNotEmptyCv);
	cnd_init(&usageInfoNotFullCv);

	CircularBuffer_t* procStatCbuf = CircularBuffer_create(ProcStat_size(), PROCSTAT_CBUF_CAPACITY);
	CircularBuffer_t* usageInfoCbuf = CircularBuffer_create(CpuUsageInfo_size(), USAGEINFO_CBUF_CAPACITY);
	
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
			.outMtx 		= &procStatMtx,
			.outNotEmptyCv 	= &procStatNotEmptyCv,
			.outNotFullCv 	= &procStatNotFullCv,
			.outBuf 		= procStatCbuf
		});

	thrd_create(
		&analyzerThrd, 
		AnalyzerThread,
		&(AnalyzerThreadParams_t)
		{
			.inMtx 			= &procStatMtx,
			.inNotEmptyCv 	= &procStatNotEmptyCv,
			.inNotFullCv 	= &procStatNotFullCv,
			.inBuf 			= procStatCbuf,
			.outMtx			= &usageInfoMtx,
			.outNotEmptyCv 	= &usageInfoNotEmptyCv,
			.outNotFullCv 	= &usageInfoNotFullCv,
			.outBuf			= usageInfoCbuf
		});

	thrd_create(
		&printerThrd,
		PrinterThread,
		&(PrinterThreadParams_t)
		{
			.inMtx 			= &usageInfoMtx,
			.inNotEmptyCv 	= &usageInfoNotEmptyCv,
			.inNotFullCv 	= &usageInfoNotFullCv,
			.inBuf 			= usageInfoCbuf
		});

	int watchdogResult;
	int loggerResult;
	int readerResult;
	int analyzerResult;
	int printerResult;
	
	thrd_join(printerThrd, &printerResult);
	thrd_join(analyzerThrd, &analyzerResult);
	thrd_join(readerThrd, &readerResult);
	thrd_join(loggerThrd, &loggerResult);
	thrd_join(watchdogThrd, &watchdogResult);

	cnd_destroy(&usageInfoNotFullCv);
	cnd_destroy(&usageInfoNotEmptyCv);
	cnd_destroy(&procStatNotFullCv);
	cnd_destroy(&usageInfoNotEmptyCv);
	mtx_destroy(&usageInfoMtx);
	mtx_destroy(&procStatMtx);

	CircularBuffer_destroy(usageInfoCbuf);
	CircularBuffer_destroy(procStatCbuf);

	system("clear");
	printf("%-10s = %i\n", "Reader", readerResult);
	printf("%-10s = %i\n", "Analyzer", analyzerResult);
	printf("%-10s = %i\n", "Printer", printerResult);
	printf("%-10s = %i\n", "Logger", loggerResult);
	printf("%-10s = %i\n", "Watchdog", watchdogResult);

	return 0;
}
