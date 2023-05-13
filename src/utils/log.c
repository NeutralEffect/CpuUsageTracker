#include "log.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdatomic.h>


#define MESSAGE_BUFFER_SIZE 2048u


static atomic_int programLoggingLevel = LLEVEL_TRACE;


void Log(LogLevel_t logLevel, const char* format, ...)
{
	if ((int)logLevel > programLoggingLevel)
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

	char msgbuf[MESSAGE_BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, MESSAGE_BUFFER_SIZE, format, args);
	printf(BASE_FORMATS[logLevel], msgbuf);
	va_end(args);
}


void SetLogLevel(LogLevel_t newLogLevel)
{
	if ((newLogLevel >= LLEVEL_NONE) && (newLogLevel < LLEVEL_COUNT_))
	{
		programLoggingLevel = newLogLevel;
	}
}