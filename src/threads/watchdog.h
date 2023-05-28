/**
 * \file watchdog.h
 * Watchdog thread interface, used also for activity reporting.
*/
#ifndef WATCHDOG_H_INCLUDED
#define WATCHDOG_H_INCLUDED
#include <stdbool.h>


/**
 * \brief Finalizes watchdog module, cleaning up any resources used by it.
 * Should only be called after successful call to Watchdog_init() and only when watchdog module is no longer in use.
*/
void Watchdog_finalize(void);


/**
 * \brief Initializes watchdog module.
 * Has to be called before any function in this module is used.
*/
bool Watchdog_init(void);


/**
 * \brief Reports this thread as active. Should be used on a regular basis within
 * given thread, as program uses those reports to detect unresponsive threads.
 * \param threadId Identifier of reporting thread.
*/
void Watchdog_reportActive(void);


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
