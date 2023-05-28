/**
 * \file cpuusage.h
 * Functions and data types used for processing raw /proc/stat data into
 * readable percentage values.
*/
#ifndef CPUUSAGE_H_INCLUDED
#define CPUUSAGE_H_INCLUDED
#include <stddef.h>
#include "procstat.h"


/**
 * Type used to represent CPU usage value as percentage.
*/
typedef double PercentageValue_t;


/**
 * Structure containing CPU usage statistics for every core, as percentage.
*/
typedef struct CpuUsageInfo
{
	/** Values array length. Expected to be equal to amount of logical processors available plus one. */
	size_t valuesLength;
	/** Usage statistics for every CPU core, expressed in percentage. */
	PercentageValue_t values[];
}
CpuUsageInfo_t;


/**
 * \brief Calculates usage statistics for every core using raw data retrieved at start and end of measurement period.
 * \param oldProcStat Data from /proc/stat retrieved at start of measurement period.
 * \param newProcStat Data from /proc/stat retrieved at start of measurement period.
 * \param output Output buffer for calculated statistics.
*/
void CpuUsageInfo_calculate(const ProcStat_t* oldProcStat, const ProcStat_t* newProcStat, CpuUsageInfo_t* output);


/**
 * \brief Retrieves expected size of CpuUsageInfo_t structure in bytes.
 * \warning Since this function uses CpuCount_get() internally, CpuCount_init() should be called before using it.
 * \return Size of CpuUsageInfo structure, in bytes.
*/
size_t CpuUsageInfo_size(void);


#endif // !CPUUSAGE_H_INCLUDED
