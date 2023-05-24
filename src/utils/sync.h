/**
 * \file thread_utils.h
 * Thread synchronization, communication and management utilities.
*/
#ifndef THREAD_UTILS_H_INCLUDED
#define THREAD_UTILS_H_INCLUDED
#include "sync_types.h"
#include <stdbool.h>
#include <threads.h>


int CondVar_notify(CondVarHandle_t cv);


int CondVar_notifyAll(CondVarHandle_t cv);


int CondVar_waitMs(CondVarHandle_t cv, MutexHandle_t mtx, unsigned ms);


/**
 * \brief Attempts to lock mutex either until successful or specified amount of time has passed.
 * \param mutex Mutex to be locked.
 * \param waitTimeS Maximum amount of time to wait for mutex to be locked, in seconds.
 * \return
 * thrd_success if mutex has been locked,
 * thrd_timedout if timeout occured without locking the mutex,
 * thrd_error if error occured.
*/
int Mutex_tryLock(MutexHandle_t mutex, unsigned waitTimeS);


/**
 * \brief As Mutex_tryLock but measuring duration in milliseconds.
 * \param mutex Mutex to lock on, mtx_timed type required.
 * \param waitTimeMs Maximum amount of time to wait for mutex to be locked, in milliseconds.
 * \return 
 * thrd_success if mutex has been locked,
 * thrd_timedout if timeout occured without locking the mutex,
 * thrd_error if error occured.
*/
int Mutex_tryLockMs(MutexHandle_t mutex, unsigned waitTimeMs);


/**
 * \brief Unlocks provided mutex.
 * \param mutex Mutex to be unlocked.
 * \return thrd_success if succesful, thrd_error otherwise.
*/
int Mutex_unlock(MutexHandle_t mutex);


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


#endif // !THREAD_UTILS_H_INCLUDED
