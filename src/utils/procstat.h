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
	CSINDEX_GUESTNICE
}
CpuStatIndex_t;


typedef struct CpuStat
{
	unsigned long long values[10];
}
CpuStat_t;


struct ProcStat
{
	size_t 		cpuStatsLength;
	CpuStat_t 	cpuStats[];
};


#endif // !PROCSTAT_H_INCLUDED