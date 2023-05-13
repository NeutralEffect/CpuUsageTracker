#include "cpuusage.h"
#include "cpucount.h"
#include "procstat.h"
#include <limits.h>
#include <stdio.h>


/**
 * \brief Calculates CPU usage percentage. Formula taken from https://stackoverflow.com/a/23376195.
*/
static PercentageValue_t calculateCpuUsagePercentage(const CpuStat_t* oldStat, const CpuStat_t* newStat)
{
	if ((NULL == oldStat) || (NULL == newStat))
	{
		return UINT_MAX;
	}

	CpuStatValue_t prevIdle = oldStat->values[CSINDEX_IDLE] + oldStat->values[CSINDEX_IOWAIT];
	CpuStatValue_t idle = newStat->values[CSINDEX_IDLE] + newStat->values[CSINDEX_IOWAIT];

	CpuStatValue_t prevNonIdle = 
		oldStat->values[CSINDEX_USER] +
		oldStat->values[CSINDEX_NICE] +
		oldStat->values[CSINDEX_SYSTEM] +
		oldStat->values[CSINDEX_IRQ] +
		oldStat->values[CSINDEX_SOFTIRQ] +
		oldStat->values[CSINDEX_STEAL];

	CpuStatValue_t nonIdle =
		newStat->values[CSINDEX_USER] +
		newStat->values[CSINDEX_NICE] +
		newStat->values[CSINDEX_SYSTEM] +
		newStat->values[CSINDEX_IRQ] +
		newStat->values[CSINDEX_SOFTIRQ] +
		newStat->values[CSINDEX_STEAL];

	CpuStatValue_t prevTotal = prevIdle + prevNonIdle;
	CpuStatValue_t total = idle + nonIdle;

	CpuStatValue_t totald = total - prevTotal;
	CpuStatValue_t idled = idle - prevIdle;

	PercentageValue_t result = ((float) totald - idled) / totald;
	// Adjust from fraction to percentage
	return result * 100.0;
}


void CpuUsageInfo_calculate(const ProcStat_t* oldProcStat, const ProcStat_t* newProcStat, CpuUsageInfo_t* output)
{
	if (NULL == oldProcStat)
	{
		return;
	}

	if (NULL == newProcStat)
	{
		return;
	}

	if (NULL == output)
	{
		return;
	}


	const size_t cpuLineCount = oldProcStat->cpuStatsLength;
	output->valuesLength = cpuLineCount;

	for (unsigned ii = 0; ii < cpuLineCount; ++ii)
	{
		output->values[ii] = calculateCpuUsagePercentage(&oldProcStat->cpuStats[ii], &newProcStat->cpuStats[ii]);
	}
}


size_t CpuUsageInfo_size(void)
{
	return sizeof (CpuUsageInfo_t) + (CpuCount_get() + 1) * sizeof (PercentageValue_t);
}