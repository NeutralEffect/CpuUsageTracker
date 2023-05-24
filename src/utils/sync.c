#include "sync.h"
#include <threads.h>
#include <stdatomic.h>
#include "logger.h"


static volatile atomic_bool g_killSwitch = false;


/**
 * \brief Converts duration in milliseconds into time point relative to current time.
 * \param ms Milliseconds to convert.
 * \return Time point when given amount of milliseconds from now on will pass.
*/
static struct timespec durationMsToTimespecPoint(unsigned ms)
{
	struct timespec timePoint;
	timespec_get(&timePoint, TIME_UTC);
	unsigned long long seconds = ms / 1000;
	// Can be replaced with formula: (ms % 1000) * 1000000
	unsigned long long nanoseconds = (ms - (seconds * 1000)) * 1000000;
	timePoint.tv_sec += seconds;
	timePoint.tv_nsec += nanoseconds;

	return timePoint;
}


int CondVar_notify(CondVarHandle_t cv)
{
	return cnd_signal(cv);
}


int CondVar_notifyAll(CondVarHandle_t cv)
{
	return cnd_broadcast(cv);
}


int CondVar_waitMs(CondVarHandle_t cv, MutexHandle_t mtx, unsigned ms)
{
	if ( (NULL == cv) || (NULL == mtx) )
	{
		return thrd_error;
	}

	const struct timespec timePoint = durationMsToTimespecPoint(ms);

	return cnd_timedwait(cv, mtx, &timePoint);
}


int Mutex_tryLock(MutexHandle_t mutex, unsigned waitTimeS)
{
	return Mutex_tryLockMs(mutex, waitTimeS * 1000u);
}


int Mutex_tryLockMs(MutexHandle_t mutex, unsigned waitTimeMs)
{
	if (NULL == mutex)
	{
		return thrd_error;
	}

	struct timespec timePoint = durationMsToTimespecPoint(waitTimeMs);
	
	return mtx_timedlock(mutex, &timePoint);
}


int Mutex_unlock(MutexHandle_t mutex)
{
	return mtx_unlock(mutex);
}


void Thread_activateKillSwitch()
{
	g_killSwitch = true;
}


bool Thread_getKillSwitchStatus(void)
{
	return g_killSwitch;
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