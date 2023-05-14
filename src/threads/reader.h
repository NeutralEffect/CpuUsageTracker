#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED
#include <threads.h>
#include "circbuf.h"


typedef struct ReaderThreadParams
{
	mtx_t* mutex;
	CircularBuffer_t* buffer;
}
ReaderThreadParams_t;


/**
 * \brief Thread function for reading /proc/stat file and parsing it's content
 * into ProcStat_t structure.
 * \details Thread will periodically read and parse /proc/stat file and write the result
 * into circular buffer provided through params. Those writes will be only performed if provided
 * mutex is available, as it will be locked for their entire duration.
 * Field mutex of params should point to valid mutex which will be used for synchronizing
 * operations on buffer.
 * Field buffer of params should point to valid CircularBuffer_t structure capable of holding
 * at least one ProcStat_t structure.
 * \param params Pointer to valid ReaderThreadParams_t structure.
*/
int ReaderThread(void* params);


#endif // !READER_H_INCLUDED
