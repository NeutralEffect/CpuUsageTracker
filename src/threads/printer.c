#include "printer.h"
#include "sync.h"
#include "cpuusage.h"
#include "logger.h"
#include "watchdog.h"
#include "circbuf.h"
#include "helpers.h"
#include "threadctl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define PRINTER_CONDVAR_WAIT_TIME_MS 	2000
#define PRINTER_MUTEX_WAIT_TIME_MS 		50
#define PERCENTAGE_VALUE_FORMAT 		"%.2f"
#define PRINTER_THREAD_ID 				TID_PRINTER
#define PRINTER_THREAD_NAME 			"Printer"


static ThreadInfo_t g_printerThreadInfo =
{
	.tid 	= PRINTER_THREAD_ID,
	.name 	= PRINTER_THREAD_NAME
};


static void printFormattedCpuUsage(const CpuUsageInfo_t* cuinfo)
{
	if ((NULL == cuinfo) || (cuinfo->valuesLength < 1))
	{
		return;
	}

	printf("CPU:\t" PERCENTAGE_VALUE_FORMAT " %%\n", cuinfo->values[0]);

	for (unsigned ii = 1; ii < (unsigned long long) cuinfo->valuesLength; ++ii)
	{
		printf("CPU%d:\t" PERCENTAGE_VALUE_FORMAT " %%\n",
			ii - 1,
			cuinfo->values[ii]);
	}
}


int PrinterThread(void* rawParams)
{
	int retval = 0;

	if (NULL == rawParams)
	{
		retval = -1;
		goto error_exit_1;
	}

	if (thrd_success != ThreadInfo_set(&g_printerThreadInfo))
	{
		retval = -2;
		goto error_exit_1;
	}

	PrinterThreadParams_t* params = (PrinterThreadParams_t*) rawParams;

	CpuUsageInfo_t* usageInfoBuffer = malloc(CpuUsageInfo_size());

	if (NULL == usageInfoBuffer)
	{
		retval = -3;
		goto error_exit_1;
	}

	while (false == Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive();

		if (thrd_success != Mutex_tryLockMs(params->inMtx, PRINTER_MUTEX_WAIT_TIME_MS))
		{
			Log(LLEVEL_WARNING, "couldn't acquire input buffer mutex");
			continue;
		}

		struct timespec timePoint = TimePointMs(PRINTER_CONDVAR_WAIT_TIME_MS);
		while (CircularBuffer_isEmpty(params->inBuf) && (false == Thread_getKillSwitchStatus()))
		{
			int result = CondVar_waitUntil(params->inNotEmptyCv, params->inMtx, &timePoint);

			if (thrd_success == result)
			{
				Log(LLEVEL_DEBUG, "received notification on input condition variable");
				break;
			}
			else if (thrd_timedout == result)
			{
				Log(LLEVEL_WARNING, "timeout while waiting on input condition variable");
				break;
			}
			else
			{
				Log(LLEVEL_ERROR, "error while waiting on input condition variable");
			}
		}

		if (Thread_getKillSwitchStatus())
		{
			if (thrd_success != Mutex_unlock(params->inMtx))
			{
				Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
			}
			break;
		}

		if (!CircularBuffer_read(params->inBuf, usageInfoBuffer))
		{
			Log(LLEVEL_ERROR, "attempted read from empty buffer");
		}

		if (thrd_success != Mutex_unlock(params->inMtx))
		{
			Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
			continue;
		}

		system("clear");
		printFormattedCpuUsage(usageInfoBuffer);
		Log(LLEVEL_TRACE, "usage statistics printed to standard output");
	}

	Log(LLEVEL_INFO, "thread exiting");

	free(usageInfoBuffer);
	thrd_exit(retval);

error_exit_1:
	thrd_exit(retval);
}
