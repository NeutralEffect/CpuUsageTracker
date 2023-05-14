#ifndef WATCHDOG_H_INCLUDED
#define WATCHDOG_H_INCLUDED


typedef enum ThreadId
{
	TID_READER = 0,
	TID_ANALYZER,
	TID_PRINTER,
	TID_LOGGER,
	TID_WATCHDOG,
	TID_COUNT_
}
ThreadId_t;


/**
 * \brief Initializes watchdog module.
 * Has to be called before any function in this module is used.
*/
void Watchdog_init(void);


/**
 * \brief Reports this thread as active. Should be used on a regular basis within
 * given thread, as program uses those reports to detect unresponsive threads.
*/
void Watchdog_reportActive(ThreadId_t threadId);


int WatchdogThread(void* params);


#endif // !WATCHDOG_H_INCLUDED
