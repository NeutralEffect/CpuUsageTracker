#include "cpucount.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <sys/sysinfo.h>


static atomic_int g_cpuCount;
static atomic_bool g_cpuCountInitialized = false;


void CpuCount_init(void)
{
	if (true == g_cpuCountInitialized)
	{
		return;
	}

	g_cpuCount = get_nprocs();
	g_cpuCountInitialized = true;
}


int CpuCount_get(void)
{
	return g_cpuCount;
}