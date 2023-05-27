#include "threadctl.h"
#include <stdatomic.h>
#include <threads.h>
#include <stdbool.h>
#include "logger.h"


static volatile atomic_bool g_killSwitch = false;
static tss_t g_tssId;


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

		switch (result)
		{
			case 0:
			{
				Log(LLEVEL_DEBUG, "thread sleep successful");
			}
			break;

			case -1:
			{
				Log(LLEVEL_WARNING, "thread sleep interrupted");
			}
			break;

			default:
			{
				Log(LLEVEL_ERROR, "error encountered while attempting to sleep");
			}
			break;
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


const ThreadInfo_t* ThreadInfo_get(void)
{
	return tss_get(g_tssId);
}


bool ThreadInfo_init(void)
{
	if (thrd_success != tss_create(&g_tssId, NULL))
	{
		Log(LLEVEL_ERROR, "cannot create thread-specific storage");
		return false;
	}

	return true;
}


int ThreadInfo_set(ThreadInfo_t* tinfo)
{
	// 
	return tss_set(g_tssId, tinfo);
}