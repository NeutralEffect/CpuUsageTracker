#ifndef WATCHDOG_H_INCLUDED
#define WATCHDOG_H_INCLUDED


/**
 * Thread identifiers used for activity reporting.
*/
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
 * \param threadId Identifier of reporting thread.
*/
void Watchdog_reportActive(ThreadId_t threadId);


/**
 * \brief Thread function for detecting unresponsive threads and terminating program.
 * \details Thread will periodically check activity reports supplied by other threads
 * and compare their timing against predefined maximum allowed unresponsiveness period.
 * If one or more of these reports are delayed beyond allowed period, this thread
 * will activate kill switch requesting all threads to immediately terminate.
 * \param params Ignored.
*/
int WatchdogThread(void* params);


#endif // !WATCHDOG_H_INCLUDED
