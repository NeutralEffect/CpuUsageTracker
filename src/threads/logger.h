/**
 * \file logger.h
 * Logger thread and logging interface.
*/
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED


/**
 * Maximum length of single log message. Longer messages will be truncated.
*/
#define LOG_MESSAGE_MAX_LENGTH 1024


/**
 * Logging levels available in program.
*/
typedef enum LogLevel
{
	/** Logging disabled. */
	LLEVEL_NONE = 0,

	/** Fatal errors necessitating program termination. */
	LLEVEL_FATAL,

	/** Non-fatal errors which might cause program malfunction. */
	LLEVEL_ERROR,

	/** Warnings about unusual or hazardous circumstances. */
	LLEVEL_WARNING,

	/** Informational messages regarding normal program operation. */
	LLEVEL_INFO,

	/** Detailed information about program's state'. */
	LLEVEL_DEBUG,

	/** Raw data and complex information. */
	LLEVEL_TRACE,
	
	/** Amount of values in this enum, not a valid value by itself. */
	LLEVEL_COUNT_
}
LogLevel_t;


#ifndef CUT_DISABLE_LOGGING
/**
 * \brief Send formated message to the log output
 * that will be visible only at or above given logging level.
 * Message formatting follows printf function family formatting.
 * \warning Requires call to Logger_init() to be completed before using,
 * otherwise might result in undefined behavior.
 * \param logLevel Logging level for message to be assigned.
 * \param format Message format specifier.
 * \param ... Format arguments.
*/
void Log(LogLevel_t logLevel, const char* format, ...);
#else
#define Log(...) ((void) 0)
#endif // CUT_DISABLE_LOGGING


/**
 * \brief Finalizes logger module, cleaning up any resources used by it.
 * Should only be called after successful call to Logger_init() and only when logger module is no longer in use.
*/
void Logger_finalize(void);


/**
 * \brief Initializes logger module. Should be called before using any other components from this module.
 * \return 0 if successful, negative error code otherwise.
*/
int Logger_init(void);


/**
 * \brief Sets new logging level for program.
 * Messages will be only sent to log output if they have been
 * assigned logging level equal to or greater than that of program.
 * \param newLogLevel New logging level to use.
*/
void Logger_setLogLevel(LogLevel_t newLogLevel);


/**
 * \brief Shorthand for sending message with current source file name and line number into debug log output.
*/
#define LogFileLine() do { Log(LLEVEL_DEBUG, "%s:%i", __FILE__, __LINE__); } while (0)


/**
 * \brief Thread function for retrieving log messages from program components and saving them into a text file.
 * \warning This thread should be launched before logging functionality is used.
 * \param params Ignored.
*/
int LoggerThread(void* params);


#endif // !LOGGER_H_INCLUDED
