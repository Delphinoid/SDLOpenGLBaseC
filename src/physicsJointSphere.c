#include "physicsJoint.h"
#include "physicsRigidBody.h"
#include <math.h>
#include "inline.h"

static __FORCE_INLINE__ float physJointSphereEffectiveMass(const vec3 pointA, const mat3 inverseInertiaTensorA, const vec3 pointB, const mat3 inverseInertiaTensorB, const vec3 normal, const float inverseMassTotal){

	/*
	** Calculates the effective mass for the constraint.
	** The inverse of this is used as the denominator for the
	** impulse magnitude (constraint Lagrange multiplier):
	**
	** 1 / ((JM^-1)J^T)
	**
	** Where J is the Jacobian row vector, J^T is its transpose
	** and M^-1 is the inverse mass matrix:
	**
	** J = [0, -p, 0, p]
	**
	**       [    -p   ]
	**       [-(rA x p)]
	** J^T = [     p   ]
	**       [ (rB x p)]
	**
	**        [mA^-1  0    0    0  ]
	**        [  0  IA^-1  0    0  ]
	** M^-1 = [  0    0  mB^-1  0  ]
	**        [  0    0    0  IB^-1]
	**
	** Note that p = (pB - pA). J is derived from JV = C', where C is
	** the constraint equation and V is the velocity column vector:
	**
	** C =
	** C' = dC/dt = ((vB + wB X rB) - (vA + wA X rA)) . (pB - pA) = 0
	**
	**     [vA]
	**     [wA]
	** V = [vB]
	**     [wB]
	**
	** Thus, our initial expression above expands to the following:
	**
	** 1 / (mA + mB + ((rA X p) . (IA(rA x p))) + ((rB x p) . (IB(rB x p))))
	*/

	const vec3 angularDeltaA = vec3Cross(pointA, normal);
	const vec3 angularDeltaB = vec3Cross(pointB, normal);

	// We don't invert the effective mass here, as
	// we may modify it in physJointGenerateBias().
	return inverseMassTotal +
	       vec3Dot(angularDeltaA, mat3MMultVKet(inverseInertiaTensorA, angularDeltaA)) +
	       vec3Dot(angularDeltaB, mat3MMultVKet(inverseInertiaTensorB, angularDeltaB));

}

void physJointSpherePresolveConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB, const float dt){

	//

}

void physJointSphereSolveVelocityConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	//

}

#ifdef PHYSICS_SOLVER_GAUSS_SEIDEL
return_t physJointSphereSolveConfigurationConstraints(physJoint *const restrict joint, physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB){

	//

}
#endif