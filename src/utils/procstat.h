#ifndef PROCSTAT_H_INCLUDED
#define PROCSTAT_H_INCLUDED
#include <stddef.h>


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
	long values[10];
}
CpuStat_t;


typedef struct ProcStat
{
	CpuStat_t cpuStatTotal;
	size_t cpuStatsLength;
	CpuStat_t cpuStats[];
}
ProcStat_t;


ProcStat_t* ProcStat_parse(const char* fileContent);


#endif // !PROCSTAT_H_INCLUDED