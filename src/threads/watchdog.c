#include "watchdog.h"
#include "sync.h"
#include "logger.h"
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_UNRESPONSIVE_TIME_SECONDS 2u
#define SLEEP_TIME_MILLISECONDS 500u
#define MUTEX_WAIT_TIME_MS 50u


static volatile struct timespec g_activityReports[TID_COUNT_];
static mtx_t g_activityReportsMutex;


static struct timespec timespecDifference(struct timespec a, struct timespec b)
{
	const unsigned long long NSEC_IN_SEC = 1000000000u;

	struct timespec result;

	result.tv_sec = a.tv_sec - b.tv_sec;

	// If b nsecs are higher than a's, we need to perform carryover
	if (a.tv_nsec <= b.tv_nsec)
	{
		result.tv_nsec = NSEC_IN_SEC - (b.tv_nsec - a.tv_nsec);
		--result.tv_sec;
	}
	else
	{
		result.tv_nsec = a.tv_nsec - b.tv_nsec;
	}

	return result;
}


static void triggerKillswitch(void)
{
	if (0 != raise(SIGTERM))
	{
		Log(LLEVEL_FATAL, "unable to raise SIGTERM signal");
	}
}


void Watchdog_init(void)
{
	if (thrd_success != mtx_init(&g_activityReportsMutex, mtx_timed))
	{
		// Highly unlikely for this error to arise
		Log(LLEVEL_FATAL, "cannot create mutex for watchdog module");
	}
}


void Watchdog_reportActive(ThreadId_t threadIndex)
{
	static const unsigned MAX_LOCK_TIME_MS = 50u;

	if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, MAX_LOCK_TIME_MS))
	{
		// We need to discard the volatile qualifier; it is safe to do so since we're mutex-locked.
		clock_gettime(CLOCK_REALTIME, (struct timespec*) &g_activityReports[threadIndex]);

		// It's possible for error to arise while unlocking mutex,
		// but handling it here is unproductive.
		Mutex_unlock(&g_activityReportsMutex);
	}
	else
	{
		Log(LLEVEL_ERROR, "cannot report activity");
	}
}


int WatchdogThread(void* rawParams)
{
	// Suppress unused parameter warning
	(void) rawParams;

	static const char* THREAD_NAMES[TID_COUNT_] =
	{
		[TID_READER]	= "Reader",
		[TID_ANALYZER]	= "Analyzer",
		[TID_PRINTER]	= "Printer",
		[TID_LOGGER]	= "Logger",
		[TID_WATCHDOG]	= "Watchdog"
	};

	// Initialize activity reports with current time value
	// to prevent them from being instantly recognized as unresponsive
	if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, MUTEX_WAIT_TIME_MS))
	{
		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		
		for (int ii = 0; ii < TID_COUNT_; ++ii)
		{
			g_activityReports[ii] = now;
		}
		
		if (thrd_success != Mutex_unlock(&g_activityReportsMutex))
		{
			Log(LLEVEL_ERROR, "watchdog: cannot release mutex");
		}
	}

	while (!Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive(TID_WATCHDOG);
		
		struct timespec reportsCopy[TID_COUNT_];

		if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, MUTEX_WAIT_TIME_MS))
		{
			// Discard volatile on g_activityReports since we're mutex-locked.
			memcpy(reportsCopy, (void*) g_activityReports, sizeof(g_activityReports));
			
			if (thrd_success != Mutex_unlock(&g_activityReportsMutex))
			{
				Log(LLEVEL_ERROR, "watchdog: cannot release mutex");
			}
		}

		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		int unresponsiveThreads = 0;

		for (int ii = 0; ii < TID_COUNT_; ++ii)
		{
			struct timespec diff = timespecDifference(now, reportsCopy[ii]);
			if (diff.tv_sec > MAX_UNRESPONSIVE_TIME_SECONDS)
			{
				++unresponsiveThreads;
				Log(LLEVEL_FATAL, "watchdog: thread \"%s\" is unresponsive", THREAD_NAMES[ii]);
			}
		}

		if (unresponsiveThreads > 0)
		{
			Log(LLEVEL_FATAL, "watchdog: terminating program due to unresponsive threads");
			system("clear");
			printf("Unresponsive threads, terminating program...\n");
			triggerKillswitch();
			break;
		}

		Thread_sleepMs(SLEEP_TIME_MILLISECONDS);
	}

	thrd_exit(0);
}