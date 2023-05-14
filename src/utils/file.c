#include "file.h"
#include <stdio.h>
#include <limits.h>
#include "logger.h"


int File_readContentInto(const char* fileName, char* bufPtr, size_t bufSz)
{
	if (NULL == fileName)
	{
		Log(LLEVEL_ERROR, "invalid argument provided: fileName");
		return -1;
	}

	if (NULL == bufPtr)
	{
		Log(LLEVEL_ERROR, "invalid argument provided: bufPtr");
		return -1;
	}

	if (bufSz < 1)
	{
		Log(LLEVEL_ERROR, "invalid argument provided: bufSz");
		return -1;
	}

	FILE* fp = fopen(fileName, "r");

	if (NULL == fp)
	{
		Log(LLEVEL_ERROR, "cannot open file: %s", fileName);
		return -2;
	}

	// Try to read file's content into provided buffer
	size_t bytesRead = fread(bufPtr, sizeof *bufPtr, bufSz - 1, fp);

	// End-of-file has not been reached yet reading has been aborted,
	// either an error could have occured or file is too large for provided buffer
	if (0 == feof(fp))
	{
		if (0 != ferror(fp))
		{
			Log(LLEVEL_ERROR, "an error has been encountered while attempting to read from file: %s", fileName);

			if (EOF == fclose(fp))
			{
				Log(LLEVEL_ERROR, "an error has been encountered while attempting to close file: %s", fileName);
				return -4;
			}

			return -3;
		}
		else
		{
			Log(LLEVEL_WARNING, "file is too large for provided buffer: %s", fileName);
		}
	}

	// CRUCIAL - FREAD DOES NOT AUTOMATICALLY APEND NULL TERMINATOR, IT HAS TO BE DONE MANUALLY
	bufPtr[bytesRead] = '\0';

	if (EOF == fclose(fp))
	{
		Log(LLEVEL_ERROR, "an error has been encountered while attempting to close file: %s", fileName);
		return -4;
	}

	// Return amount of data loaded into user-provided buffer if possible,
	// otherwise return INT_MAX to indicate the content is longer than maximum size of int
	int retval = (INT_MAX > bytesRead) ? bytesRead : INT_MAX;
	return retval;
}