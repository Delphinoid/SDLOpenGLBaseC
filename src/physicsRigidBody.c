#include "physicsRigidBody.h"

#define PHYS_INTEGRATION_STEPS_EULER 2
#define PHYS_INTEGRATION_STEPS_RUNGEKUTTA 4

void physColliderGenerateMassProperties(physCollider *collider, const float mass, mat3 *inertiaTensor){

	vec3 *v0;
	vec3 *v1;
	float temp;
	float doubleVolume = 0.f;
	size_t i;

	vec3SetS(&collider->localCentroid, 0.f);
	inertiaTensor->m[0][0] = 0.f; inertiaTensor->m[0][1] = 0.f; inertiaTensor->m[0][2] = 0.f;
	inertiaTensor->m[1][0] = 0.f; inertiaTensor->m[1][1] = 0.f; inertiaTensor->m[1][2] = 0.f;
	inertiaTensor->m[2][0] = 0.f; inertiaTensor->m[2][1] = 0.f; inertiaTensor->m[2][2] = 0.f;

	if(collider->localHull.vertexNum > 0){

		/** Pass in an array of vertex masses? Probably dumb. **/
		const float avgVertexMass = mass / collider->localHull.vertexNum;

		// Recursively calculate the center of mass.
		for(i = 0; i < collider->localHull.vertexNum; ++i){

			v0 = &collider->localHull.vertices[i-1];
			v1 = &collider->localHull.vertices[i];
			temp = v0->x * v1->y - v0->y * v1->x;
			collider->localCentroid.x += (v0->x + v1->x) * temp;
			collider->localCentroid.y += (v0->y + v1->y) * temp;
			collider->localCentroid.z += (v0->z + v1->z) * temp;
			doubleVolume += temp;
		}

		// Final iteration with the last and first vertices.
		v0 = &collider->localHull.vertices[collider->localHull.vertexNum-1];
		v1 = &collider->localHull.vertices[0];
		temp = v0->x * v1->y - v0->y * v1->x;
		collider->localCentroid.x += (v0->x + v1->x) * temp;
		collider->localCentroid.y += (v0->y + v1->y) * temp;
		collider->localCentroid.z += (v0->z + v1->z) * temp;
		doubleVolume += temp;

		// Calculate the mesh's final center of mass.
		temp = 1.f / (3.f * doubleVolume);
		collider->localCentroid.x *= temp;
		collider->localCentroid.y *= temp;
		collider->localCentroid.z *= temp;

		// Calculate the moment of inertia tensor.
		for(i = 0; i < collider->localHull.vertexNum; ++i){

			const float x = collider->localHull.vertices[i].x - collider->localCentroid.x;  /** Is this correct? **/
			const float y = collider->localHull.vertices[i].y - collider->localCentroid.y;
			const float z = collider->localHull.vertices[i].z - collider->localCentroid.z;
			const float sqrX = x*x;
			const float sqrY = y*y;
			const float sqrZ = z*z;
			// xx
			inertiaTensor->m[0][0] += (sqrY + sqrZ);// * collider->localHull.vertices[i].mass;
			// yy
			inertiaTensor->m[1][1] += (sqrX + sqrZ);// * collider->localHull.vertices[i].mass;
			// zz
			inertiaTensor->m[2][2] += (sqrX + sqrY);// * collider->localHull.vertices[i].mass;
			// xy yx
			inertiaTensor->m[0][1] -= x * y;// * collider->localHull.vertices[i].mass;
			// xz zx
			inertiaTensor->m[0][2] -= x * z;// * collider->localHull.vertices[i].mass;
			// yz zy
			inertiaTensor->m[1][2] -= y * z;// * collider->localHull.vertices[i].mass;

		}

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		inertiaTensor->m[0][0] *= avgVertexMass;
		inertiaTensor->m[1][1] *= avgVertexMass;
		inertiaTensor->m[2][2] *= avgVertexMass;
		inertiaTensor->m[0][1] *= avgVertexMass;
		inertiaTensor->m[0][2] *= avgVertexMass;
		inertiaTensor->m[1][2] *= avgVertexMass;

		// No point calculating the same numbers twice.
		inertiaTensor->m[1][0] = inertiaTensor->m[0][1];
		inertiaTensor->m[2][0] = inertiaTensor->m[0][2];
		inertiaTensor->m[2][1] = inertiaTensor->m[1][2];

	}

}

