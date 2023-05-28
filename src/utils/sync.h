/**
 * \file sync.h
 * Thread synchronization utilities.
*/
#ifndef SYNC_H_INCLUDED
#define SYNC_H_INCLUDED
#include "sync_types.h"
#include <stdbool.h>
#include <threads.h>


/**
 * \brief Notifies singular thread waiting on given condition variable (signal).
 * \param cv Condition variable to notify on.
 * \return thrd_success if successful, thrd_error otherwise.
*/
int CondVar_notify(CondVarHandle_t cv);


/**
 * \brief Notifies all threads waiting on given condition variable (broadcast).
 * \param cv Condition variable to notify on.
 * \return thrd_success if successful, thrd_error otherwise.
*/
int CondVar_notifyAll(CondVarHandle_t cv);


/**
 * \brief Releases lock on given mutex and waits for notification on given condition variable,
 * up to a maximum specified amount of time. The mutex will be locked again upon returning from this
 * function.
 * \warning Attempting this function without having acquired lock on given mutex results in undefined behavior.
 * \param cv Condition variable to wait for notification on.
 * \param mtx Mutex to be used for synchronization.
 * \param ms Maximum amount of time to be spent waiting for notification, in milliseconds.
 * \return
 * thrd_success if notification has been received within given amount of time,
 * thrd_timedout if timeout occured without receiving a notification,
 * thrd_error if error occured while waiting.
*/
int CondVar_waitMs(CondVarHandle_t cv, MutexHandle_t mtx, unsigned ms);


/**
 * \brief Releases lock on given mutex and waits for notification on given condition variable,
 * or until time reaches the time point provided. The mutex will be locked again upon returning from this
 * function.
 * \warning Attempting this function without having acquired lock on given mutex results in undefined behavior.
 * \param cv Condition variable to wait for notification on.
 * \param mtx Mutex to be used for synchronization.
 * \param timePoint Point in time upon reaching which thread will be leaving waiting state without checking for notification.
 * \return
 * thrd_success if notification has been received before given time point has been reached,
 * thrd_timedout if timeout occured without receiving a notification,
 * thrd_error if error occured while waiting.
*/
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
