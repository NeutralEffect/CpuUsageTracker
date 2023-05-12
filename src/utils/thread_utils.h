#ifndef THREAD_UTILS_H_INCLUDED
#define THREAD_UTILS_H_INCLUDED
#include <stdbool.h>
#include <threads.h>


/**
 * \brief Atomically get current kill switch status.
 * \return Kill switch status.
*/
bool Thread_getKillSwitchStatus(void);


/**
 * \brief Atomically set new status for kill switch.
 * \param status New status.
*/
void Thread_setKillSwitchStatus(bool status);


/**
 * \brief Attempts to lock mutex either until successful or specified amount of time has passed.
 * \param mutex Mutex to be locked.
 * \param waitTimeS Maximum amount of time to wait for mutex to be locked, in seconds.
 * \return
 * thrd_success if mutex has been locked,
 * thrd_timeout if timeout occured without locking the mutex,
 * thrd_error if error occured.
*/
int Mutex_tryLock(mtx_t* mutex, unsigned waitTimeS);


/**
 * \brief As Mutex_tryLock but measuring duration in milliseconds.
 * \param mutex Mutex to lock on, mtx_timed type required.
 * \param waitTimeMs Maximum amount of time to wait for mutex to be locked, in milliseconds.
 * \return 
 * thrd_success if mutex has been locked,
 * thrd_timeout if timeout occured without locking the mutex,
 * thrd_error if error occured.
*/
int Mutex_tryLockMs(mtx_t* mutex, unsigned waitTimeMs);


/**
 * \brief Blocks the execution of the current for at least specified amount of time.
 * \param seconds Amount of time for thread to be blocked for, in seconds.
 * \return 0 if successful, -1 in case of interrupt, other negative value in case of error.
*/
int Thread_sleep(unsigned seconds);


/**
 * \brief As Thread_sleep, but measuring duration in milliseconds.
 * \param seconds Amount of time for thread to be blocked for, in milliseconds.
 * \return As Thread_sleep.
*/
int Thread_sleepMs(unsigned milliseconds);


/**
 * \brief Blocks the execution of the current thread for at least specified amount of time.
 * If interrupted, will repeatedly reenter sleeping state until specified duration has passed.
 * \param seconds Amount of time for thread to be blocked for, in seconds.
*/
void Thread_forceSleep(unsigned seconds);


/**
 * \brief As Thread_forceSleep, but measuring duration in milliseconds.
 * \param seconds Amount of time for thread to be blocked for, in milliseconds.
*/
void Thread_forceSleepMs(unsigned milliseconds);


#endif // !THREAD_UTILS_H_INCLUDED