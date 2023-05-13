#include "cpucount.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <sys/sysinfo.h>


static atomic_int cpuCount;
static atomic_bool cpuCountInitialized = false;


void CpuCount_init(void)
{
	if (true == cpuCountInitialized)
	{
		return;
	}

	cpuCount = get_nprocs();
	cpuCountInitialized = true;
}


int CpuCount_get(void)
{
	return cpuCount;
}