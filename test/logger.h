#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED


typedef enum LogLevel
{
	LLEVEL_NONE = 0,
	LLEVEL_FATAL,
	LLEVEL_ERROR,
	LLEVEL_WARNING,
	LLEVEL_INFO,
	LLEVEL_DEBUG,
	LLEVEL_TRACE,
	LLEVEL_COUNT_
}
LogLevel_t;


void Log(LogLevel_t logLevel, const char* format, ...)
{
	(void) logLevel;
	(void) format;
	return;
}


#endif // !LOGGER_H_INCLUDED
