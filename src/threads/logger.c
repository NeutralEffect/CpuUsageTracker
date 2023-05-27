#include "logger.h"
#include "watchdog.h"
#include "sync.h"
#include "circbuf.h"
#include "helpers.h"
#include "threadctl.h"
#include <threads.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <time.h>


#define LOG_FILE_NAME 						"cut_log.txt"
#define LOG_TIME_STRING_MAX_LENGTH 			40
#define LOGGER_MESSAGE_BUFFER_ITEM_COUNT 	100u
#define LOGGER_MUTEX_WAIT_TIME_MS 			50
#define LOGGER_CONDVAR_WAIT_TIME_MS 		2000
#define LOGGER_THREAD_ID					TID_LOGGER
#define LOGGER_THREAD_NAME					"Logger"


static ThreadInfo_t g_loggerThreadInfo =
{
	.tid 	= LOGGER_THREAD_ID,
	.name 	= LOGGER_THREAD_NAME
};


static atomic_int g_programLoggingLevel = LLEVEL_TRACE;
// Input buffer allocated dynamically, used throughout entire program's lifespan
// - doesn't need to be destroyed, as it will be cleaned up by system
// once program is closed.
static CircularBuffer_t* g_inBuf = NULL;
// Inner buffer to pointer-swap with input buffer for lower downtime.
static CircularBuffer_t* g_secondaryBuf = NULL;
static mtx_t g_inMtx;
static cnd_t g_inNotEmptyCv;


static void logMessage(FILE* file, const char* message)
{
	fputs(message, file);
}


static void logAllFromCbuf(FILE* file, CircularBuffer_t* cbuf)
{
	char msgbuf[LOG_MESSAGE_MAX_LENGTH];

	while (CircularBuffer_read(cbuf, msgbuf))
	{
		msgbuf[sizeof msgbuf / sizeof *msgbuf - 1] = '\0';
		logMessage(file, msgbuf);
	}
}


static void timeStr(char* bufPtr, size_t bufLen)
{
	time_t now = time(NULL);
	struct tm nowInfo;
	localtime_r(&now, &nowInfo);
	strftime(bufPtr, bufLen, "%H:%M:%S %d/%m/%Y", &nowInfo);
}


void Logger_finalize(void)
{
	CircularBuffer_destroy(g_secondaryBuf);
	CircularBuffer_destroy(g_inBuf);
	cnd_destroy(&g_inNotEmptyCv);
	mtx_destroy(&g_inMtx);
}


int Logger_init(void)
{
	if (thrd_success != mtx_init(&g_inMtx, mtx_timed))
	{
		// Error - mutex creation failed
		return -1;
	}

	if (thrd_success != cnd_init(&g_inNotEmptyCv))
	{
		// Error - condition variable creation failed
		return -2;
	}

	g_inBuf = CircularBuffer_create(LOG_MESSAGE_MAX_LENGTH, LOGGER_MESSAGE_BUFFER_ITEM_COUNT);

	if (NULL == g_inBuf)
	{
		return -3;
	}

	g_secondaryBuf = CircularBuffer_create(LOG_MESSAGE_MAX_LENGTH, LOGGER_MESSAGE_BUFFER_ITEM_COUNT);

	if (NULL == g_secondaryBuf)
	{
		return -4;
	}

	return 0;
}


