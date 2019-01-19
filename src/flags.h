#ifndef FLAGS_H
#define FLAGS_H

#include <stdint.h>

typedef uint_least8_t flags_t;

#define flagsSet(flags, bits) flags |= bits
#define flagsUnset(flags, bits) flags &= ~(bits)
#define flagsMask(flags, bits) flags &= bits

#define flagsCheck(flags, bits) (flags & (bits))
#define flagsAreSet(flags, bits) (flags & (bits)) > 0
#define flagsAreUnset(flags, bits) (flags & (bits)) == 0

#endif
