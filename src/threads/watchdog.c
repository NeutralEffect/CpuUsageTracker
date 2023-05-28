#include "watchdog.h"
#include "sync.h"
#include "logger.h"
#include "threadctl.h"
#include "helpers.h"
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define WATCHDOG_ALLOWED_UNRESPONSIVE_MS 	2000u
#define WATCHDOG_MUTEX_WAIT_TIME_MS 		50u
#define WATCHDOG_THREAD_ID					TID_WATCHDOG
#define WATCHDOG_THREAD_NAME				"Watchdog"		


static ThreadInfo_t g_watchdogThreadInfo =
{
	.tid 	= WATCHDOG_THREAD_ID,
	.name 	= WATCHDOG_THREAD_NAME
};

static const char* THREAD_NAMES[TID_COUNT_] =
{
	[TID_READER]	= "Reader",
	[TID_ANALYZER]	= "Analyzer",
	[TID_PRINTER]	= "Printer",
	[TID_LOGGER]	= "Logger",
	[TID_WATCHDOG]	= "Watchdog"
};

static volatile struct timespec g_timestamps[TID_COUNT_];
static mtx_t g_timestampsMtx;
static cnd_t g_timestampsUpdatedCv;


static inline int getCurrentTime(struct timespec* out)
{
	// Could use clock_gettime(CLOCK_REALTIME, out) instead.
	return timespec_get(out, TIME_UTC);
}


static struct timespec timespecAdd(struct timespec a, struct timespec b)
{
	unsigned secs = a.tv_sec + b.tv_sec;
	unsigned nsecs = a.tv_nsec + b.tv_nsec;

	if (nsecs > 1000000000)
	{
		nsecs -= 1000000000;
		++secs;
	}

	struct timespec result = { .tv_sec = secs, .tv_nsec = nsecs };
	return result;
}


static int timespecCompare(struct timespec a, struct timespec b)
{
	long long int retval = a.tv_sec - b.tv_sec;

	if (retval == 0)
	{
		retval = a.tv_nsec - b.tv_nsec;
	}

	// Return -1, 0 or 1 depending on retval sign.
	return (retval > 0) ? 1 : ((retval < 1) ? -1 : 0);
}


static inline int findLowestTimespecIndex(struct timespec array[], int arrayLen)
{
	int iiOldest = 0;
	
	for (int ii = 0; ii < arrayLen; ++ii)
	{
		if (timespecCompare(array[ii], array[iiOldest]) < 0)
		{
			iiOldest = ii;
		}
	}

	return iiOldest;
}


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


static struct timespec msToTimespec(unsigned long long ms)
{
	unsigned secs = ms / 1000;
	unsigned nsecs = (ms - secs * 1000) * 1000000;
	struct timespec result = { .tv_sec = secs, .tv_nsec = nsecs };
	return result;
}


static void triggerKillSwitch(void)
{
	if (0 != raise(SIGTERM))
	{
		Log(LLEVEL_FATAL, "unable to raise SIGTERM signal");
	}
}


void Watchdog_finalize(void)
{
	mtx_destroy(&g_timestampsMtx);
}


bool Watchdog_init(void)
{
	if (thrd_success != mtx_init(&g_timestampsMtx, mtx_timed))
	{
		// Nigh impossible for this error to arise
		Log(LLEVEL_FATAL, "cannot create mutex for watchdog module");
		return false;
	}

	if (thrd_success != cnd_init(&g_timestampsUpdatedCv))
	{
		// Nigh impossible for this error to arise
		Log(LLEVEL_FATAL, "cannot create condition variable for watchdog module");
		mtx_destroy(&g_timestampsMtx);
		return false;
	}

	return true;
}


void Watchdog_reportActive(void)
{
	static const unsigned MAX_LOCK_TIME_MS = 50u;

	const ThreadInfo_t* thrdInfo = ThreadInfo_get();

	if (NULL == thrdInfo)
	{
		return;
	}

	if (thrd_success == Mutex_tryLockMs(&g_timestampsMtx, MAX_LOCK_TIME_MS))
	{
		// We need to discard the volatile qualifier; it is safe to do so since we're mutex-locked.
		getCurrentTime((struct timespec*) &g_timestamps[thrdInfo->tid]);

		// It's possible for error to arise while unlocking mutex,
		// but handling it here is unproductive.
		if (thrd_success != Mutex_unlock(&g_timestampsMtx))
		{
			Log(LLEVEL_ERROR, "couldn't release timestamps mutex");
		}

		if (thrd_success != CondVar_notify(&g_timestampsUpdatedCv))
		{
			Log(LLEVEL_ERROR, "couldn't notify on timestamps condition variable");
		}
	}
	else
	{
		Log(LLEVEL_ERROR, "couldn't acquire timestamps mutex");
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

	// Initialize timeout before entering mutex lock
	const struct timespec allowedUnresponsiveTime = msToTimespec(WATCHDOG_ALLOWED_UNRESPONSIVE_MS);
	struct timespec timeoutPoint;
	getCurrentTime(&timeoutPoint);

	if (thrd_success != Mutex_tryLockMs(&g_timestampsMtx, WATCHDOG_MUTEX_WAIT_TIME_MS))
	{
		Log(LLEVEL_FATAL, "cannot acquire timestamps mutex");
		retval = -2;
		thrd_exit(retval);
	}

	// Initialize timestamps to make sure threads won't be falsely detected as unresponsive.
	for (unsigned ii = 0; ii < sizeof g_timestamps / sizeof *g_timestamps; ++ii)
	{
		g_timestamps[ii] = timeoutPoint;
	}

	// Currently timeoutPoint holds (almost) current time; offset it by allowed unresponsive time.
	timeoutPoint = timespecAdd(timeoutPoint, allowedUnresponsiveTime);

	while (false == Thread_getKillSwitchStatus())
	{
		int result = CondVar_waitUntil(&g_timestampsUpdatedCv, &g_timestampsMtx, &timeoutPoint);

		if (thrd_error == result)
		{
			Log(LLEVEL_ERROR, "error while waiting on tiemstamps condition variable");
			continue;
		}
		else if (thrd_timedout == result)
		{
			Log(LLEVEL_WARNING, "timeout while waiting on timestamps condition variable");
		}
		else
		{
			Log(LLEVEL_DEBUG, "received notification on timestamps condition variable");
		}

		struct timespec now;
		getCurrentTime(&now);
		// Ensure we won't detect Watchdog thread as unresponsive
		g_timestamps[TID_WATCHDOG] = now;

		int iiOldest = findLowestTimespecIndex((struct timespec*) g_timestamps, sizeof g_timestamps / sizeof *g_timestamps);

		// Check if difference between now and oldest timestamp exceeds maximum allowed unresponsive time
		if (timespecCompare(timespecDifference(now, g_timestamps[iiOldest]), allowedUnresponsiveTime) > 0)
		{
			triggerKillSwitch();
			Log(LLEVEL_FATAL, "thread \"%s\" unresponsive, terminating", THREAD_NAMES[iiOldest]);
			retval = iiOldest;
			break;
		}

		// Update time point for next check
		timeoutPoint = timespecAdd(g_timestamps[iiOldest], allowedUnresponsiveTime);
	}

	if (thrd_success != Mutex_unlock(&g_timestampsMtx))
	{
		Log(LLEVEL_ERROR, "couldn't release timestamps mutex");
		retval = -3;
		// Relying on thrd_exit immediately after this block could easily introduce a bug.
		thrd_exit(retval);
	}

	Log(LLEVEL_INFO, "thread exiting");

	thrd_exit(retval);
}
