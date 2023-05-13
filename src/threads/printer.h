#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED
#include <threads.h>
#include "circbuf.h"


typedef struct PrinterThreadParams
{
	mtx_t* mutex;
	CircularBuffer_t* buffer;
}
PrinterThreadParams_t;


int PrinterThread(void* params);


#endif // !PRINTER_H_INCLUDED