void physRigidBodyGenerateMassProperties(physRigidBody *body, const float *mass, const mat3 *inertiaTensor){

	size_t i;

	vec3SetS(&body->localCentroid, 0.f);
	body->mass = 0.f;

	// Calculate the total, weighted centroid of the body.
	for(i = 0; i < body->colliderNum; ++i){

		body->localCentroid.x += body->colliders[i].localCentroid.x * mass[i];
		body->localCentroid.y += body->colliders[i].localCentroid.y * mass[i];
		body->localCentroid.z += body->colliders[i].localCentroid.z * mass[i];
		body->mass += mass[i];

	}

	body->inverseMass = 1.f / body->mass;
	body->localCentroid.x *= body->inverseMass;
	body->localCentroid.y *= body->inverseMass;
	body->localCentroid.z *= body->inverseMass;


	// Calculate the body's moment of inertia tensor as the sum of its colliders' moments.
	body->localInertiaTensor.m[0][0] = 0.f; body->localInertiaTensor.m[0][1] = 0.f; body->localInertiaTensor.m[0][2] = 0.f;
	body->localInertiaTensor.m[1][0] = 0.f; body->localInertiaTensor.m[1][1] = 0.f; body->localInertiaTensor.m[1][2] = 0.f;
	body->localInertiaTensor.m[2][0] = 0.f; body->localInertiaTensor.m[2][1] = 0.f; body->localInertiaTensor.m[2][2] = 0.f;

	for(i = 0; i < body->colliderNum; ++i){

		body->localInertiaTensor.m[0][0] += inertiaTensor[i].m[0][0];
		body->localInertiaTensor.m[0][1] += inertiaTensor[i].m[0][1];
		body->localInertiaTensor.m[0][2] += inertiaTensor[i].m[0][2];
		body->localInertiaTensor.m[1][0] += inertiaTensor[i].m[1][0];
		body->localInertiaTensor.m[1][1] += inertiaTensor[i].m[1][1];
		body->localInertiaTensor.m[1][2] += inertiaTensor[i].m[1][2];
		body->localInertiaTensor.m[2][0] += inertiaTensor[i].m[2][0];
		body->localInertiaTensor.m[2][1] += inertiaTensor[i].m[2][1];
		body->localInertiaTensor.m[2][2] += inertiaTensor[i].m[2][2];

	}

	//mat3Invert(&body->localInverseInertiaTensor);

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

static void physRigidBodyCentroidFromPosition(physRigidBody *body){
	body->globalCentroid = body->localCentroid;
	quatGetRotatedVec3(&body->orientation, &body->globalCentroid);
	vec3AddVToV(&body->globalCentroid, &body->position);
}

static void physRigidBodyPositionFromCentroid(physRigidBody *body){
	body->position.x = -body->localCentroid.x;
	body->position.y = -body->localCentroid.y;
	body->position.z = -body->localCentroid.z;
	quatGetRotatedVec3(&body->orientation, &body->position);
	vec3AddVToV(&body->position, &body->globalCentroid);
}

static void physRigidBodyGenerateGlobalInertia(physRigidBody *body){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &body->orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &body->localInertiaTensor, &body->globalInverseInertiaTensor);
	mat3MultMByM1(&body->globalInverseInertiaTensor, &inverseOrientationMatrix);

}

