#include "physicsRigidBody.h"
#include <string.h>

#define PHYS_INTEGRATION_STEPS_EULER 2
#define PHYS_INTEGRATION_STEPS_RUNGEKUTTA 4

void physRigidBodyGenerateMassProperties(physRigidBody *body, float **vertexMassArrays){

	/*
	** Calculates the rigid body's total mass, inverse mass,
	** centroid and inertia tensor, as well as the mass
	** properties for each of its colliders.
	*/

	size_t i, j;
	float tempInertiaTensor[6];

	body->mass = 0.f;
	vec3SetS(&body->centroid, 0.f);
	body->inertiaTensor.m[0][0] = 0.f; body->inertiaTensor.m[0][1] = 0.f; body->inertiaTensor.m[0][2] = 0.f;
	body->inertiaTensor.m[1][0] = 0.f; body->inertiaTensor.m[1][1] = 0.f; body->inertiaTensor.m[1][2] = 0.f;
	body->inertiaTensor.m[2][0] = 0.f; body->inertiaTensor.m[2][1] = 0.f; body->inertiaTensor.m[2][2] = 0.f;

	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	for(i = 0; i < body->colliderNum; ++i){

		const float colliderMass = physColliderGenerateMassProperties(&body->colliders[i], vertexMassArrays[i]);
		body->centroid.x += body->colliders[i].centroid.x * colliderMass;// * mass[i];
		body->centroid.y += body->colliders[i].centroid.y * colliderMass;// * mass[i];
		body->centroid.z += body->colliders[i].centroid.z * colliderMass;// * mass[i];
		body->mass += colliderMass;

	}

	body->inverseMass = 1.f / body->mass;
	body->centroid.x *= body->inverseMass;
	body->centroid.y *= body->inverseMass;
	body->centroid.z *= body->inverseMass;

	// Calculate the combined moment of inertia for the body
	// as the sum of its collider's moments.
	for(i = 0; i < body->colliderNum; ++i){

		tempInertiaTensor[0] = 0.f; tempInertiaTensor[1] = 0.f; tempInertiaTensor[2] = 0.f;
		tempInertiaTensor[3] = 0.f; tempInertiaTensor[4] = 0.f; tempInertiaTensor[5] = 0.f;

		for(j = 0; j < body->colliders[i].hull.vertexNum; ++j){

			const float x = body->colliders[i].hull.vertices[j].x - body->centroid.x;  /** Is this correct? **/
			const float y = body->colliders[i].hull.vertices[j].y - body->centroid.y;
			const float z = body->colliders[i].hull.vertices[j].z - body->centroid.z;
			const float sqrX = x*x;
			const float sqrY = y*y;
			const float sqrZ = z*z;
			// xx
			tempInertiaTensor[0] += (sqrY + sqrZ) * vertexMassArrays[i][j];
			// yy
			tempInertiaTensor[1] += (sqrX + sqrZ) * vertexMassArrays[i][j];
			// zz
			tempInertiaTensor[2] += (sqrX + sqrY) * vertexMassArrays[i][j];
			// xy yx
			tempInertiaTensor[3] -= x * y * vertexMassArrays[i][j];
			// xz zx
			tempInertiaTensor[4] -= x * z * vertexMassArrays[i][j];
			// yz zy
			tempInertiaTensor[5] -= y * z * vertexMassArrays[i][j];

		}

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		body->inertiaTensor.m[0][0] += tempInertiaTensor[0];// * avgVertexMass;
		body->inertiaTensor.m[1][1] += tempInertiaTensor[1];// * avgVertexMass;
		body->inertiaTensor.m[2][2] += tempInertiaTensor[2];// * avgVertexMass;
		body->inertiaTensor.m[0][1] += tempInertiaTensor[3];// * avgVertexMass;
		body->inertiaTensor.m[0][2] += tempInertiaTensor[4];// * avgVertexMass;
		body->inertiaTensor.m[1][2] += tempInertiaTensor[5];// * avgVertexMass;

	}

	// No point calculating the same numbers twice.
	body->inertiaTensor.m[1][0] = body->inertiaTensor.m[0][1];
	body->inertiaTensor.m[2][0] = body->inertiaTensor.m[0][2];
	body->inertiaTensor.m[2][1] = body->inertiaTensor.m[1][2];

	//mat3Invert(&body->localInverseInertiaTensor);

}

