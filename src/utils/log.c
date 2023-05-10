#include "log.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>


#define MESSAGE_BUFFER_SIZE 2048u


void LogError(const char* format, ...)
{
	char msgbuf[MESSAGE_BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, MESSAGE_BUFFER_SIZE, format, args);
	printf("[E] %s\n", msgbuf);
	va_end(args);
}


void LogInfo(const char* format, ...)
{
	char msgbuf[MESSAGE_BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, MESSAGE_BUFFER_SIZE, format, args);
	printf("[I] %s\n", msgbuf);
	va_end(args);
}


void LogWarning(const char* format, ...)
{
	char msgbuf[MESSAGE_BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(msgbuf, MESSAGE_BUFFER_SIZE, format, args);
	printf("[W] %s\n", msgbuf);
	va_end(args);
}