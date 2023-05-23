#include "printer.h"
#include "sync.h"
#include "cpuusage.h"
#include "logger.h"
#include "watchdog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define SLEEP_TIME_SECONDS 1
#define MUTEX_WAIT_TIME_MS 50
#define PERCENTAGE_VALUE_FORMAT "%.2f"


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
	int returnCode = 0;
	PrinterThreadParams_t* params = (PrinterThreadParams_t*) rawParams;

	if (NULL == rawParams)
	{
		returnCode = -1;
		goto error_exit_1;
	}

	CpuUsageInfo_t* usageInfoBuffer = malloc(CpuUsageInfo_size());
	usageInfoBuffer->values[0] = 299.0; // FIXME

	if (NULL == usageInfoBuffer)
	{
		returnCode = -2;
		goto error_exit_1;
	}

	while (false == Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive(TID_PRINTER);

		int mtxstatus = Mutex_tryLockMs(params->mutex, MUTEX_WAIT_TIME_MS);

		switch (mtxstatus)
		{
			case thrd_success:
			{
				bool newValuesRead = false;
				// In order to prevent reading the same information multiple times,
				// we overwrite valuesLength to indicate that it is no longer valid.
				// Since only this code will ever read from that buffer
				// and it will never be assigned non-positive value outside of this block
				// it's a safe way of ensuring that we don't consume the same values more than once.
				// However, the structure HAS TO be initialized before going into use.
				if (params->buffer->valuesLength != 0)
				{
					memcpy(usageInfoBuffer, params->buffer, CpuUsageInfo_size());
					params->buffer->valuesLength = 0;
					newValuesRead = true;
				}

				if (thrd_success != Mutex_unlock(params->mutex))
				{
					Log(LLEVEL_ERROR, "printer: error while releasing mutex");
				}

				if (!newValuesRead)
				{
					break;
				}

				system("clear");
				printFormattedCpuUsage(usageInfoBuffer);
			}
			break;

			case thrd_timedout:
			{
				Log(LLEVEL_WARNING, "printer: timeout while attempting to lock on mutex");
			}
			break;

			default:
			{
				Log(LLEVEL_ERROR, "printer: error while attempting to lock on mutex");
			}
			break;
		}

		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	free(usageInfoBuffer);
	thrd_exit(returnCode);

error_exit_1:
	thrd_exit(returnCode);
}