void physRigidBodyDelete(physRigidBody *body){
	if(body->colliders != NULL){
		size_t i;
		for(i = 0; i < body->colliderNum; ++i){
			physColliderDelete(&body->colliders[i]);
		}
		free(body->colliders);
	}
}

/*void physRigidBodyGenerateMassProperties(physRigidBody *body){

	vec3 *v0;
	vec3 *v1;
	float temp;
	float doubleVolume = 0.f;
	size_t i;

	vec3SetS(&body->localCentroid, 0.f);
	body->localInverseInertiaTensor.m[0][0] = 0.f; body->localInverseInertiaTensor.m[0][1] = 0.f; body->localInverseInertiaTensor.m[0][2] = 0.f;
	body->localInverseInertiaTensor.m[1][0] = 0.f; body->localInverseInertiaTensor.m[1][1] = 0.f; body->localInverseInertiaTensor.m[1][2] = 0.f;
	body->localInverseInertiaTensor.m[2][0] = 0.f; body->localInverseInertiaTensor.m[2][1] = 0.f; body->localInverseInertiaTensor.m[2][2] = 0.f;

	if(body->hull.vertexNum > 0){

		const float avgVertexMass = body->mass * body->hull.vertexNum;

		// Recursively calculate the center of mass.
		for(i = 0; i < body->hull.vertexNum; ++i){

			v0 = &body->hull.vertices[i-1];
			v1 = &body->hull.vertices[i];
			temp = v0->x * v1->y - v0->y * v1->x;
			body->localCentroid.x += (v0->x + v1->x) * temp;
			body->localCentroid.y += (v0->y + v1->y) * temp;
			body->localCentroid.z += (v0->z + v1->z) * temp;
			doubleVolume += temp;
		}

		// Final iteration with the last and first vertices.
		v0 = &body->hull.vertices[body->hull.vertexNum-1];
		v1 = &body->hull.vertices[0];
		temp = v0->x * v1->y - v0->y * v1->x;
		body->localCentroid.x += (v0->x + v1->x) * temp;
		body->localCentroid.y += (v0->y + v1->y) * temp;
		body->localCentroid.z += (v0->z + v1->z) * temp;
		doubleVolume += temp;

		// Calculate the mesh's final center of mass.
		temp = 1.f / (3.f * doubleVolume);
		body->localCentroid.x *= temp;
		body->localCentroid.y *= temp;
		body->localCentroid.z *= temp;

		// Calculate the moment of inertia tensor.
		for(i = 0; i < body->hull.vertexNum; ++i){

			const float x = body->hull.vertices[i].x - body->localCentroid.x;  // Is this correct?
			const float y = body->hull.vertices[i].y - body->localCentroid.y;
			const float z = body->hull.vertices[i].z - body->localCentroid.z;
			const float sqrX = x*x;
			const float sqrY = y*y;
			const float sqrZ = z*z;
			// xx
			body->localInertiaTensor.m[0][0] += (sqrY + sqrZ);// * avgVertexMass;
			// yy
			body->localInertiaTensor.m[1][1] += (sqrX + sqrZ);// * avgVertexMass;
			// zz
			body->localInertiaTensor.m[2][2] += (sqrX + sqrY);// * avgVertexMass;
			// xy yx
			body->localInertiaTensor.m[0][1] -= x * y;// * avgVertexMass;
			// xz zx
			body->localInertiaTensor.m[0][2] -= x * z;// * avgVertexMass;
			// yz zy
			body->localInertiaTensor.m[1][2] -= y * z;// * avgVertexMass;
		}

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		body->localInertiaTensor.m[0][0] *= avgVertexMass;
		body->localInertiaTensor.m[1][1] *= avgVertexMass;
		body->localInertiaTensor.m[2][2] *= avgVertexMass;
		body->localInertiaTensor.m[0][1] *= avgVertexMass;
		body->localInertiaTensor.m[0][2] *= avgVertexMass;
		body->localInertiaTensor.m[1][2] *= avgVertexMass;

		// No point calculating the same numbers twice.
		body->localInertiaTensor.m[1][0] = body->localInertiaTensor.m[0][1];
		body->localInertiaTensor.m[2][0] = body->localInertiaTensor.m[0][2];
		body->localInertiaTensor.m[2][1] = body->localInertiaTensor.m[1][2];

		//mat3Invert(&body->localInverseInertiaTensor);

	}

}*/

