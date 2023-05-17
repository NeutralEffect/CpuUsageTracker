#include "thread_utils.h"
#include <threads.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>


#define SLEEP_TIME_MS 5000


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


static bool timespecHigher(const struct timespec a, const struct timespec b)
{
	if (a.tv_sec == b.tv_sec)
	{
		return a.tv_nsec > b.tv_nsec;
	}

	return a.tv_sec > b.tv_sec;
}


struct timespec timespecFromMilliseconds(unsigned long long milliseconds)
{
	unsigned long long secs = milliseconds / 1000;
	unsigned long long nsecs = (milliseconds - (1000 * secs)) * 1000000;

	struct timespec result = 
	{
		.tv_sec = secs,
		.tv_nsec = nsecs
	};

	return result;
}


void Thread_sleepMs_test(void)
{
	struct timespec prev;
	struct timespec now;
	assert(0 == clock_gettime(CLOCK_REALTIME, &prev));
	assert(0 == Thread_sleepMs(SLEEP_TIME_MS));
	assert(0 == clock_gettime(CLOCK_REALTIME, &now));
	struct timespec diff = timespecDifference(now, prev);
	struct timespec expected = timespecFromMilliseconds(SLEEP_TIME_MS);
	assert(true == timespecHigher(diff, expected));
}


int main()
{
	Thread_sleepMs_test();
}
