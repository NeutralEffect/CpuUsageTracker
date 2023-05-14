#ifndef CPUUSAGE_H_INCLUDED
#define CPUUSAGE_H_INCLUDED
#include <stddef.h>
#include "procstat.h"


typedef double PercentageValue_t;


typedef struct CpuUsageInfo
{
	size_t valuesLength;
	PercentageValue_t values[];
}
CpuUsageInfo_t;


void CpuUsageInfo_calculate(const ProcStat_t* oldProcStat, const ProcStat_t* newProcStat, CpuUsageInfo_t* output);


size_t CpuUsageInfo_size(void);


#endif // !CPUUSAGE_H_INCLUDED