void physRBIInit(physRBInstance *prbi){
	prbi->flags = PHYSICS_BODY_SIMULATE | PHYSICS_BODY_COLLIDE;
	prbi->local = NULL;
	prbi->colliders = NULL;
	boneInit(&prbi->configuration[0]);
	boneInit(&prbi->configuration[1]);
	vec3SetS(&prbi->linearVelocity, 0.f);
	vec3SetS(&prbi->angularVelocity, 0.f);
	vec3SetS(&prbi->netForce, 0.f);
	vec3SetS(&prbi->netTorque, 0.f);
	prbi->constraintNum = 0;
	prbi->constraints = NULL;
}

signed char physRBICreate(physRBInstance *prbi, physRigidBody *body){

	size_t i;

	// Allocate memory for each collider.
	physCollider *tempBuffer = malloc(body->colliderNum * sizeof(physCollider));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Copy each collider so we can transform it into global space.
	for(i = 0; i < body->colliderNum; ++i){

		size_t vertexArraySize = body->colliders[i].hull.vertexNum * sizeof(vec3);
		tempBuffer[i].hull.vertices = malloc(vertexArraySize);
		if(tempBuffer[i].hull.vertices == NULL){
			/** Memory allocation failure. **/
			break;
		}

		tempBuffer[i].hull.vertexNum = body->colliders[i].hull.vertexNum;
		memcpy(tempBuffer[i].hull.vertices, body->colliders[i].hull.vertices, vertexArraySize);
		tempBuffer[i].hull.indexNum = body->colliders[i].hull.indexNum;
		tempBuffer[i].hull.indices = body->colliders[i].hull.indices;  // Re-use the indices array.
		tempBuffer[i].aabb = body->colliders[i].aabb;
		tempBuffer[i].centroid = body->colliders[i].centroid;

	}

	// Make sure every collider copied successfully.
	if(i < body->colliderNum){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			free(tempBuffer[i].hull.vertices);
		}
		free(tempBuffer);
		return -1;
	}

	physRBIInit(prbi);
	prbi->local = body;
	prbi->colliders = tempBuffer;
	return 1;

}

signed char physRBIStateCopy(physRBInstance *o, physRBInstance *c){
	return 1;
}

signed char physRBIAddConstraint(physRBInstance *prbi, const physConstraint *c){

	physConstraint *tempBuffer = realloc(prbi->constraints, (prbi->constraintNum+1)*sizeof(size_t));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	prbi->constraints = tempBuffer;
	prbi->constraints[prbi->constraintNum] = *c;
	++prbi->constraintNum;
	return 1;

}

static void physRBICentroidFromPosition(physRBInstance *prbi){
	prbi->centroid = prbi->local->centroid;
	quatGetRotatedVec3(&prbi->configuration[0].orientation, &prbi->centroid);
	vec3AddVToV(&prbi->centroid, &prbi->configuration[0].position);
}

static void physRBIPositionFromCentroid(physRBInstance *prbi){
	prbi->configuration[0].position.x = -prbi->local->centroid.x;
	prbi->configuration[0].position.y = -prbi->local->centroid.y;
	prbi->configuration[0].position.z = -prbi->local->centroid.z;
	quatGetRotatedVec3(&prbi->configuration[0].orientation, &prbi->configuration[0].position);
	vec3AddVToV(&prbi->configuration[0].position, &prbi->centroid);
}

static void physRBIGenerateGlobalInertia(physRBInstance *prbi){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &prbi->configuration[0].orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &prbi->local->inertiaTensor, &prbi->inverseInertiaTensor);
	mat3MultMByM1(&prbi->inverseInertiaTensor, &inverseOrientationMatrix);

}

