#include "logger.h"
#include "watchdog.h"
#include "thread_utils.h"
#include <threads.h>


#define SLEEP_TIME_SECONDS 1u


int LoggerThread(void* rawParams)
{
	(void) rawParams;

	while (!Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive(TID_LOGGER);
		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	thrd_exit(0);
}