#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


void Log_Error(const char* format, ...);
void Log_Info(const char* format, ...);
void Log_Warning(const char* format, ...);


#define Log_FileLine() do { Log_Info("%s:%i", __FILE__, __LINE__); } while (0)


#endif // !LOG_H_INCLUDED