void Log(LogLevel_t logLevel, const char* format, ...)
{
	if ((int)logLevel > g_programLoggingLevel)
	{
		return;
	}

	static const char* BASE_FORMATS[] =
	{
		[LLEVEL_NONE]		= "",
		[LLEVEL_FATAL]		= "%s [F] %s :: %s\n",
		[LLEVEL_ERROR]		= "%s [E] %s :: %s\n",
		[LLEVEL_WARNING]	= "%s [W] %s :: %s\n",
		[LLEVEL_INFO]		= "%s [I] %s :: %s\n",
		[LLEVEL_DEBUG]		= "%s [D] %s :: %s\n",
		[LLEVEL_TRACE]		= "%s [T] %s :: %s\n"
	};

	char msgbuf[LOG_MESSAGE_MAX_LENGTH];
	char outbuf[LOG_MESSAGE_MAX_LENGTH];
	char timestrbuf[LOG_TIME_STRING_MAX_LENGTH];

	timeStr(timestrbuf, sizeof timestrbuf / sizeof *timestrbuf);
	timestrbuf[LOG_TIME_STRING_MAX_LENGTH - 1] = '\0';

	const char* threadName = "UNSPECIFIED";
	const ThreadInfo_t* threadInfo = ThreadInfo_get();
	if (NULL != threadInfo)
	{
		threadName = threadInfo->name;
	}

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, LOG_MESSAGE_MAX_LENGTH, format, args);
	snprintf(outbuf, LOG_MESSAGE_MAX_LENGTH, BASE_FORMATS[logLevel], timestrbuf, threadName, msgbuf);

	if (thrd_success == Mutex_tryLockMs(&g_inMtx, LOGGER_MUTEX_WAIT_TIME_MS))
	{
		CircularBuffer_write(g_inBuf, outbuf);
		Mutex_unlock(&g_inMtx);
		CondVar_notify(&g_inNotEmptyCv);
	}

	va_end(args);
}


void Logger_setLogLevel(LogLevel_t newLogLevel)
{
	if ((newLogLevel >= LLEVEL_NONE) && (newLogLevel < LLEVEL_COUNT_))
	{
		g_programLoggingLevel = newLogLevel;
	}
}


int LoggerThread(void* rawParams)
{
	(void) rawParams;

	int retval = 0;

	if (thrd_success != ThreadInfo_set(&g_loggerThreadInfo))
	{
		retval = -1;
		goto error_exit_1;
	}

	if (NULL == g_inBuf)
	{
		retval = -2;
		goto error_exit_1;
	}

	FILE* fp = fopen(LOG_FILE_NAME, "w");

	if (NULL == fp)
	{
		retval = -3;
		goto error_exit_1;
	}

	while (false == Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive();

		if (thrd_success != Mutex_tryLockMs(&g_inMtx, LOGGER_MUTEX_WAIT_TIME_MS))
		{

			struct timespec timePoint = TimePointMs(LOGGER_CONDVAR_WAIT_TIME_MS);
			while (CircularBuffer_isEmpty(g_inBuf) && (false == Thread_getKillSwitchStatus()))
			{
				Watchdog_reportActive();

				int result = CondVar_waitUntil(&g_inNotEmptyCv, &g_inMtx, &timePoint);

				if (thrd_success == result)
				{
					Log(LLEVEL_DEBUG, "received notification on input condition variable");
					break;
				}
				else if (thrd_timedout == result)
				{
					Log(LLEVEL_WARNING, "timeout while waiting on input condition variable");
					break;
				}
				else
				{
					Log(LLEVEL_ERROR, "error while waiting on input condition variable");
				}
			}

			if (Thread_getKillSwitchStatus())
			{
				if (thrd_success != Mutex_unlock(&g_inMtx))
				{
					Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
				}
				break;
			}

			CircularBuffer_t* tmp = g_inBuf;
			g_inBuf = g_secondaryBuf;
			g_secondaryBuf = tmp;

			if (thrd_success != Mutex_unlock(&g_inMtx))
			{
				Log(LLEVEL_ERROR, "couldn't release input buffer mutex");
			}

			logAllFromCbuf(fp, g_secondaryBuf);
		}
	}

	// Log any remaining messages before exiting - from both buffers.
	if (thrd_success == Mutex_tryLockMs(&g_inMtx, 0u))
	{
		// Swap buffers first - it's likely that g_secondaryBuf will be empty, thus reducing mutex lock duration.
		CircularBuffer_t* tmp = g_inBuf;
		g_inBuf = g_secondaryBuf;
		g_secondaryBuf = tmp;

		logAllFromCbuf(fp, g_inBuf);
		
		if (thrd_success != Mutex_unlock(&g_inMtx))
		{
			Log(LLEVEL_ERROR, "finalization routine - couldn't release input buffer mutex");
		}

		logAllFromCbuf(fp, g_secondaryBuf);
	}
	else
	{
		Log(LLEVEL_ERROR, "finalization routine - couldn't acquire input buffer mutex");
	}

	Log(LLEVEL_INFO, "thread exiting");

	fclose(fp);
	thrd_exit(0);

error_exit_1:
	thrd_exit(retval);
}
