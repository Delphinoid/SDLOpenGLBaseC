#include "timer.h"

#ifdef _WIN32
	static time32_t freq;
	static float rfreq;
#else
	#include <stddef.h>
	#include <errno.h>
	#if !HAVE_NANOSLEEP
		#include <sys/select.h>
	#endif
	#if HAVE_CLOCK_GETTIME
		#ifdef CLOCK_MONOTONIC_RAW
			#define TIMING_MONOTONIC_CLOCK CLOCK_MONOTONIC_RAW
		#else
			#define TIMING_MONOTONIC_CLOCK CLOCK_MONOTONIC
		#endif
	#endif
#endif

// Stores the time value at which the timing system was initialized.
static timerVal_t initial;

void timerInit(){
	#ifdef _WIN32
		LARGE_INTEGER li;
		// This should only fail on versions of Windows before XP.
		if(!QueryPerformanceFrequency(&li)){
			freq = 1;
			rfreq = 1.f;
		}else{
			freq = li.u.LowPart / 1000;
			rfreq = 1000.f / (float)li.QuadPart;
		}
		QueryPerformanceCounter(&initial);
		// timeBeginPeriod() is called when SDL is initialized.
		timeEndPeriod(1);
	#else
		#if HAVE_CLOCK_GETTIME
		clock_gettime(TIMING_MONOTONIC_CLOCK, &initial);
		#else
		gettimeofday(&initial, NULL);
		#endif
	#endif
}

time32_t timerElapsedTime(const timerVal_t start, const timerVal_t end){
	// Return how many milliseconds have
	// passed between "start" and "end".
	#ifdef _WIN32
		return (end.u.LowPart - start.u.LowPart)/freq ;
	#else
		#if HAVE_CLOCK_GETTIME
		return (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec)/1000000;
		#else
		return (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
		#endif
	#endif
}
float timerElapsedTimeFloat(const timerVal_t start, const timerVal_t end){
	// Return how many milliseconds have
	// passed between "start" and "end".
	#ifdef _WIN32
		return (float)(end.u.LowPart - start.u.LowPart)*rfreq;
	#else
		#if HAVE_CLOCK_GETTIME
		return (float)((end.tv_sec - start.tv_sec)*1000) + (float)(end.tv_nsec - start.tv_nsec)/1000000.f;
		#else
		return (float)((end.tv_sec - start.tv_sec)*1000) + (float)(end.tv_usec - start.tv_usec)/1000.f;
		#endif
	#endif
}

time32_t timerGetTime(){
	// Return how many milliseconds have elapsed
	// since the timing system was initialized.
	#ifdef _WIN32
		timerVal_t now;
		QueryPerformanceCounter(&now);
	#else
		#if HAVE_CLOCK_GETTIME
				timerVal_t now;
				clock_gettime(TIMING_MONOTONIC_CLOCK, &now);
		#else
		timerVal_t now;
		gettimeofday(&now, NULL);
		#endif
	#endif
	return timerElapsedTime(initial, now);
}
float timerGetTimeFloat(){
	// Return how many milliseconds have elapsed
	// since the timing system was initialized.
	#ifdef _WIN32
		timerVal_t now;
		QueryPerformanceCounter(&now);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t now;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &now);
		#else
		timerVal_t now;
		gettimeofday(&now, NULL);
		#endif
	#endif
	return timerElapsedTimeFloat(initial, now);
}

timerVal_t timerStart(){
	// Returns the current tick, which is mostly
	// useless for timing without the frequency.
	#ifdef _WIN32
		timerVal_t start;
		QueryPerformanceCounter(&start);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t start;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &start);
		#else
		timerVal_t start;
		gettimeofday(&start, NULL);
		#endif
	#endif
	return start;
}
time32_t timerEnd(const timerVal_t start){
	// Return how many milliseconds have elapsed since "start".
	#ifdef _WIN32
		timerVal_t end;
		QueryPerformanceCounter(&end);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t end;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &end);
		#else
		timerVal_t end;
		gettimeofday(&end, NULL);
		#endif
	#endif
	return timerElapsedTime(start, end);
}
float timerEndFloat(const timerVal_t start){
	// Return how many milliseconds have elapsed since "start".
	#ifdef _WIN32
		timerVal_t end;
		QueryPerformanceCounter(&end);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t end;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &end);
		#else
		timerVal_t end;
		gettimeofday(&end, NULL);
		#endif
	#endif
	return timerElapsedTimeFloat(start, end);
}

#ifndef _WIN32
// On UNIX derivatives, we only have a single sleep function (and busy-waiting).
#if HAVE_NANOSLEEP
static void sleepUnix(const time32_t ms){
#else
static void sleepUnix(time32_t ms){
#endif

	int r;

	#if HAVE_NANOSLEEP
	const time32_t s = ms / 1000;
	timerVal_t tv = {
		.tv_sec = s;
		.tv_nsec = (ms - s * 1000) * 1000000;
	};
	#else
	time32_t s = ms / 1000;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	#endif

	// We may get interrupted while sleeping, so
	// keep trying until we've slept for long enough.
	do {

		#if HAVE_NANOSLEEP

		const struct timespec ts = {
			.tv_sec = tv.tv_sec;
			.tv_nsec = tv.tv_nsec;
		};
		errno = 0;
		r = nanosleep(&ts, &tv);

		#else

		struct timeval now;
		const time32_t elapsed = (gettimeofday(&now, NULL), timerElapsedTime(tv, now));
		if(elapsed >= ms){
			return;
		}
		ms -= elapsed;

		s = ms / 1000;
		tv.tv_sec = s;
		tv.tv_usec = (ms - s * 1000) * 1000;

		errno = 0;
		r = select(0, NULL, NULL, NULL, &tv);

		#endif

	} while(r == -1 && errno == EINTR);

}
#endif

void sleepResolution(const time32_t ms, const unsigned int res){
	// Sleep with a particular timer resolution (Windows only).
	// The system's default resolution is used if "res" is '0'.
	#ifdef _WIN32
		timeBeginPeriod(res);
		Sleep(ms);
		timeEndPeriod(res);
	#else
		sleepUnix(ms);
	#endif
}
void sleepInaccurate(const time32_t ms){
	// Although possibly more lenient on the processor,
	// it tends to sleep for much longer than specified.
	#ifdef _WIN32
		timeBeginPeriod(ms);
		Sleep(ms);
		timeEndPeriod(ms);
	#else
		sleepUnix(ms);
	#endif
}
void sleepAccurate(const time32_t ms){
	// Good accuracy and decently low power usage.
	#ifdef _WIN32
		timeBeginPeriod(1);
		Sleep(ms);
		timeEndPeriod(1);
	#else
		sleepUnix(ms);
	#endif
}
void sleepBusy(const time32_t ms){
	// Extremely accurate but not very kind on the processor.
	timerVal_t now = timerStart();
	#ifdef _WIN32
		const time64_t end = now.QuadPart + ms * freq;
		while(now = timerStart(), now.QuadPart < end);
	#else
		#if HAVE_CLOCK_GETTIME
		const time64_t end = ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_nsec) + (time64_t)ms * 1000000;
		while(now = timerStart(), ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_nsec) < end);
		#else
		const time64_t end = ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_usec) + (time64_t)ms * 1000000;
		while(now = timerStart(), ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_usec) < end);
		#endif
	#endif
}