void physRBIUpdateCollisionMesh(physRBInstance *prbi){

	/*
	** Transform the vertices of each body into global space.
	*/

	if(prbi->local != NULL){  /** Remove? **/

		size_t i, j;
		for(i = 0; i < prbi->local->colliderNum; ++i){

			// Update the collider's global centroid.
			prbi->colliders[i].centroid.x = prbi->local->colliders[i].centroid.x + prbi->configuration[0].position.x;
			prbi->colliders[i].centroid.y = prbi->local->colliders[i].centroid.y + prbi->configuration[0].position.y;
			prbi->colliders[i].centroid.z = prbi->local->colliders[i].centroid.z + prbi->configuration[0].position.z;

			for(j = 0; j < prbi->colliders[i].hull.vertexNum; ++j){

				// Transform the vertex.
				// Subtract the local centroid from the vertex.
				vec3SubVFromVR(&prbi->local->colliders[i].hull.vertices[j], &prbi->local->colliders[i].centroid, &prbi->colliders[i].hull.vertices[j]);
				// Rotate the new vertex around (0, 0, 0).
				quatRotateVec3Fast(&prbi->configuration[0].orientation, &prbi->colliders[i].hull.vertices[j]);
				// Translate it by the global centroid.
				vec3AddVToV(&prbi->colliders[i].hull.vertices[j], &prbi->colliders[i].centroid);

				// Update mesh minima and maxima.
				if(j == 0){
					// Initialize them to the first vertex.
					prbi->colliders[i].aabb.left = prbi->colliders[i].hull.vertices[j].x;
					prbi->colliders[i].aabb.right = prbi->colliders[i].hull.vertices[j].x;
					prbi->colliders[i].aabb.top = prbi->colliders[i].hull.vertices[j].y;
					prbi->colliders[i].aabb.bottom = prbi->colliders[i].hull.vertices[j].y;
					prbi->colliders[i].aabb.front = prbi->colliders[i].hull.vertices[j].z;
					prbi->colliders[i].aabb.back = prbi->colliders[i].hull.vertices[j].z;
				}else{
					// Update aabb.left and aabb.right.
					if(prbi->colliders[i].hull.vertices[j].x <= prbi->colliders[i].aabb.left){
						prbi->colliders[i].aabb.left = prbi->colliders[i].hull.vertices[j].x;
					}else if(prbi->colliders[i].hull.vertices[j].x > prbi->colliders[i].aabb.right){
						prbi->colliders[i].aabb.right = prbi->colliders[i].hull.vertices[j].x;
					}
					// Update aabb.top and aabb.bottom.
					if(prbi->colliders[i].hull.vertices[j].y >= prbi->colliders[i].aabb.top){
						prbi->colliders[i].aabb.top = prbi->colliders[i].hull.vertices[j].y;
					}else if(prbi->colliders[i].hull.vertices[j].y < prbi->colliders[i].aabb.bottom){
						prbi->colliders[i].aabb.bottom = prbi->colliders[i].hull.vertices[j].y;
					}
					// Update aabb.front and aabb.back.
					if(prbi->colliders[i].hull.vertices[j].z >= prbi->colliders[i].aabb.front){
						prbi->colliders[i].aabb.front = prbi->colliders[i].hull.vertices[j].z;
					}else if(prbi->colliders[i].hull.vertices[j].z < prbi->colliders[i].aabb.back){
						prbi->colliders[i].aabb.back = prbi->colliders[i].hull.vertices[j].z;
					}
				}

			}

			// Update body minima and maxima.
			if(i == 0){
				// Initialize them to the first collider's bounding box.
				prbi->aabb.left = prbi->colliders[i].aabb.left;
				prbi->aabb.right = prbi->colliders[i].aabb.right;
				prbi->aabb.top = prbi->colliders[i].aabb.top;
				prbi->aabb.bottom = prbi->colliders[i].aabb.bottom;
				prbi->aabb.front = prbi->colliders[i].aabb.front;
				prbi->aabb.back = prbi->colliders[i].aabb.back;
			}else{
				// Update aabb.left and aabb.right.
				if(prbi->colliders[i].aabb.left <= prbi->aabb.left){
					prbi->aabb.left = prbi->colliders[i].aabb.left;
				}else if(prbi->colliders[i].aabb.right > prbi->aabb.right){
					prbi->aabb.right = prbi->colliders[i].aabb.right;
				}
				// Update aabb.top and aabb.bottom.
				if(prbi->colliders[i].aabb.top >= prbi->aabb.top){
					prbi->aabb.top = prbi->colliders[i].aabb.top;
				}else if(prbi->colliders[i].aabb.bottom < prbi->aabb.bottom){
					prbi->aabb.bottom = prbi->colliders[i].aabb.bottom;
				}
				// Update aabb.front and aabb.back.
				if(prbi->colliders[i].aabb.front >= prbi->aabb.front){
					prbi->aabb.front = prbi->colliders[i].aabb.front;
				}else if(prbi->colliders[i].aabb.back < prbi->aabb.back){
					prbi->aabb.back = prbi->colliders[i].aabb.back;
				}
			}

		}

	}

}

