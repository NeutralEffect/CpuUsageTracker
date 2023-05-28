#include "cpucount.h"
#include <assert.h>
#include <unistd.h>


int main()
{
	int cpuCount = sysconf(_SC_NPROCESSORS_ONLN);

	assert(0 < cpuCount);

	CpuCount_init();

	assert(CpuCount_get() == cpuCount);

	return 0;
}