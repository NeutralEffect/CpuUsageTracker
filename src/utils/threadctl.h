/**
 * \file threadctl.h
 * Thread control and management utilities.
*/
#ifndef THREADCTL_H_INCLUDED
#define THREADCTL_H_INCLUDED
#include <stdbool.h>


#define THREAD_NAME_LENGTH 64u


/**
 * Thread identifiers.
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
 * Basic information used to identify given thread.
*/
typedef struct ThreadInfo
{
	ThreadId_t 	tid;
	char 		name[THREAD_NAME_LENGTH];
}
ThreadInfo_t;


/**
 * \brief Activate kill switch, atomically setting it's value.
*/
void Thread_activateKillSwitch(void);


/**
 * \brief Blocks the execution of the current thread for at least specified amount of time.
 * If interrupted, will repeatedly reenter sleeping state until specified duration has passed.
 * \param seconds Amount of time for thread to be blocked for, in seconds.
*/
void Thread_forceSleep(unsigned seconds);


/**
 * \brief As Thread_forceSleep, but measuring duration in milliseconds.
 * \param milliseconds Amount of time for thread to be blocked for, in milliseconds.
*/
void Thread_forceSleepMs(unsigned milliseconds);


/**
 * \brief Atomically get current kill switch status.
 * \return Kill switch status.
*/
bool Thread_getKillSwitchStatus(void);


/**
 * \brief Blocks the execution of the current for at least specified amount of time.
 * \param seconds Amount of time for thread to be blocked for, in seconds.
 * \return 0 if successful, -1 in case of interrupt, other negative value in case of error.
*/
int Thread_sleep(unsigned seconds);


/**
 * \brief As Thread_sleep, but measuring duration in milliseconds.
 * \param milliseconds Amount of time for thread to be blocked for, in milliseconds.
 * \return As Thread_sleep.
*/
int Thread_sleepMs(unsigned milliseconds);


const ThreadInfo_t* ThreadInfo_get(void);


bool ThreadInfo_init(void);


int ThreadInfo_set(ThreadInfo_t* tinfo);


#endif // !THREADCTL_H_INCLUDED