void physRigidBodyUpdateCollisionMesh(physRigidBody *body){

	/*
	** Transform the vertices of each body into global space.
	*/

	size_t i, j;
	for(i = 0; i < body->colliderNum; ++i){

		// Update the collider's global centroid.
		body->colliders[i].globalCentroid.x = body->colliders[i].localCentroid.x + body->position.x;
		body->colliders[i].globalCentroid.y = body->colliders[i].localCentroid.y + body->position.y;
		body->colliders[i].globalCentroid.z = body->colliders[i].localCentroid.z + body->position.z;

		for(j = 0; j < body->colliders[i].localHull.vertexNum; ++j){

			// Transform the vertex.
			// Subtract the local centroid from the vertex.
            vec3SubVFromVR(&body->colliders[i].localHull.vertices[j], &body->colliders[i].localCentroid, &body->colliders[i].globalHull.vertices[j]);
			// Rotate the new vertex around (0, 0, 0).
            quatRotateVec3(&body->orientation, &body->colliders[i].globalHull.vertices[j]);
            // Translate it by the global centroid.
            vec3AddVToV(&body->colliders[i].globalHull.vertices[j], &body->colliders[i].globalCentroid);

			// Update mesh minima and maxima.
            if(j == 0){
				// Initialize them to the first vertex.
				body->colliders[i].aabb.left = body->colliders[i].globalHull.vertices[j].x;
				body->colliders[i].aabb.right = body->colliders[i].globalHull.vertices[j].x;
				body->colliders[i].aabb.top = body->colliders[i].globalHull.vertices[j].y;
				body->colliders[i].aabb.bottom = body->colliders[i].globalHull.vertices[j].y;
				body->colliders[i].aabb.front = body->colliders[i].globalHull.vertices[j].z;
				body->colliders[i].aabb.back = body->colliders[i].globalHull.vertices[j].z;
            }else{
            	// Update aabb.left and aabb.right.
				if(body->colliders[i].globalHull.vertices[j].x <= body->colliders[i].aabb.left){
					body->colliders[i].aabb.left = body->colliders[i].globalHull.vertices[j].x;
				}else if(body->colliders[i].globalHull.vertices[j].x > body->colliders[i].aabb.right){
					body->colliders[i].aabb.right = body->colliders[i].globalHull.vertices[j].x;
				}
            	// Update aabb.top and aabb.bottom.
				if(body->colliders[i].globalHull.vertices[j].y >= body->colliders[i].aabb.top){
					body->colliders[i].aabb.top = body->colliders[i].globalHull.vertices[j].y;
				}else if(body->colliders[i].globalHull.vertices[j].y < body->colliders[i].aabb.bottom){
					body->colliders[i].aabb.bottom = body->colliders[i].globalHull.vertices[j].y;
				}
            	// Update aabb.front and aabb.back.
				if(body->colliders[i].globalHull.vertices[j].z >= body->colliders[i].aabb.front){
					body->colliders[i].aabb.front = body->colliders[i].globalHull.vertices[j].z;
				}else if(body->colliders[i].globalHull.vertices[j].z < body->colliders[i].aabb.back){
					body->colliders[i].aabb.back = body->colliders[i].globalHull.vertices[j].z;
				}
            }

		}

		// Update body minima and maxima.
		if(i == 0){
			// Initialize them to the first collider's bounding box.
			body->aabb.left = body->colliders[i].aabb.left;
			body->aabb.right = body->colliders[i].aabb.right;
			body->aabb.top = body->colliders[i].aabb.top;
			body->aabb.bottom = body->colliders[i].aabb.bottom;
			body->aabb.front = body->colliders[i].aabb.front;
			body->aabb.back = body->colliders[i].aabb.back;
		}else{
			// Update aabb.left and aabb.right.
			if(body->colliders[i].aabb.left <= body->aabb.left){
				body->aabb.left = body->colliders[i].aabb.left;
			}else if(body->colliders[i].aabb.right > body->aabb.right){
				body->aabb.right = body->colliders[i].aabb.right;
			}
			// Update aabb.top and aabb.bottom.
			if(body->colliders[i].aabb.top >= body->aabb.top){
				body->aabb.top = body->colliders[i].aabb.top;
			}else if(body->colliders[i].aabb.bottom < body->aabb.bottom){
				body->aabb.bottom = body->colliders[i].aabb.bottom;
			}
			// Update aabb.front and aabb.back.
			if(body->colliders[i].aabb.front >= body->aabb.front){
				body->aabb.front = body->colliders[i].aabb.front;
			}else if(body->colliders[i].aabb.back < body->aabb.back){
				body->aabb.back = body->colliders[i].aabb.back;
			}
		}

	}

}

