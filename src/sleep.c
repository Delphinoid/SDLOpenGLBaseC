#include "sleep.h"
#include "inline.h"

#ifdef _WIN32

#include <windows.h>

__FORCE_INLINE__ void sleep(const unsigned int s, const unsigned int us){
	Sleep(s/1000 + us*1000);
}

__FORCE_INLINE__ void sleepc(const unsigned int us){
	// Controlled sleep.
	if(us < SLEEP_MAXIMUM_CPU_TICK_US){
		Sleep(us*1000);
	}
}

__FORCE_INLINE__ void sleepm(const int ms){
	// Controlled sleep.
	const int mss = ms - SLEEP_MAXIMUM_CPU_TICK;
	if(mss >= 0){
		Sleep(mss);
	}
}

#else

#include <sys/socket.h>

__FORCE_INLINE__ void sleep(const unsigned int s, const unsigned int us){
	struct timeval tv = {.tv_sec = s, .tv_usec = us};
	select(0, NULL, NULL, NULL, &tv);
}

__FORCE_INLINE__ void sleepc(const unsigned int us){
	// Controlled sleep.
	if(us <= SLEEP_MAXIMUM_CPU_TICK_US){
		struct timeval tv = {.tv_sec = 0, .tv_usec = us};
		select(0, NULL, NULL, NULL, &tv);
	}
}

__FORCE_INLINE__ void sleepm(const int ms){
	// Controlled sleep.
	const int mss = ms - SLEEP_MAXIMUM_CPU_TICK;
	if(mss >= 0){
		const int ss = mss%1000;
		struct timeval tv = {.tv_sec = ss, .tv_usec = (mss - ss*1000)*1000};
		select(0, NULL, NULL, NULL, &tv);
	}
}

#endif