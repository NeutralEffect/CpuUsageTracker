/**
 * \file sync_types.h
 * Types used by sync module.
*/
#ifndef SYNC_TYPES_H_INCLUDED
#define SYNC_TYPES_H_INCLUDED
#include <threads.h>


/**
 * Condition variable handle type, used with functions performing operations on condition variables.
*/
typedef cnd_t* CondVarHandle_t;

/**
 * Mutex handle type, used with functions performing operations on mutexes.
*/
typedef mtx_t* MutexHandle_t;


#endif // !SYNC_TYPES_H_INCLUDED