void physRigidBodyApplyForceAtGlobalPoint(physRigidBody *body, const vec3 *F, const vec3 *r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in world space.
	*/

	/* Accumulate torque. */
	// T = r x F
	vec3 rsR, rxF;
	vec3SubVFromVR(r, &body->globalCentroid, &rsR);
	vec3Cross(&rsR, F, &rxF);
	vec3AddVToV(&body->netTorque, &rxF);

	/* Accumulate force. */
	vec3AddVToV(&body->netForce, F);

}

void physRigidBodyAddLinearVelocity(physRigidBody *body, const vec3 *impulse){
	/* Add to the linear velocity. */
	body->linearVelocity.x += impulse->x;
	body->linearVelocity.y += impulse->y;
	body->linearVelocity.z += impulse->z;
}

void physRigidBodyApplyLinearImpulse(physRigidBody *body, const vec3 *impulse){
	/* Apply a linear impulse. */
	body->linearVelocity.x += impulse->x * body->inverseMass;
	body->linearVelocity.y += impulse->y * body->inverseMass;
	body->linearVelocity.z += impulse->z * body->inverseMass;
}

void physRigidBodyAddAngularVelocity(physRigidBody *body, const float angle, const float x, const float y, const float z){
	/* Add to the angular velocity. */
	/** This is most definitely naive and incorrect. **/
	body->angularVelocity.x += x * angle;
	body->angularVelocity.y += y * angle;
	body->angularVelocity.z += z * angle;
}

static void physRigidBodyResetForceAccumulator(physRigidBody *body){
	body->netForce.x = 0.f;
	body->netForce.y = 0.f;
	body->netForce.z = 0.f;
}

static void physRigidBodyResetTorqueAccumulator(physRigidBody *body){
	body->netTorque.x = 0.f;
	body->netTorque.y = 0.f;
	body->netTorque.z = 0.f;
}

void physRigidBodyIntegrateEuler(physRigidBody *body, const float dt){

	/* Euler integration scheme. */
	const unsigned int steps = PHYS_INTEGRATION_STEPS_EULER;
	const float dtStep = dt*(1.f/PHYS_INTEGRATION_STEPS_EULER);
	vec3 tempVec3;
	float tempFloat;
	quat tempQuat;
	size_t i;

	/* Update moment of inertia. */
	physRigidBodyGenerateGlobalInertia(body);

	for(i = 0; i < steps; ++i){

		/* Calculate linear velocity. */
		// a = F/m
		// dv = a * dt
		body->linearVelocity.x += body->netForce.x * body->inverseMass * dtStep;
		body->linearVelocity.y += body->netForce.y * body->inverseMass * dtStep;
		body->linearVelocity.z += body->netForce.z * body->inverseMass * dtStep;

		/* Update position. */
		body->position.x += body->linearVelocity.x * dtStep;
		body->position.y += body->linearVelocity.y * dtStep;
		body->position.z += body->linearVelocity.z * dtStep;

		/* Calculate angular velocity. */
		tempVec3.x = body->netTorque.x * dtStep;
		tempVec3.y = body->netTorque.y * dtStep;
		tempVec3.z = body->netTorque.z * dtStep;
		mat3MultMByV(&body->globalInverseInertiaTensor, &tempVec3);
		body->angularVelocity.x += tempVec3.x;
		body->angularVelocity.y += tempVec3.y;
		body->angularVelocity.z += tempVec3.z;

		/* Update orientation. */
		// Angle
		tempFloat = vec3GetMagnitude(&body->angularVelocity) * dtStep;
		// Axis
		tempVec3 = body->angularVelocity;
		vec3NormalizeFast(&tempVec3);
		// Convert axis-angle rotation to a quaternion.
		quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
		quatMultQByQ2(&tempQuat, &body->orientation);
		// Normalize the orientation.
		quatNormalizeFast(&body->orientation);

	}

	/* Update global centroid. */
	physRigidBodyCentroidFromPosition(body);

	/* Update constraints. */
	//

	/* Reset force and torque accumulators. */
	body->netForce.x = 0.f;
	body->netForce.y = 0.f;
	body->netForce.z = 0.f;
	body->netTorque.x = 0.f;
	body->netTorque.y = 0.f;
	body->netTorque.z = 0.f;

}

