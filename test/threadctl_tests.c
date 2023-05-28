#include "threadctl.h"
#include <assert.h>
#include <threads.h>
#include <string.h>


#define TEST_THREAD_NAME_MAX_LENGTH 	20
#define TEST_THREAD_1_NAME 				"T1"
#define TEST_THREAD_2_NAME 				"T2"
#define TEST_THREAD_MAIN_NAME 			"MAIN"
#define TEST_THREAD_1_TID				TID_READER
#define TEST_THREAD_2_TID				TID_PRINTER
#define TEST_THREAD_MAIN_TID			TID_LOGGER


static int thread1Fn(void* arg)
{
	(void) arg;

	static ThreadInfo_t THREAD_1_INFO =
	{
		.name = TEST_THREAD_1_NAME,
		.tid = TEST_THREAD_1_TID
	};

	assert(thrd_success == ThreadInfo_set(&THREAD_1_INFO)); // Couldn't set ThreadInfo for thread 1

	const ThreadInfo_t* tinfo = ThreadInfo_get();

	assert(NULL != tinfo);

	assert(0 == strncmp(tinfo->name, THREAD_1_INFO.name, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == THREAD_1_INFO.tid);
	assert(0 == strncmp(tinfo->name, TEST_THREAD_1_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == TEST_THREAD_1_TID);
	assert(0 != strncmp(tinfo->name, TEST_THREAD_MAIN_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid != TEST_THREAD_MAIN_TID);

	return 0;
}


static int thread2Fn(void* arg)
{
	(void) arg;

	static ThreadInfo_t THREAD_2_INFO =
	{
		.name = TEST_THREAD_2_NAME,
		.tid = TEST_THREAD_2_TID
	};

	assert(thrd_success == ThreadInfo_set(&THREAD_2_INFO)); // Couldn't set ThreadInfo for thread 2

	const ThreadInfo_t* tinfo = ThreadInfo_get();

	assert(0 == strncmp(tinfo->name, THREAD_2_INFO.name, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == THREAD_2_INFO.tid);
	assert(0 == strncmp(tinfo->name, TEST_THREAD_2_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == TEST_THREAD_2_TID);
	assert(0 != strncmp(tinfo->name, TEST_THREAD_MAIN_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid != TEST_THREAD_MAIN_TID);

	return 0;
}


int main()
{
	static ThreadInfo_t THREAD_MAIN_INFO =
	{
		.name = TEST_THREAD_MAIN_NAME,
		.tid = TEST_THREAD_MAIN_TID
	};

	assert(ThreadInfo_init()); // Module has to initialize properly in order to be tested

	assert(thrd_success == ThreadInfo_set(&THREAD_MAIN_INFO)); // Couldn't set ThreadInfo for main thread

	thrd_t thread1Handle;
	thrd_t thread2Handle;
	
	assert(thrd_success == thrd_create(&thread1Handle, thread1Fn, NULL));
	assert(thrd_success == thrd_create(&thread2Handle, thread1Fn, NULL));

	assert(thrd_success == thrd_join(thread1Handle, NULL));
	assert(thrd_success == thrd_join(thread2Handle, NULL));

	const ThreadInfo_t* tinfo = ThreadInfo_get();

	assert(NULL != tinfo);

	assert(0 == strncmp(tinfo->name, THREAD_MAIN_INFO.name, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == THREAD_MAIN_INFO.tid);
	assert(0 == strncmp(tinfo->name, TEST_THREAD_MAIN_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid == TEST_THREAD_MAIN_TID);
	assert(0 != strncmp(tinfo->name, TEST_THREAD_1_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid != TEST_THREAD_1_TID);
	assert(0 != strncmp(tinfo->name, TEST_THREAD_2_NAME, TEST_THREAD_NAME_MAX_LENGTH));
	assert(tinfo->tid != TEST_THREAD_2_TID);

	return 0;
}