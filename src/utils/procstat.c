#include "procstat.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>


#define MAX_FILE_LINE_LENGTH 4096u
#define MAX_FILE_LINE_COUNT 2048u


/**
 * \brief Get number of available logical processors.
 * Current implementation is a wrapper for get_nprocs.
 * \return Number of processors.
*/
static inline int getProcessorCount(void)
{
	return get_nprocs();
}


/**
 * \brief Parse contents of /proc/stat file, modifying contents in the process.
 * \param mutableFileContent Content of /proc/stat file. Will be modified.
 * \return Pointer to dynamically-allocated structure containing data extracted
 * from file contents if successful, NULL in case of failure.
*/
static ProcStat_t* parse(char* mutableFileContent)
{
	// Delimiting by '\n' allows us to split file content by lines
	const char DELIMITERS[] 			= "\n";
	// Accounts for initial "cpu(N)" string along with 10 significant values, asterisk to ignore assignment of string prefix
	static const char SSCANF_FORMAT[] 	= "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu";

	// Create ProcStat structure first, since if it fails the entire function should abort
	ProcStat_t* result = ProcStat_create();
	if (NULL == result)
	{
		return NULL;
	}

	char* linesQueue[MAX_FILE_LINE_COUNT] = { NULL };
	int linesQueueEnd = 0;

	// Divide file content by lines, save them to queue
	for (char* p = strtok(mutableFileContent, DELIMITERS); NULL != p; p = strtok(NULL, DELIMITERS))
	{
		linesQueue[linesQueueEnd] = p;
		++linesQueueEnd;
	}

	/* CPU usage can be computed using only "cpu" and "cpuN" lines,
	 * and since they are the first lines in the /proc/stat file
	 * we can safely take only those first lines from it and ignore others.
	 * Adding one to account for total "cpu" line.
	 */
	const int significantLinesCount = getProcessorCount() + 1;

	// Scan "cpu(N)" lines into structure
	for (int ii = 0; ii < significantLinesCount; ++ii)
	{
		sscanf(
			linesQueue[ii],
			SSCANF_FORMAT,
			&result->cpuStats[ii].values[0],
			&result->cpuStats[ii].values[1],
			&result->cpuStats[ii].values[2],
			&result->cpuStats[ii].values[3],
			&result->cpuStats[ii].values[4],
			&result->cpuStats[ii].values[5],
			&result->cpuStats[ii].values[6],
			&result->cpuStats[ii].values[7],
			&result->cpuStats[ii].values[8],
			&result->cpuStats[ii].values[9]);
	}

	return result;
}


ProcStat_t* ProcStat_create(void)
{
	// Add one to account for total "cpu" line
	const int cpuCount = getProcessorCount() + 1;
	const size_t size = sizeof(ProcStat_t) + sizeof(CpuStat_t) * cpuCount;
	ProcStat_t* result = malloc(size);

	if (NULL == result)
	{
		return NULL;
	}

	/* Initialize all fields to 0.
	 * Could be split into two calls to avoid setting result->cpuStatsLength to zero as well,
	 * but the overhead of function call is likely higher than that of overwriting a few more bytes.
	 */
	memset(result, 0, size);
	// Save CPU count.
	result->cpuStatsLength = cpuCount;
	return result;
}


ProcStat_t* ProcStat_parseFrom(const char* fileContent)
{
	if (NULL == fileContent)
	{
		Log_Error("invalid argument provided: fileContent");
		return NULL;
	}

	// Create mutable copy of original file content
	size_t len = strlen(fileContent);
	// Allocate buffer, adjust by +1 byte for null terminator
	char* copy = (char*) malloc(len + 1u);

	if (NULL == copy)
	{
		Log_Error("cannot allocate memory for content buffer");
		return NULL;
	}

	strncpy(copy, fileContent, len);
	// Append terminating null-character for safety
	copy[len] = '\0';
	ProcStat_t* result = parse(copy);

	// Release memory used for mutable copy
	free(copy);

	return result;
}