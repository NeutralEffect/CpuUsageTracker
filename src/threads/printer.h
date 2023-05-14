#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED
#include <threads.h>
#include "cpuusage.h"


/**
 * Paramters required by PrinterThread() function.
*/
typedef struct PrinterThreadParams
{
	/**
	 * Mutex to lock on while reading usage statistics from input buffer.
	*/
	mtx_t* mutex;
	
	/**
	 * Input buffer to read CPU usage statistics from. Buffer should be shared with analyzer thread
	 * and hold either valid data or be zero-initialized.
	*/
	CpuUsageInfo_t* buffer;
}
PrinterThreadParams_t;


/**
 * \brief Thread function for retrieving information about CPU usage and printing it to standard output.
 * \details Thread will periodically read data from provided buffer and print it to
 * standard output using predefined format showing usage of every logical processor in percentages.
 * \param params Pointer to valid PrinterThreadParams_t structure.
*/
int PrinterThread(void* params);


#endif // !PRINTER_H_INCLUDED
