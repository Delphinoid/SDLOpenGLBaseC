#ifndef PHYSICSCONSTRAINT_H
#define PHYSICSCONSTRAINT_H

#include "constantsMath.h"

#ifndef PHYSICS_LINEAR_SLOP
	#define PHYSICS_LINEAR_SLOP 0.005f
#endif

#ifndef PHYSICS_ANGULAR_SLOP
	#define PHYSICS_ANGULAR_SLOP (2.f * RADIAN_RATIO)
#endif

#ifndef PHYSICS_BAUMGARTE_TERM
	#define PHYSICS_BAUMGARTE_TERM 0.1f
#endif

#ifndef PHYSICS_RESTITUTION_THRESHOLD
	#define PHYSICS_RESTITUTION_THRESHOLD 1.f
#endif

#ifndef PHYSICS_SEPARATION_BIAS
	#define PHYSICS_SEPARATION_BIAS 0.f
#endif
#define PHYSICS_SEPARATION_BIAS_TOTAL (2.f * PHYSICS_SEPARATION_BIAS)

// Maximum linear correction for NGS configuration solving.
#ifndef PHYSICS_MAXIMUM_LINEAR_CORRECTION
	#define PHYSICS_MAXIMUM_LINEAR_CORRECTION 0.2f
#endif

// Maximum angular correction for NGS configuration solving.
#ifndef PHYSICS_MAXIMUM_ANGULAR_CORRECTION
	#define PHYSICS_MAXIMUM_ANGULAR_CORRECTION (8.f * RADIAN_RATIO)
#endif

#ifndef PHYSICS_VELOCITY_SOLVER_ITERATIONS
	#define PHYSICS_VELOCITY_SOLVER_ITERATIONS 4
#endif

#ifndef PHYSICS_CONFIGURATION_SOLVER_ITERATIONS
	#define PHYSICS_CONFIGURATION_SOLVER_ITERATIONS 4
#endif

#endif