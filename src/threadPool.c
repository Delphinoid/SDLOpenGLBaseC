#include "threadPool.h"

/** Static **/

/**#ifdef _WIN32
DWORD WINAPI threadRun(LPVOID args){
#else
void threadRun(void *args){
#endif

	threadPool *tp = (threadPool *)args;
	threadTask task;

	for(;;){

		//

	}

    return NULL;

}

return_t poolInit(threadPool *tp, const size_t threadsAvailable){
	tp->threadArray = memAllocate(threadsAvailable * sizeof(thread));
	if(tp->threadArray == NULL){
		// Memory allocation failure. **
		return -1;
	}
	for(ta->threadNum = 0; tp->threadNum < threadsAvailable; ++ta->threadNum){
		#ifdef _WIN32
		tp->threadArray[tp->threadNum].handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadRun, &tp->threadArray[tp->threadNum], 0, NULL);
		if(tp->threadArray[tp->threadNum].handle != NULL){
			//tp->threadArray[tp->threadNum].mutex = CreateMutex(NULL, 0, NULL);
		#else
		if(pthread_create(&tp->threadArray[tp->threadNum].handle, NULL, &threadRun, &tp->threadArray[tp->threadNum]) == 0 &&
		   pthread_detach(&tp->threadArray[tp->threadNum].handle) == 0){
			//tp->threadArray[tp->threadNum].mutex = PTHREAD_MUTEX_INITIALIZER;
		#endif
			tp->threadArray[tp->threadNum].id = tp->threadNum;
			tp->threadArray[tp->threadNum].flags = THREAD_ACTIVE;
		}else{
			if(ta->threadNum > 0){
				// An error occurred while creating the thread.
				// Resize threadArray to fit the number of threads that were allocated successfully.
				thread *const tempBuffer = memReallocate(tp->threadArray, tp->threadNum * sizeof(thread));
				if(tempBuffer == NULL){
					// Memory allocation failure. **
					memFree(ta->threadArray);
					return -1;
				}
				tp->threadArray = tempBuffer;
			}else{
				tp->threadArray = NULL;
			}
			return 1;
		}
	}
	return 1;
}

void threadDelete(thread *t){
	if(t->mutex != NULL){
		#ifdef _WIN32
		CloseHandle(&t->mutex);
		#else
		pthread_mutex_destroy(&t->mutex);
		#endif
	}
	if(t->mutex != NULL){
		#ifdef _WIN32
		CloseHandle(&t->handle);
		#else
		flagsUnset(t->flags, THREAD_ACTIVE);
	}
}**/
