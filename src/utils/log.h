#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


void LogError(const char* format, ...);
void LogInfo(const char* format, ...);
void LogWarning(const char* format, ...);


#define LogFileLine() do { Log_Info("%s:%i", __FILE__, __LINE__); } while (0)


#endif // !LOG_H_INCLUDED