void physRBIApplyForceAtGlobalPoint(physRBInstance *prbi, const vec3 *F, const vec3 *r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in world space.
	*/

	/* Accumulate torque. */
	// T = r x F
	vec3 rsR, rxF;
	vec3SubVFromVR(r, &prbi->centroid, &rsR);
	vec3Cross(&rsR, F, &rxF);
	vec3AddVToV(&prbi->netTorque, &rxF);

	/* Accumulate force. */
	vec3AddVToV(&prbi->netForce, F);

}

void physRBIAddLinearVelocity(physRBInstance *prbi, const vec3 *impulse){
	/* Add to the linear velocity. */
	prbi->linearVelocity.x += impulse->x;
	prbi->linearVelocity.y += impulse->y;
	prbi->linearVelocity.z += impulse->z;
}

void physRBIApplyLinearImpulse(physRBInstance *prbi, const vec3 *impulse){
	/* Apply a linear impulse. */
	prbi->linearVelocity.x += impulse->x * prbi->local->inverseMass;
	prbi->linearVelocity.y += impulse->y * prbi->local->inverseMass;
	prbi->linearVelocity.z += impulse->z * prbi->local->inverseMass;
}

void physRBIAddAngularVelocity(physRBInstance *prbi, const float angle, const float x, const float y, const float z){
	/* Add to the angular velocity. */
	/** This is most definitely naive and incorrect. **/
	prbi->angularVelocity.x += x * angle;
	prbi->angularVelocity.y += y * angle;
	prbi->angularVelocity.z += z * angle;
}

static void physRBIResetForceAccumulator(physRBInstance *prbi){
	prbi->netForce.x = 0.f;
	prbi->netForce.y = 0.f;
	prbi->netForce.z = 0.f;
}

static void physRBIResetTorqueAccumulator(physRBInstance *prbi){
	prbi->netTorque.x = 0.f;
	prbi->netTorque.y = 0.f;
	prbi->netTorque.z = 0.f;
}

void physRBIIntegrateEuler(physRBInstance *prbi, const float dt){

	if(prbi->local != NULL){  /** Remove? **/

		/* Euler integration scheme. */
		const unsigned int steps = PHYS_INTEGRATION_STEPS_EULER;
		const float dtStep = dt*(1.f/PHYS_INTEGRATION_STEPS_EULER);
		vec3 tempVec3;
		float tempFloat;
		quat tempQuat;
		size_t i;

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		for(i = 0; i < steps; ++i){

			/* Calculate linear velocity. */
			// a = F/m
			// dv = a * dt
			prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * dtStep;
			prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * dtStep;
			prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * dtStep;

			/* Update position. */
			prbi->configuration[0].position.x += prbi->linearVelocity.x * dtStep;
			prbi->configuration[0].position.y += prbi->linearVelocity.y * dtStep;
			prbi->configuration[0].position.z += prbi->linearVelocity.z * dtStep;

			/* Calculate angular velocity. */
			tempVec3.x = prbi->netTorque.x * dtStep;
			tempVec3.y = prbi->netTorque.y * dtStep;
			tempVec3.z = prbi->netTorque.z * dtStep;
			mat3MultMByV(&prbi->inverseInertiaTensor, &tempVec3);
			prbi->angularVelocity.x += tempVec3.x;
			prbi->angularVelocity.y += tempVec3.y;
			prbi->angularVelocity.z += tempVec3.z;

			/* Update orientation. */
			// Angle
			tempFloat = vec3GetMagnitude(&prbi->angularVelocity) * dtStep;
			// Axis
			tempVec3 = prbi->angularVelocity;
			vec3NormalizeFast(&tempVec3);
			// Convert axis-angle rotation to a quaternion.
			quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
			quatMultQByQ2(&tempQuat, &prbi->configuration[0].orientation);
			// Normalize the orientation.
			quatNormalizeFast(&prbi->configuration[0].orientation);

		}

		/* Update global centroid. */
		physRBICentroidFromPosition(prbi);

		/* Update constraints. */
		//

		/* Reset force and torque accumulators. */
		prbi->netForce.x = 0.f;
		prbi->netForce.y = 0.f;
		prbi->netForce.z = 0.f;
		prbi->netTorque.x = 0.f;
		prbi->netTorque.y = 0.f;
		prbi->netTorque.z = 0.f;

	}

}

