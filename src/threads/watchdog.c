#include "watchdog.h"
#include "sync.h"
#include "logger.h"
#include "threadctl.h"
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define WATCHDOG_ALLOWED_UNRESPONSIVE_SECONDS 	2u
#define WATCHDOG_MUTEX_WAIT_TIME_MS 			50u
#define WATCHDOG_SLEEP_TIME_MILLISECONDS		1000
#define WATCHDOG_THREAD_ID						TID_WATCHDOG
#define WATCHDOG_THREAD_NAME					"Watchdog"


static ThreadInfo_t g_watchdogThreadInfo =
{
	.tid 	= WATCHDOG_THREAD_ID,
	.name 	= WATCHDOG_THREAD_NAME
};


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


void Watchdog_finalize(void)
{
	mtx_destroy(&g_activityReportsMutex);
}


void Watchdog_init(void)
{
	if (thrd_success != mtx_init(&g_activityReportsMutex, mtx_timed))
	{
		// Highly unlikely for this error to arise
		Log(LLEVEL_FATAL, "cannot create mutex for watchdog module");
	}
}


void Watchdog_reportActive(void)
{
	static const unsigned MAX_LOCK_TIME_MS = 50u;

	const ThreadInfo_t* thrdInfo = ThreadInfo_get();

	if (NULL == thrdInfo)
	{
		return;
	}

	if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, MAX_LOCK_TIME_MS))
	{
		// We need to discard the volatile qualifier; it is safe to do so since we're mutex-locked.
		clock_gettime(CLOCK_REALTIME, (struct timespec*) &g_activityReports[thrdInfo->tid]);

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

	int retval = 0;

	if (thrd_success != ThreadInfo_set(&g_watchdogThreadInfo))
	{
		retval = -1;
		thrd_exit(retval);
	}

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
	if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, WATCHDOG_MUTEX_WAIT_TIME_MS))
	{
		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		
		for (int ii = 0; ii < TID_COUNT_; ++ii)
		{
			g_activityReports[ii] = now;
		}
		
		if (thrd_success != Mutex_unlock(&g_activityReportsMutex))
		{
			Log(LLEVEL_ERROR, "cannot release mutex");
		}
	}

	while (!Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive();
		
		struct timespec reportsCopy[TID_COUNT_];

		if (thrd_success == Mutex_tryLockMs(&g_activityReportsMutex, WATCHDOG_MUTEX_WAIT_TIME_MS))
		{
			// Discard volatile on g_activityReports since we're mutex-locked.
			memcpy(reportsCopy, (void*) g_activityReports, sizeof(g_activityReports));
			
			if (thrd_success != Mutex_unlock(&g_activityReportsMutex))
			{
				Log(LLEVEL_ERROR, "cannot release mutex");
			}
		}

		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);

		int unresponsiveThreads = 0;

		for (int ii = 0; ii < TID_COUNT_; ++ii)
		{
			struct timespec diff = timespecDifference(now, reportsCopy[ii]);
			if (diff.tv_sec > WATCHDOG_ALLOWED_UNRESPONSIVE_SECONDS)
			{
				++unresponsiveThreads;
				Log(LLEVEL_FATAL, "thread \"%s\" is unresponsive", THREAD_NAMES[ii]);
			}
		}

		if (unresponsiveThreads > 0)
		{
			Log(LLEVEL_FATAL, "terminating program due to unresponsive threads");
			system("clear");
			printf("Unresponsive threads, terminating program...\n");
			triggerKillswitch();
			break;
		}

		Thread_sleepMs(WATCHDOG_SLEEP_TIME_MILLISECONDS);
	}

	Log(LLEVEL_INFO, "thread exiting");

	thrd_exit(retval);
}