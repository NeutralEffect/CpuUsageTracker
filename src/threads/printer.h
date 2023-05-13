#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED
#include <threads.h>
#include "cpuusage.h"


typedef struct PrinterThreadParams
{
	mtx_t* mutex;
	CpuUsageInfo_t* buffer;
}
PrinterThreadParams_t;


int PrinterThread(void* params);


#endif // !PRINTER_H_INCLUDED