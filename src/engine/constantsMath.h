#ifndef CONSTANTSMATH_H
#define CONSTANTSMATH_H

#include <math.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#endif
#ifndef M_PI_2
	#define M_PI_2 (M_PI/2.0)
#endif
#ifndef M_PI_4
	#define M_PI_4 (M_PI/4.0)
#endif

#ifndef INFINITY
	#define INFINITY 1.0/0.0
#endif

#ifndef NAN
	#define NAN 0.0/0.0
#endif

// PI over 180, used for converting degrees to radians.
#define RADIAN_RATIO 0.0174532925199432957692369076848861271344287188854172545609719144017100911

#endif
