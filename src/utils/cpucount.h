#ifndef CPUCOUNT_H_INCLUDED
#define CPUCOUNT_H_INCLUDED


/**
 * \brief Initializes CPU count with information retrieved from system.
 * \warning This function is NOT thread-safe and should never be called from many threads.
*/
void CpuCount_init(void);


/**
 * \brief Retreives amount of available processors. This function is thread-safe.
 * \warning CpuCount_init has be to called at least once before using this function,
 * otherwise it's behavior is undefined.
 * \returns Amount of available logical processors.
*/
int CpuCount_get(void);


#endif // !CPUCOUNT_H_INCLUDED