void physRBIIntegrateLeapfrog(physRBInstance *prbi, const float dt){

	if(prbi->local != NULL){  /** Remove? **/

		/* Velocity Verlet integration scheme. */
		vec3 tempVec3;
		float tempFloat;
		quat tempQuat;

		/* Integrate position and linear velocity. */
		tempFloat = 0.5f * dt;
		prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * tempFloat;
		prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * tempFloat;
		prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * tempFloat;
		// Kinematic equation for displacement.
		// x(t+1) = x(t) + (v(t) * dt) + ((0.5 * a(t)) * dt * dt)
		prbi->configuration[0].position.x += prbi->linearVelocity.x * dt;
		prbi->configuration[0].position.y += prbi->linearVelocity.y * dt;
		prbi->configuration[0].position.z += prbi->linearVelocity.z * dt;
		/**polygonResetForce(polygon);**/
		// Kinematic equation for velocity.
		// v(t+1) = v(t) + (a(t+1) * dt)
		// v(t+1) = v(t) + (((a(t) + a(t+1)) / 2) * dt)
		prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * tempFloat;
		prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * tempFloat;
		prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * tempFloat;

		/* Update centroid. */
		physRBICentroidFromPosition(prbi);

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		/* Calculate angular velocity. */
		tempVec3.x = prbi->netTorque.x * dt;
		tempVec3.y = prbi->netTorque.y * dt;
		tempVec3.z = prbi->netTorque.z * dt;
		mat3MultMByV(&prbi->inverseInertiaTensor, &tempVec3);
		prbi->angularVelocity.x += tempVec3.x;
		prbi->angularVelocity.y += tempVec3.y;
		prbi->angularVelocity.z += tempVec3.z;

		/* Update orientation. */
		// Axis
		tempVec3 = prbi->angularVelocity;
		vec3NormalizeFast(&tempVec3);
		// Angle
		tempFloat = vec3GetMagnitude(&prbi->angularVelocity) * dt;
		// Convert axis-angle rotation to a quaternion.
		quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
		quatMultQByQ2(&tempQuat, &prbi->configuration[0].orientation);
		// Normalize the orientation.
		quatNormalizeFast(&prbi->configuration[0].orientation);

		/* Update constraints. */
		//

		/* Reset force and torque accumulators. */
		physRBIResetForceAccumulator(prbi);
		physRBIResetTorqueAccumulator(prbi);

	}

}

static void physRBIIntegrateRungeKutta(physRBInstance *prbi, const float dt){
	/* RK4 integration scheme. */

}

void physRBIUpdate(physRBInstance *prbi, const float dt){

	//

}

void physRBIDelete(physRBInstance *prbi){
	size_t i;
	if(prbi->colliders != NULL){
		for(i = 0; i < prbi->local->colliderNum; ++i){
			// Only free vertices, as we re-use indices from the local collider's hull.
			free(prbi->colliders[i].hull.vertices);
		}
		free(prbi->colliders);
	}
	if(prbi->constraints != NULL){
		free(prbi->constraints);
	}
}
