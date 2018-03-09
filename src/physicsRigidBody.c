#include "physicsRigidBody.h"

#define PHYS_INTEGRATION_STEPS_EULER 2
#define PHYS_INTEGRATION_STEPS_RUNGEKUTTA 4

void physColliderGenerateMassProperties(physCollider *collider, const float mass, mat3 *inertiaTensor){

	vec3 *v0;
	vec3 *v1;
	float temp;
	float doubleVolume = 0.f;
	size_t i;

	vec3SetS(&collider->centroid, 0.f);
	inertiaTensor->m[0][0] = 0.f; inertiaTensor->m[0][1] = 0.f; inertiaTensor->m[0][2] = 0.f;
	inertiaTensor->m[1][0] = 0.f; inertiaTensor->m[1][1] = 0.f; inertiaTensor->m[1][2] = 0.f;
	inertiaTensor->m[2][0] = 0.f; inertiaTensor->m[2][1] = 0.f; inertiaTensor->m[2][2] = 0.f;

	if(collider->hull.vertexNum > 0){

		/** Pass in an array of vertex masses? Probably dumb. **/
		const float avgVertexMass = mass / collider->hull.vertexNum;

		// Recursively calculate the center of mass.
		for(i = 0; i < collider->hull.vertexNum; ++i){

			v0 = &collider->hull.vertices[i-1];
			v1 = &collider->hull.vertices[i];
			temp = v0->x * v1->y - v0->y * v1->x;
			collider->centroid.x += (v0->x + v1->x) * temp;
			collider->centroid.y += (v0->y + v1->y) * temp;
			collider->centroid.z += (v0->z + v1->z) * temp;
			doubleVolume += temp;
		}

		// Final iteration with the last and first vertices.
		v0 = &collider->hull.vertices[collider->hull.vertexNum-1];
		v1 = &collider->hull.vertices[0];
		temp = v0->x * v1->y - v0->y * v1->x;
		collider->centroid.x += (v0->x + v1->x) * temp;
		collider->centroid.y += (v0->y + v1->y) * temp;
		collider->centroid.z += (v0->z + v1->z) * temp;
		doubleVolume += temp;

		// Calculate the mesh's final center of mass.
		temp = 1.f / (3.f * doubleVolume);
		collider->centroid.x *= temp;
		collider->centroid.y *= temp;
		collider->centroid.z *= temp;

		// Initialize the AABB to the first vertex.
		collider->aabb.left = collider->hull.vertices[0].x;
		collider->aabb.right = collider->hull.vertices[0].x;
		collider->aabb.top = collider->hull.vertices[0].y;
		collider->aabb.bottom = collider->hull.vertices[0].y;
		collider->aabb.front = collider->hull.vertices[0].z;
		collider->aabb.back = collider->hull.vertices[0].z;

		// Calculate the moment of inertia tensor and the AABB.
		for(i = 0; i < collider->hull.vertexNum; ++i){

			const float x = collider->hull.vertices[i].x - collider->centroid.x;  /** Is this correct? **/
			const float y = collider->hull.vertices[i].y - collider->centroid.y;
			const float z = collider->hull.vertices[i].z - collider->centroid.z;
			const float sqrX = x*x;
			const float sqrY = y*y;
			const float sqrZ = z*z;
			// xx
			inertiaTensor->m[0][0] += (sqrY + sqrZ);// * collider->hull.vertices[i].mass;
			// yy
			inertiaTensor->m[1][1] += (sqrX + sqrZ);// * collider->hull.vertices[i].mass;
			// zz
			inertiaTensor->m[2][2] += (sqrX + sqrY);// * collider->hull.vertices[i].mass;
			// xy yx
			inertiaTensor->m[0][1] -= x * y;// * collider->hull.vertices[i].mass;
			// xz zx
			inertiaTensor->m[0][2] -= x * z;// * collider->hull.vertices[i].mass;
			// yz zy
			inertiaTensor->m[1][2] -= y * z;// * collider->hull.vertices[i].mass;

			// Update aabb.left and aabb.right.
			if(collider->hull.vertices[i].x <= collider->aabb.left){
				collider->aabb.left = collider->hull.vertices[i].x;
			}else if(collider->hull.vertices[i].x > collider->aabb.right){
				collider->aabb.right = collider->hull.vertices[i].x;
			}
			// Update aabb.top and aabb.bottom.
			if(collider->hull.vertices[i].y >= collider->aabb.top){
				collider->aabb.top = collider->hull.vertices[i].y;
			}else if(collider->hull.vertices[i].y < collider->aabb.bottom){
				collider->aabb.bottom = collider->hull.vertices[i].y;
			}
			// Update aabb.front and aabb.back.
			if(collider->hull.vertices[i].z >= collider->aabb.front){
				collider->aabb.front = collider->hull.vertices[i].z;
			}else if(collider->hull.vertices[i].z < collider->aabb.back){
				collider->aabb.back = collider->hull.vertices[i].z;
			}

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

void physColliderDelete(physCollider *collider){
	if(collider->hull.vertices != NULL){
		free(collider->hull.vertices);
	}
	if(collider->hull.indices != NULL){
		free(collider->hull.indices);
	}
}

void physRigidBodyGenerateMassProperties(physRigidBody *body, const float *mass, const mat3 *inertiaTensor){

	size_t i;

	vec3SetS(&body->centroid, 0.f);
	body->mass = 0.f;

	// Calculate the total, weighted centroid of the body.
	for(i = 0; i < body->colliderNum; ++i){

		body->centroid.x += body->colliders[i].centroid.x * mass[i];
		body->centroid.y += body->colliders[i].centroid.y * mass[i];
		body->centroid.z += body->colliders[i].centroid.z * mass[i];
		body->mass += mass[i];

	}

	body->inverseMass = 1.f / body->mass;
	body->centroid.x *= body->inverseMass;
	body->centroid.y *= body->inverseMass;
	body->centroid.z *= body->inverseMass;


	// Calculate the body's moment of inertia tensor as the sum of its colliders' moments.
	body->inertiaTensor.m[0][0] = 0.f; body->inertiaTensor.m[0][1] = 0.f; body->inertiaTensor.m[0][2] = 0.f;
	body->inertiaTensor.m[1][0] = 0.f; body->inertiaTensor.m[1][1] = 0.f; body->inertiaTensor.m[1][2] = 0.f;
	body->inertiaTensor.m[2][0] = 0.f; body->inertiaTensor.m[2][1] = 0.f; body->inertiaTensor.m[2][2] = 0.f;

	for(i = 0; i < body->colliderNum; ++i){

		body->inertiaTensor.m[0][0] += inertiaTensor[i].m[0][0];
		body->inertiaTensor.m[0][1] += inertiaTensor[i].m[0][1];
		body->inertiaTensor.m[0][2] += inertiaTensor[i].m[0][2];
		body->inertiaTensor.m[1][0] += inertiaTensor[i].m[1][0];
		body->inertiaTensor.m[1][1] += inertiaTensor[i].m[1][1];
		body->inertiaTensor.m[1][2] += inertiaTensor[i].m[1][2];
		body->inertiaTensor.m[2][0] += inertiaTensor[i].m[2][0];
		body->inertiaTensor.m[2][1] += inertiaTensor[i].m[2][1];
		body->inertiaTensor.m[2][2] += inertiaTensor[i].m[2][2];

	}

	//mat3Invert(&body->localInverseInertiaTensor);

}

void physRigidBodyDelete(physRigidBody *body){
	//
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

static void prbiCentroidFromPosition(prbInstance *prbi){
	prbi->centroid = prbi->local->centroid;
	quatGetRotatedVec3(&prbi->configuration.orientation, &prbi->centroid);
	vec3AddVToV(&prbi->centroid, &prbi->configuration.position);
}

static void prbiPositionFromCentroid(prbInstance *prbi){
	prbi->configuration.position.x = -prbi->local->centroid.x;
	prbi->configuration.position.y = -prbi->local->centroid.y;
	prbi->configuration.position.z = -prbi->local->centroid.z;
	quatGetRotatedVec3(&prbi->configuration.orientation, &prbi->configuration.position);
	vec3AddVToV(&prbi->configuration.position, &prbi->centroid);
}

static void prbiGenerateGlobalInertia(prbInstance *prbi){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &prbi->configuration.orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &prbi->local->inertiaTensor, &prbi->inverseInertiaTensor);
	mat3MultMByM1(&prbi->inverseInertiaTensor, &inverseOrientationMatrix);

}

void prbiUpdateCollisionMesh(prbInstance *prbi){

	/*
	** Transform the vertices of each body into global space.
	*/

	size_t i, j;
	for(i = 0; i < prbi->local->colliderNum; ++i){

		// Update the collider's global centroid.
		prbi->colliders[i].centroid.x = prbi->local->colliders[i].centroid.x + prbi->configuration.position.x;
		prbi->colliders[i].centroid.y = prbi->local->colliders[i].centroid.y + prbi->configuration.position.y;
		prbi->colliders[i].centroid.z = prbi->local->colliders[i].centroid.z + prbi->configuration.position.z;

		for(j = 0; j < prbi->colliders[i].hull.vertexNum; ++j){

			// Transform the vertex.
			// Subtract the local centroid from the vertex.
			vec3SubVFromVR(&prbi->local->colliders[i].hull.vertices[j], &prbi->local->colliders[i].centroid, &prbi->colliders[i].hull.vertices[j]);
			// Rotate the new vertex around (0, 0, 0).
			quatRotateVec3(&prbi->configuration.orientation, &prbi->colliders[i].hull.vertices[j]);
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

void prbiApplyForceAtGlobalPoint(prbInstance *prbi, const vec3 *F, const vec3 *r){

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

void prbiAddLinearVelocity(prbInstance *prbi, const vec3 *impulse){
	/* Add to the linear velocity. */
	prbi->linearVelocity.x += impulse->x;
	prbi->linearVelocity.y += impulse->y;
	prbi->linearVelocity.z += impulse->z;
}

void prbiApplyLinearImpulse(prbInstance *prbi, const vec3 *impulse){
	/* Apply a linear impulse. */
	prbi->linearVelocity.x += impulse->x * prbi->local->inverseMass;
	prbi->linearVelocity.y += impulse->y * prbi->local->inverseMass;
	prbi->linearVelocity.z += impulse->z * prbi->local->inverseMass;
}

void prbiAddAngularVelocity(prbInstance *prbi, const float angle, const float x, const float y, const float z){
	/* Add to the angular velocity. */
	/** This is most definitely naive and incorrect. **/
	prbi->angularVelocity.x += x * angle;
	prbi->angularVelocity.y += y * angle;
	prbi->angularVelocity.z += z * angle;
}

static void prbiResetForceAccumulator(prbInstance *prbi){
	prbi->netForce.x = 0.f;
	prbi->netForce.y = 0.f;
	prbi->netForce.z = 0.f;
}

static void prbiResetTorqueAccumulator(prbInstance *prbi){
	prbi->netTorque.x = 0.f;
	prbi->netTorque.y = 0.f;
	prbi->netTorque.z = 0.f;
}

void prbiIntegrateEuler(prbInstance *prbi, const float dt){

	/* Euler integration scheme. */
	const unsigned int steps = PHYS_INTEGRATION_STEPS_EULER;
	const float dtStep = dt*(1.f/PHYS_INTEGRATION_STEPS_EULER);
	vec3 tempVec3;
	float tempFloat;
	quat tempQuat;
	size_t i;

	/* Update moment of inertia. */
	prbiGenerateGlobalInertia(prbi);

	for(i = 0; i < steps; ++i){

		/* Calculate linear velocity. */
		// a = F/m
		// dv = a * dt
		prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * dtStep;
		prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * dtStep;
		prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * dtStep;

		/* Update position. */
		prbi->configuration.position.x += prbi->linearVelocity.x * dtStep;
		prbi->configuration.position.y += prbi->linearVelocity.y * dtStep;
		prbi->configuration.position.z += prbi->linearVelocity.z * dtStep;

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
		quatMultQByQ2(&tempQuat, &prbi->configuration.orientation);
		// Normalize the orientation.
		quatNormalizeFast(&prbi->configuration.orientation);

	}

	/* Update global centroid. */
	prbiCentroidFromPosition(prbi);

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

void prbiIntegrateLeapfrog(prbInstance *prbi, const float dt){

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
	prbi->configuration.position.x += prbi->linearVelocity.x * dt;
	prbi->configuration.position.y += prbi->linearVelocity.y * dt;
	prbi->configuration.position.z += prbi->linearVelocity.z * dt;
	/**polygonResetForce(polygon);**/
	// Kinematic equation for velocity.
	// v(t+1) = v(t) + (a(t+1) * dt)
	// v(t+1) = v(t) + (((a(t) + a(t+1)) / 2) * dt)
	prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * tempFloat;
	prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * tempFloat;
	prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * tempFloat;

	/* Update centroid. */
	prbiCentroidFromPosition(prbi);

	/* Update moment of inertia. */
	prbiGenerateGlobalInertia(prbi);

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
	quatMultQByQ2(&tempQuat, &prbi->configuration.orientation);
	// Normalize the orientation.
	quatNormalizeFast(&prbi->configuration.orientation);

	/* Update constraints. */
	//

	/* Reset force and torque accumulators. */
	prbiResetForceAccumulator(prbi);
	prbiResetTorqueAccumulator(prbi);

}

static void prbiIntegrateRungeKutta(prbInstance *prbi, const float dt){
	/* RK4 integration scheme. */

}

void prbiUpdate(prbInstance *prbi, const float dt){

	//

}

void prbiDelete(prbInstance *prbi){
	size_t i;
	if(prbi->colliders != NULL){
		for(i = 0; i < prbi->local->colliderNum; ++i){
			// Only free vertices, as we re-use indices from the local collider's hull.
			free(prbi->colliders[i].hull.vertices);
		}
	}
	if(prbi->constraints != NULL){
		free(prbi->constraints);
	}
}
