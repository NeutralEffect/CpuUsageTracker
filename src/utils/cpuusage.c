#include "cpuusage.h"
#include "cpucount.h"
#include "procstat.h"
#include <stdio.h>


/**
 * \brief Calculates CPU usage percentage. Formula taken from https://stackoverflow.com/a/23376195.
 * \param oldStat Processor state time unit measurement taken at the start of measurement period. 
 * \param newStat Processor state time unit measurement taken at the end of measurement period.
 * \return Processor usage as percentage value in 0-100 range.
*/
static PercentageValue_t calculateCpuUsagePercentage(const CpuStat_t* oldStat, const CpuStat_t* newStat)
{
	static const double ERROR_VAL = -1.0;

	if ((NULL == oldStat) || (NULL == newStat))
	{
		return ERROR_VAL;
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

	PercentageValue_t result = (totald != 0.0) ? ((double) totald - idled) / totald : ERROR_VAL;
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
