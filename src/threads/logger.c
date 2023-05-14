#include "logger.h"
#include "watchdog.h"
#include "thread_utils.h"
#include "circbuf.h"
#include <threads.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdarg.h>


#define LOG_FILE_NAME "cut_log.txt"
#define SLEEP_TIME_SECONDS 1u
#define MUTEX_WAIT_TIME_MS 50u
#define MESSAGE_BUFFER_ITEM_COUNT 10u


static atomic_int g_programLoggingLevel = LLEVEL_TRACE;
// Allocated dynamically, used throughout entire program's lifespan
// - doesn't need to be destroyed, as it will be cleaned up by system
// once program is closed.
static CircularBuffer_t* g_messageBuffer = NULL;
static mtx_t g_messageBufferMutex;


static void logMessage(FILE* file, const char* message)
{
	fputs(message, file);
}


void Logger_init(void)
{
	if (thrd_success != mtx_init(&g_messageBufferMutex, mtx_timed))
	{
		return;
	}

	g_messageBuffer = CircularBuffer_create(LOG_MESSAGE_MAX_LENGTH, MESSAGE_BUFFER_ITEM_COUNT);
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
		[LLEVEL_FATAL]		= "[F] %s\n",
		[LLEVEL_ERROR]		= "[E] %s\n",
		[LLEVEL_WARNING]	= "[W] %s\n",
		[LLEVEL_INFO]		= "[I] %s\n",
		[LLEVEL_DEBUG]		= "[D] %s\n",
		[LLEVEL_TRACE]		= "[T] %s\n"
	};

	char msgbuf[LOG_MESSAGE_MAX_LENGTH];
	char outbuf[LOG_MESSAGE_MAX_LENGTH];

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, LOG_MESSAGE_MAX_LENGTH, format, args);
	snprintf(outbuf, LOG_MESSAGE_MAX_LENGTH, BASE_FORMATS[logLevel], msgbuf);

	if (thrd_success == Mutex_tryLockMs(&g_messageBufferMutex, MUTEX_WAIT_TIME_MS))
	{
		CircularBuffer_write(g_messageBuffer, outbuf);
		Mutex_unlock(&g_messageBufferMutex);
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

	int returnCode = 0;

	if (NULL == g_messageBuffer)
	{
		returnCode = -1;
		goto error_exit_1;
	}

	FILE* fp = fopen(LOG_FILE_NAME, "w");

	if (NULL == fp)
	{
		returnCode = -2;
		goto error_exit_1;
	}

	while (!Thread_getKillSwitchStatus())
	{
		Watchdog_reportActive(TID_LOGGER);

		if (thrd_success == Mutex_tryLock(&g_messageBufferMutex, MUTEX_WAIT_TIME_MS))
		{
			char msgbuf[LOG_MESSAGE_MAX_LENGTH];

			while (CircularBuffer_read(g_messageBuffer, msgbuf))
			{
				// Just to be safe
				msgbuf[LOG_MESSAGE_MAX_LENGTH - 1] = '\0';
				logMessage(fp, msgbuf);
			}

			Mutex_unlock(&g_messageBufferMutex);
		}

		Thread_sleep(SLEEP_TIME_SECONDS);
	}

	fclose(fp);
	thrd_exit(0);

error_exit_1:
	thrd_exit(returnCode);
}