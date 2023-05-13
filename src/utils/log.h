#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


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


/**
 * \brief Send formated message to the log output
 * that will be visible only at or above given logging level.
 * Message formatting follows printf function family formatting.
 * \param logLevel Logging level for message to be assigned.
 * \param format Message format specifier.
 * \param ... Format arguments.
*/
void Log(LogLevel_t logLevel, const char* format, ...);


/**
 * \brief Sets new logging level for program.
 * Messages will be only sent to log output if they have been
 * assigned logging level equal to or greater than that of program.
 * \param newLogLevel New logging level to use.
*/
void SetLogLevel(LogLevel_t newLogLevel);


/**
 * \brief Shorthand for sending message with current source file name and line number into info log output.
*/
#define LogFileLine() do { Log(LLEVEL_DEBUG, "%s:%i", __FILE__, __LINE__); } while (0)


#endif // !LOG_H_INCLUDED