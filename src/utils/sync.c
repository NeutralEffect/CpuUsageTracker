#include "sync.h"
#include <threads.h>
#include "logger.h"
#include "helpers.h"


int CondVar_notify(CondVarHandle_t cv)
{
	return cnd_signal(cv);
}


int CondVar_notifyAll(CondVarHandle_t cv)
{
	return cnd_broadcast(cv);
}


int CondVar_waitMs(CondVarHandle_t cv, MutexHandle_t mtx, unsigned ms)
{
	if ( (NULL == cv) || (NULL == mtx) )
	{
		return thrd_error;
	}

	const struct timespec timePoint = TimePointMs(ms);

	return cnd_timedwait(cv, mtx, &timePoint);
}


int CondVar_waitUntil(CondVarHandle_t cv, MutexHandle_t mtx, const struct timespec* timePoint)
{
	if ( (NULL == cv) || (NULL == mtx) )
	{
		return thrd_error;
	}

	return cnd_timedwait(cv, mtx, timePoint);
}


int Mutex_tryLock(MutexHandle_t mutex, unsigned waitTimeS)
{
	return Mutex_tryLockMs(mutex, waitTimeS * 1000u);
}


int Mutex_tryLockMs(MutexHandle_t mutex, unsigned waitTimeMs)
{
	if (NULL == mutex)
	{
		return thrd_error;
	}

	struct timespec timePoint = TimePointMs(waitTimeMs);
	
	return mtx_timedlock(mutex, &timePoint);
}


int Mutex_unlock(MutexHandle_t mutex)
{
	return mtx_unlock(mutex);
}
