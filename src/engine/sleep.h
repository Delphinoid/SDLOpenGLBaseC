#ifndef SLEEP_H
#define SLEEP_H

// Maximum expected variance for sleeping, in milliseconds and microseconds.
#ifndef SLEEP_MAXIMUM_CPU_TICK
	#define SLEEP_MAXIMUM_CPU_TICK 1
#endif
#define SLEEP_MAXIMUM_CPU_TICK_US (1000*SLEEP_MAXIMUM_CPU_TICK)

void sleep(const unsigned int s, const unsigned int us);
void sleepc(const unsigned int us);
void sleepm(const int ms);

#endif
