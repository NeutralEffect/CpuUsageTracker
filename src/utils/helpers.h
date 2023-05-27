/**
 * \file helpers.h
 * Standalone helper functions used in multiple places throughout program.
*/
#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include <time.h>
#include <stddef.h>


/**
 * \brief Converts duration in milliseconds into time point relative to current UTC time.
 * \param ms Amount of time for resulting time point to be shifted forward by, in milliseconds.
 * \return Time point when given amount of milliseconds from now on will pass.
*/
struct timespec TimePointMs(unsigned ms);


/**
 * \brief Read content of requested file into user-provided buffer.
 * This fucntion appends null-terminator automatically, for which one byte of the buffer is reserved.
 * \param fileName Name of file to read.
 * \param bufPtr Pointer to buffer the file content will be read into.
 * \param bufSz Maximum size of provided buffer.
 * \return On success, amount of bytes read from file will be returned,
 * or INT_MAX if that amount is equal to or greater than INT_MAX.
 * Otherwise, a negative integer will be returned.
*/
int ReadFileContent(const char* fileName, char* bufPtr, size_t bufSz);


#endif // !HELPERS_H_INCLUDED