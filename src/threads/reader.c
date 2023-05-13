#include "reader.h"
#include <threads.h>
#include <stdatomic.h>
#include "procstat.h"
#include "thread_utils.h"
#include "log.h"


#define SLEEP_TIME_SECONDS 1
#define MUTEX_WAIT_TIME_MS 50


int ReaderThread(void* rawParams)
{
	if (NULL == rawParams)
	{
		thrd_exit(-1);
	}

	ReaderThreadParams_t* params = (ReaderThreadParams_t*) rawParams;

	// Only continue execution if kill switch hasn't been activated
	while (false == Thread_getKillSwitchStatus())
	{
		// Load procstat from file
		ProcStat_t* procStat = ProcStat_loadFromFile();

		if (NULL == procStat)
		{
			// Procstat is unavailable, log error and try again in next iteration
			Log(LLEVEL_ERROR, "reader: cannot load data from /proc/stat file");
		}
		else
		{
			// Procstat has been acquired, lock mutex on circular buffer and write
			if (thrd_success == Mutex_tryLockMs(params->mutex, MUTEX_WAIT_TIME_MS))
			{
				// Mutex acquired, write procstat
				CircularBuffer_write(params->buffer, procStat);
				// Unlock mutex so analyzer thread can access it
				Mutex_unlock(params->mutex);
				
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
			}
			else
			{
				Log(LLEVEL_WARNING, "reader: couldn't acquire buffer mutex");
			}

			// Remove obsolete procstat object and move on
			ProcStat_destroy(procStat);	
		}

		// Wait for next iteration
		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	// Exit as usual
	thrd_exit(0);
}