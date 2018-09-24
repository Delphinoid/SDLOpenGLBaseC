#ifndef INLINE_H
#define INLINE_H

#ifdef _MSC_VER
	#define __FORCE_INLINE__ __forceinline
#else
	#define __FORCE_INLINE__ __attribute__((always_inline)) inline
#endif

#endif
