/**
 * \file procstat.h
 * Data types and functions used to access /proc/stat file
 * and convert it's contents into program-usable format.
*/
#ifndef PROCSTAT_H_INCLUDED
#define PROCSTAT_H_INCLUDED
#include <stddef.h>


typedef struct ProcStat ProcStat_t;


/**
 * \brief Create new, blank ProcStat structure with enough capacity to hold information about this system's CPUs.
 * \return Pointer to newly created ProcStat structure, or NULL in case of malloc failure.
 * \warning Resulting structure has to be manually deallocated by user once no longer needed.
*/
ProcStat_t* ProcStat_create(void);


/**
 * \brief Destroy ProcStat structure deallocating memory and cleaning up any resources used.
 * \param self Pointer to ProcStat_t structure to be destroyed.
*/
void ProcStat_destroy(ProcStat_t* self);


/**
 * \brief Retrieve size of struct ProcStat on this system.
 * \return Size of struct ProcStat in bytes.
*/
size_t ProcStat_size(void);


/**
 * \brief Reads /proc/stat file and parses it's content into specialized structure.
 * \return Pointer to dynamically-allocated structure containing data extracted
 * from file contents if successful, NULL in case of failure.
 * \warning The resulting structure has to be deallocated manually by the user.
*/
ProcStat_t* ProcStat_loadFromFile(void);


/**
 * \brief Parses contents of /proc/stat file into specialized structure.
 * \param fileContent Contents of /proc/stat file.
 * \return Pointer to dynamically-allocated structure containing data extracted
 * from file contents if successful, NULL in case of failure.
 * \warning The resulting structure has to be deallocated manually by the user.
*/
ProcStat_t* ProcStat_parse(const char* fileContent);


/**
 * Indices of values corresponding to columns in "cpu(N)" lines of /proc/stat file.
 * Represents processor states.
*/
typedef enum CpuStatIndex
{
	CSINDEX_USER = 0,
	CSINDEX_NICE,
	CSINDEX_SYSTEM,
	CSINDEX_IDLE,
	CSINDEX_IOWAIT,
	CSINDEX_IRQ,
	CSINDEX_SOFTIRQ,
	CSINDEX_STEAL,
	CSINDEX_GUEST,
	CSINDEX_GUESTNICE,
	CSINDEX_COUNT_
}
CpuStatIndex_t;


typedef unsigned long long CpuStatValue_t;


/**
 * Representation of single "cpu(N)" line from /proc/stat file.
*/
typedef struct CpuStat
{
	/**
	 * Array containing values associated with amount of time unit spent in various processor states.
	*/
	CpuStatValue_t values[CSINDEX_COUNT_];
}
CpuStat_t;


/**
 * Representation of data from /proc/stat file, used to hold data parsed from said file.
*/
struct ProcStat
{
	/** CPU stats array length. */
	size_t 		cpuStatsLength;

	/** Array of values corresponding to "cpu(N)" lines in /proc/stat file. */
	CpuStat_t 	cpuStats[];
};


#endif // !PROCSTAT_H_INCLUDED
