#include "file.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define FILE_NAME "FileTestsTmp.txt"
#define BUFFER_SIZE 8192u
#define NAMEOF(x) #x


static int readFileUsingPopen(void* bufPtr, size_t bufSz)
{
	assert(NULL != bufPtr); // Invalid argument bufPtr
	assert(bufSz > 0); 		// Invalid argument bufSz

	const char COMMAND[] = "cat " FILE_NAME;

	FILE* fp = popen(COMMAND, "r");

	assert(NULL != fp); // File has to be successfully opened to compare content

	return fread(bufPtr, 1, bufSz, fp);
}


static void File_readContentInto_test(void)
{
	assert(0 == system("cp /proc/stat " FILE_NAME)); // Couldn't create temporary artifact for test to use

	char buf1[BUFFER_SIZE];
	char buf2[BUFFER_SIZE];

	assert(File_readContentInto(FILE_NAME, buf1, sizeof buf1) > 0); // No data has been read from file
	assert(readFileUsingPopen(buf2, sizeof buf2) > 0); // No data has been read from file

	assert(0 == strncmp(buf1, buf2, BUFFER_SIZE)); // Test failed
	
	assert(0 == system("rm -f " FILE_NAME)); // Test succeeded, system command to remove temporary artifact failed
}


int main()
{
	File_readContentInto_test();
}