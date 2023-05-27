#include "analyzer.h"
#include "sync.h"
#include "procstat.h"
#include "circbuf.h"
#include "cpuusage.h"
#include "logger.h"
#include "helpers.h"
#include "watchdog.h"
#include "threadctl.h"
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <stdbool.h>


#define ANALYZER_MUTEX_WAIT_TIME_MS 	50
#define ANALYZER_CONDVAR_WAIT_TIME_MS 	2000
#define ANALYZER_THREAD_ID				TID_ANALYZER
#define ANALYZER_THREAD_NAME			"Analyzer"


static ThreadInfo_t g_analyzerThreadInfo =
{
	.tid 	= ANALYZER_THREAD_ID,
	.name 	= ANALYZER_THREAD_NAME
};


int AnalyzerThread(void* rawParams)
{
	int retval = 0;

	if (NULL == rawParams)
	{
		retval = -1;
		goto error_exit_1;
	}

	if (thrd_success != ThreadInfo_set(&g_analyzerThreadInfo))
	{
		retval = -2;
		goto error_exit_1;
	}

	AnalyzerThreadParams_t* const params = (AnalyzerThreadParams_t*) rawParams;
	ProcStat_t* oldStatBuffer = malloc(ProcStat_size());
	ProcStat_t* newStatBuffer = malloc(ProcStat_size());
	CpuUsageInfo_t* const usageInfoBuffer = malloc(CpuUsageInfo_size());
	bool oldStatBufferInitialized = false;

	if (NULL == oldStatBuffer)
	{
		retval = -3;
		goto error_exit_1;
	}

	if (NULL == newStatBuffer)
	{
		retval = -3;
		goto error_exit_2;
	}

	if (NULL == usageInfoBuffer)
	{
		retval = -3;
		goto error_exit_3;
	}

	// Main loop
	while (false == Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive();

		// Acquire mutex to receive data from reader thread
		if (thrd_success != Mutex_tryLockMs(params->inMtx, ANALYZER_MUTEX_WAIT_TIME_MS))
		{
			Log(LLEVEL_WARNING, "couldn't acquire input buffer mutex");
			continue;
		}

		struct timespec timePoint = TimePointMs(ANALYZER_CONDVAR_WAIT_TIME_MS);
		while ( CircularBuffer_isEmpty(params->inBuf) && (false == Thread_getKillSwitchStatus()) )
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

		if (!CircularBuffer_read(params->inBuf, newStatBuffer))
		{
			Log(LLEVEL_ERROR, "attempted read from empty buffer");
			
			if (thrd_success != Mutex_unlock(params->inMtx))
			{
				Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
			}

			continue;
		}

		if (thrd_success != Mutex_unlock(params->inMtx))
		{
			Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
			continue;
		}

		if (thrd_success != CondVar_notify(params->inNotFullCv))
		{
			Log(LLEVEL_ERROR, "couldn't notify on input condition variable");
		}

		if (!oldStatBufferInitialized)
		{
			ProcStat_t* tmp 			= oldStatBuffer;
			oldStatBuffer 				= newStatBuffer;
			newStatBuffer 				= tmp;
			oldStatBufferInitialized 	= true;
			continue;
		}

		CpuUsageInfo_calculate(oldStatBuffer, newStatBuffer, usageInfoBuffer);

		if (thrd_success != Mutex_tryLockMs(params->outMtx, ANALYZER_MUTEX_WAIT_TIME_MS))
		{
			Log(LLEVEL_WARNING, "couldn't acquire output buffer mutex");
			continue;
		}

		// Wait and write to output buffer
		timePoint = TimePointMs(ANALYZER_CONDVAR_WAIT_TIME_MS);
		while (CircularBuffer_isFull(params->outBuf) && (false == Thread_getKillSwitchStatus()))
		{
			int result = CondVar_waitUntil(params->outNotFullCv, params->outMtx, &timePoint);

			if (thrd_success == result)
			{
				Log(LLEVEL_DEBUG, "received notification on output condition variable");
				break;
			}
			else if (thrd_timedout == result)
			{
				Log(LLEVEL_WARNING, "timeout while waiting on output condition variable");
				break;
			}
			else
			{
				Log(LLEVEL_ERROR, "error while waiting on output condition variable");
			}
			
		}

		if (Thread_getKillSwitchStatus())
		{
			if (thrd_success != Mutex_unlock(params->outMtx))
			{
				Log(LLEVEL_ERROR, "couldn't release output buffer mutex");
			}
			break;
		}

		CircularBuffer_write(params->outBuf, usageInfoBuffer);

		if (thrd_success != Mutex_unlock(params->outMtx))
		{
			Log(LLEVEL_ERROR, "couldn't release output buffer mutex");
			continue;
		}

		if (thrd_success != CondVar_notify(params->outNotEmptyCv))
		{
			Log(LLEVEL_ERROR, "couldn't notify on output condition variable");
		}
		else
		{
			Log(LLEVEL_DEBUG, "notification sent on output condition variable");
		}

		Log(LLEVEL_TRACE, "old data: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
			oldStatBuffer->cpuStats[0].values[0],
			oldStatBuffer->cpuStats[0].values[1],
			oldStatBuffer->cpuStats[0].values[2],
			oldStatBuffer->cpuStats[0].values[3],
			oldStatBuffer->cpuStats[0].values[4],
			oldStatBuffer->cpuStats[0].values[5],
			oldStatBuffer->cpuStats[0].values[6],
			oldStatBuffer->cpuStats[0].values[7],
			oldStatBuffer->cpuStats[0].values[8],
			oldStatBuffer->cpuStats[0].values[9]);

		Log(LLEVEL_TRACE, "new data: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
			newStatBuffer->cpuStats[0].values[0],
			newStatBuffer->cpuStats[0].values[1],
			newStatBuffer->cpuStats[0].values[2],
			newStatBuffer->cpuStats[0].values[3],
			newStatBuffer->cpuStats[0].values[4],
			newStatBuffer->cpuStats[0].values[5],
			newStatBuffer->cpuStats[0].values[6],
			newStatBuffer->cpuStats[0].values[7],
			newStatBuffer->cpuStats[0].values[8],
			newStatBuffer->cpuStats[0].values[9]);

		Log(LLEVEL_TRACE, "result: %.2f", usageInfoBuffer->values[0]);

		// Swap local incoming data buffer pointers so that in the next iteration, old data is overwritten
		ProcStat_t* tmp = oldStatBuffer;
		oldStatBuffer 	= newStatBuffer;
		newStatBuffer 	= tmp;
	}

	Log(LLEVEL_INFO, "thread exiting");

	free(newStatBuffer);
	free(oldStatBuffer);
	free(usageInfoBuffer);
	
	// Exit as usual
	thrd_exit(retval);

error_exit_3:
	free(newStatBuffer);
error_exit_2:
	free(oldStatBuffer);
error_exit_1:
	thrd_exit(retval);
}
