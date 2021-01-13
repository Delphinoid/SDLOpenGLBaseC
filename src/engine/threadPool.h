#ifndef THREADPOOL_H
#define THREADPOOL_H

/**#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEAN
	#include <windows.h>
	typedef HANDLE threadHandle;
	typedef HANDLE threadMutex;
	typedef x threadCondition;
#else
	#include <pthread.h>
	typedef pthread_t threadHandle;
	typedef pthread_mutex_t threadMutex;
	typedef pthread_cond_t threadCondition;
#endif
#include <stddef.h>

#define THREAD_ACTIVE  0x01
#define THREAD_WORKING 0x02
#define THREAD_ERROR   0x04

typedef struct {
	void *arguments;
	void (*function)(const size_t id, const void *arguments);
} threadTask;

typedef struct {
	threadCondition broadcast;
	size_t threadNum;
	threadHandle *threads;
	size_t taskNum;
	threadTask *taskArray;
	threadMutex taskLock;
} threadPool;

return_t poolInit(threadPool *tp, const size_t threadsAvailable);**/

#endif
