#include "analyzer.h"
#include "thread_utils.h"
#include "procstat.h"
#include "stdlib.h"
#include "cpuusage.h"
#include "log.h"
#include <threads.h>


#define SLEEP_TIME_SECONDS 1
#define SLEEP_TIME_MILLISECONDS (SLEEP_TIME_SECONDS * 1000)
#define MUTEX_WAIT_TIME_MS 50


/**
 * \brief Repeatedly attempt to read from given circular buffer
 * until either data is read or thread has been interrupted.
 * \param mutex Mutex to lock on before reading from circular buffer.
 * \param cbuf Circular buffer to read data from.
 * \param output Buffer to write resulting data into.
 * \return 0 if successful, negative value on error.
*/
static int waitForData(mtx_t* mutex, CircularBuffer_t* cbuf, void* output)
{
	int result = 0;

	while (true) 
	{
		const int mtxstatus = Mutex_tryLockMs(mutex, MUTEX_WAIT_TIME_MS);

		if (thrd_success == mtxstatus)
		{
			bool dataReceived = CircularBuffer_read(cbuf, output);
			
			if (thrd_success != Mutex_unlock(mutex))
			{
				result = -1;
				break;
			}

			if (dataReceived)
			{
				break;
			}
		}
		else if (thrd_error == mtxstatus)
		{
			Log(LLEVEL_ERROR, "cannot acquire mutex");
			result = -1;
			break;
		}

		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	return result;
}


/**
 * \brief Attempts to safely read one item from given circular buffer,
 * waiting no longer than specified amount of time for mutex to be locked.
 * \param mutex Mutex to lock on while reading from circular buffer.
 * \param cbuf Circular buffer to read item from.
 * \param output Output buffer for item to be saved into.
 * \param timeoutMs Maximum amount of time to be spent waiting for mutex to unlock, in milliseconds.
 * \return 0 if successful, 1 in case of timeout, negative value on error.
*/
static int readDataTimeout(mtx_t* mutex, CircularBuffer_t* cbuf, void* output, unsigned timeoutMs)
{
	const int mtxlockres = Mutex_tryLockMs(mutex, timeoutMs);
	int result = 0;

	if (thrd_success != mtxlockres)
	{
		result = (thrd_timedout == mtxlockres) ? -1 : -2;
		// Since we didn't obrain mutex, we cannot proceed with reading from buffer
		return result;
	}

	if (!CircularBuffer_read(cbuf, output))
	{
		result = 1;
	}

	if (thrd_success != Mutex_unlock(mutex))
	{
		result = -4;
	}

	return result;
}


int AnalyzerThread(void* rawParams)
{
	int returnCode = 0;

	if (NULL == rawParams)
	{
		returnCode = -1;
		goto error_exit_1;
	}

	AnalyzerThreadParams_t* params = (AnalyzerThreadParams_t*) rawParams;
	ProcStat_t* oldStatBuffer = malloc(ProcStat_size());
	ProcStat_t* newStatBuffer = malloc(ProcStat_size());
	CpuUsageInfo_t* const usageInfoBuffer = malloc(CpuUsageInfo_size());

	if (NULL == oldStatBuffer)
	{
		returnCode = -2;
		goto error_exit_1;
	}

	if (NULL == newStatBuffer)
	{
		returnCode = -2;
		goto error_exit_2;
	}

	if (NULL == usageInfoBuffer)
	{
		returnCode = -2;
		goto error_exit_3;
	}

	while (
		(0 != waitForData(params->inputMutex, params->inputBuffer, oldStatBuffer)) &&
		(false == Thread_getKillSwitchStatus()))
		;

	Log(LLEVEL_DEBUG, "analyzer: procstat data received");
	Log(LLEVEL_TRACE, "analyzer: cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
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

	// Main loop
	while (false == Thread_getKillSwitchStatus())
	{
		const int dataReadStatus = readDataTimeout(params->inputMutex, params->inputBuffer, newStatBuffer, MUTEX_WAIT_TIME_MS);

		switch (dataReadStatus)
		{
			case 0:
			{
				CpuUsageInfo_calculate(oldStatBuffer, newStatBuffer, usageInfoBuffer);

				if (thrd_success == Mutex_tryLockMs(params->outputMutex, MUTEX_WAIT_TIME_MS))
				{
					*params->outputBuffer = *usageInfoBuffer;
					Mutex_unlock(params->outputMutex);
					Log(LLEVEL_DEBUG, "analyzer: usage stats sent");
				}

				Log(LLEVEL_TRACE, "analyzer old: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
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

				Log(LLEVEL_TRACE, "analyzer new: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
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

				// Swap buffer pointers, so that in next iteration fresh data will overwrite oldest data
				ProcStat_t* tmp = oldStatBuffer;
				oldStatBuffer = newStatBuffer;
				newStatBuffer = tmp;
			}
			break;

			case 1:
			{
				Log(LLEVEL_WARNING, "analyzer: procstat data unavailable");
			}
			break;

			default:
			{
				Log(LLEVEL_ERROR, "analyzer: error while attempting to receive procstat data");
			}
			break;
		}

		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	free(newStatBuffer);
	free(oldStatBuffer);
	free(usageInfoBuffer);
	// Exit as usual
	thrd_exit(returnCode);

error_exit_3:
	free(newStatBuffer);
error_exit_2:
	free(oldStatBuffer);
error_exit_1:
	thrd_exit(returnCode);
}