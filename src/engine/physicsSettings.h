#ifndef PHYSICSSETTINGS_H
#define PHYSICSSETTINGS_H

// Costly inertia tensor scaling based on the object's scale.
//#define PHYSICS_BODY_SCALE_INERTIA_TENSORS

// Delay the application of friction by one tick; that is,
// only apply friction to persistent contacts. No effect on
// performance, but allows for techniques such as bhopping.
#define PHYSICS_CONTACT_FRICTION_DELAY

// Treat friction as a separate constraint. May be faster.
#define PHYSICS_CONTACT_FRICTION_CONSTRAINT

// Use a geometric average method for combining frictional
// coefficients, rather than the square root of their product.
#define PHYSICS_CONTACT_FRICTION_GEOMETRIC_AVERAGE

// Use a Gauss Seidel positional solver for constraints.
// This is slower, but should make constraints significantly
// more stable.
#define PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL

// Warm start all constraints.
#define PHYSICS_CONSTRAINT_WARM_START

#endif