void physRigidBodyIntegrateLeapfrog(physRigidBody *body, const float dt){

	/* Velocity Verlet integration scheme. */
	vec3 tempVec3;
	float tempFloat;
	quat tempQuat;

	/* Integrate position and linear velocity. */
	tempFloat = 0.5f * dt;
	body->linearVelocity.x += body->netForce.x * body->inverseMass * tempFloat;
	body->linearVelocity.y += body->netForce.y * body->inverseMass * tempFloat;
	body->linearVelocity.z += body->netForce.z * body->inverseMass * tempFloat;
	// Kinematic equation for displacement.
	// x(t+1) = x(t) + (v(t) * dt) + ((0.5 * a(t)) * dt * dt)
	body->position.x += body->linearVelocity.x * dt;
	body->position.y += body->linearVelocity.y * dt;
	body->position.z += body->linearVelocity.z * dt;
	/**polygonResetForce(polygon);**/
	// Kinematic equation for velocity.
	// v(t+1) = v(t) + (a(t+1) * dt)
	// v(t+1) = v(t) + (((a(t) + a(t+1)) / 2) * dt)
	body->linearVelocity.x += body->netForce.x * body->inverseMass * tempFloat;
	body->linearVelocity.y += body->netForce.y * body->inverseMass * tempFloat;
	body->linearVelocity.z += body->netForce.z * body->inverseMass * tempFloat;

	/* Update centroid. */
	physRigidBodyCentroidFromPosition(body);

	/* Update moment of inertia. */
	physRigidBodyGenerateGlobalInertia(body);

	/* Calculate angular velocity. */
	tempVec3.x = body->netTorque.x * dt;
	tempVec3.y = body->netTorque.y * dt;
	tempVec3.z = body->netTorque.z * dt;
	mat3MultMByV(&body->globalInverseInertiaTensor, &tempVec3);
	body->angularVelocity.x += tempVec3.x;
	body->angularVelocity.y += tempVec3.y;
	body->angularVelocity.z += tempVec3.z;

	/* Update orientation. */
	// Axis
	tempVec3 = body->angularVelocity;
	vec3NormalizeFast(&tempVec3);
	// Angle
	tempFloat = vec3GetMagnitude(&body->angularVelocity) * dt;
	// Convert axis-angle rotation to a quaternion.
	quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
	quatMultQByQ2(&tempQuat, &body->orientation);
	// Normalize the orientation.
	quatNormalizeFast(&body->orientation);

	/* Update constraints. */
	//

	/* Reset force and torque accumulators. */
	physRigidBodyResetForceAccumulator(body);
	physRigidBodyResetTorqueAccumulator(body);

}

static void physRigidBodyIntegrateRungeKutta(physRigidBody *body, const float dt){
	/* RK4 integration scheme. */

}

void physRigidBodyUpdate(physRigidBody *body, const float dt){

	//

}

void physColliderDelete(physCollider *collider){
	if(collider->localHull.vertices != NULL){
		free(collider->localHull.vertices);
	}
	if(collider->globalHull.vertices != NULL){
		free(collider->globalHull.vertices);
	}
	if(collider->localHull.indices != NULL){
		free(collider->localHull.indices);
	}
}

void physRigidBodyDelete(physRigidBody *body){
	/**if(body->colliders != NULL){
		size_t i;
		for(i = 0; i < body->colliderNum; ++i){
			physColliderDelete(&body->colliders[i]);
		}
		free(body->colliders);
	}
	if(body->constraints != NULL){
		free(body->constraints);
	}**/
}
