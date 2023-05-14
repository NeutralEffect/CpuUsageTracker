#include "thread_utils.h"
#include <threads.h>
#include <stdatomic.h>
#include "logger.h"


static volatile atomic_bool killSwitch = false;


static struct timespec durationMsToTimespecPoint(unsigned ms)
{
	struct timespec timePoint;
	timespec_get(&timePoint, TIME_UTC);
	unsigned long long seconds = ms / 1000;
	// Can be replaced with formula: (ms % 1000) * 1000
	unsigned long long nanoseconds = (ms - (seconds * 1000)) * 1000000;
	timePoint.tv_sec += seconds;
	timePoint.tv_nsec += nanoseconds;

	return timePoint;
}


int Mutex_tryLock(mtx_t* mutex, unsigned waitTimeS)
{
	return Mutex_tryLockMs(mutex, waitTimeS * 1000u);
}


int Mutex_tryLockMs(mtx_t* mutex, unsigned waitTimeMs)
{
	if (NULL == mutex)
	{
		return thrd_error;
	}

	struct timespec timePoint = durationMsToTimespecPoint(waitTimeMs);
	
	return mtx_timedlock(mutex, &timePoint);
}


int Mutex_unlock(mtx_t* mutex)
{
	return mtx_unlock(mutex);
}


void Thread_activateKillSwitch()
{
	killSwitch = true;
}


bool Thread_getKillSwitchStatus(void)
{
	return killSwitch;
}


void Thread_forceSleep(unsigned seconds)
{
	Thread_forceSleepMs(seconds * 1000u);
}


void Thread_forceSleepMs(unsigned milliseconds)
{
	time_t secs = milliseconds / 1000u;
	unsigned long long nsecs = (milliseconds - (secs * 1000)) * 1000000;
	struct timespec sleepTime = { .tv_sec = secs, .tv_nsec = nsecs };
	
	int result;

	do
	{
		result = thrd_sleep(&sleepTime, &sleepTime);

		if (-1 == result)
		{
			Log(LLEVEL_WARNING, "thread sleep interrupted");
		}
		else
		{
			Log(LLEVEL_ERROR, "error encountered while attempting to sleep");
		}

	} while (0 != result);
	
}


int Thread_sleep(unsigned seconds)
{
	return thrd_sleep(&(struct timespec) { .tv_sec = seconds, .tv_nsec = 0 }, NULL);
}


int Thread_sleepMs(unsigned milliseconds)
{
	time_t secs = milliseconds / 1000u;
	unsigned long long nsecs = (milliseconds - (secs * 1000)) * 1000000;
	return thrd_sleep(&(struct timespec) { .tv_sec = secs, .tv_nsec = nsecs }, NULL);
}