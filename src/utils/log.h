#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


/**
 * \brief Send formatted message to the error log output.
 * Message formatting is done based on printf function family.
 * \param format Message format specifier.
 * \param ... Format arguments.
*/
void LogError(const char* format, ...);


/**
 * \brief Send formatted message to the info log output.
 * Message formatting is done based on printf function family.
 * \param format Message format specifier.
 * \param ... Format arguments.
*/
void LogInfo(const char* format, ...);


/**
 * \brief Send formatted message to the warning log output.
 * Message formatting is done based on printf function family.
 * \param format Message format specifier.
 * \param ... Format arguments.
*/
void LogWarning(const char* format, ...);


/**
 * \brief Shorthand for sending message with current source file name and line number into info log output.
*/
#define LogFileLine() do { LogInfo("%s:%i", __FILE__, __LINE__); } while (0)


#endif // !LOG_H_INCLUDED