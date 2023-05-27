/**
 * \file sync.h
 * Thread synchronization utilities.
*/
#ifndef SYNC_H_INCLUDED
#define SYNC_H_INCLUDED
#include "sync_types.h"
#include <stdbool.h>
#include <threads.h>


int CondVar_notify(CondVarHandle_t cv);


int CondVar_notifyAll(CondVarHandle_t cv);


int CondVar_waitMs(CondVarHandle_t cv, MutexHandle_t mtx, unsigned ms);


int CondVar_waitUntil(CondVarHandle_t cv, MutexHandle_t mtx, const struct timespec* timePoint);


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


#endif // !SYNC_H_INCLUDED
