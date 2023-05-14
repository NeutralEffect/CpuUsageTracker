/**
 * \file file.h
 * File system access.
*/
#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED
#include <stddef.h>


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
int File_readContentInto(const char* fileName, char* bufPtr, size_t bufSz);


#endif // !FILE_H_INCLUDED