/**
 * \file threadctl.h
 * Thread control and management as well as thread-specific storage utilities.
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
 * \brief Atomically get current kill switch status. True indicates activated kill switch,
 * in which case threads should terminate as soon as possible.
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


/**
 * \brief Finalizes thread info module, cleaning up any resources used by it.
 * Should only be called after successful call to ThreadInfo_init() and only when thread info module is no longer in use.
*/
void ThreadInfo_finalize(void);


/**
 * \brief Retrieves information about given thread from it's thread-specific storage.
 * \return Pointer to structure containing information about thread, or NULL in case of failure.
*/
const ThreadInfo_t* ThreadInfo_get(void);


/**
 * \brief Initializes thread info module. Should be called before using any other components from this module.
 * \return true if successful, false otherwise.
*/
bool ThreadInfo_init(void);


/**
 * \brief Sets the pointer to thread information inside thread-specific storage for calling thread.
 * \warning Only pointer and not entire structure is copied. As such, underlying memory should not be deallocated
 * unless user can be certain that it will no longer be accessed.
 * \param tinfo Pointer to structure containing information about calling thread.
 * \return thrd_success if successful, thrd_error otherwise.
*/
int ThreadInfo_set(ThreadInfo_t* tinfo);


#endif // !THREADCTL_H_INCLUDED