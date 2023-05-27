#include "reader.h"
#include "procstat.h"
#include "sync.h"
#include "logger.h"
#include "watchdog.h"
#include "helpers.h"
#include "threadctl.h"
#include <threads.h>
#include <stdatomic.h>


#define READER_MUTEX_WAIT_TIME_MS 		50
#define READER_CONDVAR_WAIT_TIME_MS 	2000
#define READER_SLEEP_TIME_MS			500
#define READER_THREAD_ID				TID_READER
#define READER_THREAD_NAME 				"Reader"


static ThreadInfo_t g_readerThreadInfo =
{
	.tid 	= READER_THREAD_ID,
	.name 	= READER_THREAD_NAME
};


int ReaderThread(void* rawParams)
{
	int retval = 0;

	if (NULL == rawParams)
	{
		retval = -1;
		thrd_exit(retval);
	}

	ReaderThreadParams_t* params = (ReaderThreadParams_t*) rawParams;

	if (thrd_success != ThreadInfo_set(&g_readerThreadInfo))
	{
		retval = -2;
		thrd_exit(retval);
	}

	// Only continue execution if kill switch hasn't been activated
	while (false == Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive();

		// Load procstat from file
		ProcStat_t* procStat = ProcStat_loadFromFile();

		if (NULL == procStat)
		{
			// Procstat is unavailable, log error and try again in next iteration
			Log(LLEVEL_ERROR, "reader: cannot load data from /proc/stat file");
			continue;
		}

		// Procstat has been acquired, lock mutex on circular buffer and write
		if (thrd_success != Mutex_tryLockMs(params->outMtx, READER_MUTEX_WAIT_TIME_MS))
		{
			Log(LLEVEL_WARNING, "reader: couldn't acquire buffer mutex");
			continue;
		}

		// Mutex acquired, wait for buffer to have space for data
		struct timespec timePoint = TimePointMs(READER_CONDVAR_WAIT_TIME_MS);

		while (CircularBuffer_isFull(params->outBuf) && (false == Thread_getKillSwitchStatus()))
		{
			int result = CondVar_waitUntil(params->outNotFullCv, params->outMtx, &timePoint);

			if (thrd_success == result)
			{
				Log(LLEVEL_DEBUG, "reader: received notification on condition variable");
				break;
			}
			else if (thrd_timedout == result)
			{
				Log(LLEVEL_WARNING, "reader: timeout while waiting on condition variable");
				break;
			}
			else
			{
				Log(LLEVEL_ERROR, "reader: error while waiting on condition variable");
			}

		}

		if (Thread_getKillSwitchStatus())
		{
			if (thrd_success != Mutex_unlock(params->outMtx))
			{
				Log(LLEVEL_ERROR, "reader: couldn't release buffer mutex");
			}
			break;
		}
		
		// Buffer isn't full, write data
		CircularBuffer_write(params->outBuf, procStat);
		
		// Unlock mutex so analyzer thread can access it
		if (thrd_success != Mutex_unlock(params->outMtx))
		{
			Log(LLEVEL_ERROR, "reader: couldn't release buffer mutex");
		}

		// Signal to analyzer that data is available
		if (thrd_success != CondVar_notify(params->outNotEmptyCv))
		{
			Log(LLEVEL_ERROR, "reader: couldn't notify on condition variable");
		}
		
		Log(LLEVEL_DEBUG, "reader: procstat data sent");
		Log(LLEVEL_TRACE, "reader: cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
			procStat->cpuStats[0].values[0],
			procStat->cpuStats[0].values[1],
			procStat->cpuStats[0].values[2],
			procStat->cpuStats[0].values[3],
			procStat->cpuStats[0].values[4],
			procStat->cpuStats[0].values[5],
			procStat->cpuStats[0].values[6],
			procStat->cpuStats[0].values[7],
			procStat->cpuStats[0].values[8],
			procStat->cpuStats[0].values[9]);

		// Remove obsolete procstat object and move on
		ProcStat_destroy(procStat);

		// Introduce delay here to reduce probing frequency.
		// Probing frequency higher than USER_HZ can cause issues.
		Thread_sleepMs(READER_SLEEP_TIME_MS);
	}

	Log(LLEVEL_INFO, "thread exiting");

	// Exit as usual
	thrd_exit(retval);
}