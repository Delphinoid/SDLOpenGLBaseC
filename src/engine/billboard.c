#include "billboard.h"
#include "camera.h"
#include "helpersMath.h"
#include "constantsMath.h"

void billboardInit(billboard *const __RESTRICT__ data){
	data->flags = BILLBOARD_DISABLED;
	data->sectors = 0;
	data->axis = NULL;
	data->target = NULL;
	data->scale = 1.f/BILLBOARD_SCALE_CALIBRATION_DISTANCE;
}

mat3x4 billboardState(const billboard data, const camera *const __RESTRICT__ cam, const vec3 centroid, mat3x4 configuration){

	// Generates a billboard transformation matrix.
	// If no flags are set, returns the identity matrix.

	if(flagsAreSet(data.flags, BILLBOARD_INVERT_ORIENTATION)){
		// Reset the basis vectors to the (scaled) standard basis vectors.
		*((vec3 *)&configuration.m[0][0]) = vec3New(vec3Magnitude(*((vec3 *)&configuration.m[0][0])), 0.f, 0.f);
		*((vec3 *)&configuration.m[1][0]) = vec3New(0.f, vec3Magnitude(*((vec3 *)&configuration.m[1][0])), 0.f);
		*((vec3 *)&configuration.m[2][0]) = vec3New(0.f, 0.f, vec3Magnitude(*((vec3 *)&configuration.m[2][0])));
	}

	// Use a faster method for billboarding. Doesn't support individual axis locking.
	if(flagsAreSet(data.flags, BILLBOARD_SPRITE)){

		// Use the camera's X, Y and Z axes for cheap sprite billboarding.
		// This is technically an inverse matrix, so we need rows rather than columns.
		*((vec3 *)&configuration.m[0][0]) = vec3New(cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]);

		if(data.axis != NULL){
			*((vec3 *)&configuration.m[1][0]) = *data.axis;
			///configuration.m[1][3] = 0.f;
		}else{
			*((vec3 *)&configuration.m[1][0]) = vec3New(cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]);
		}

		*((vec3 *)&configuration.m[2][0]) = vec3New(cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);
		///configuration.m[3][3] = 1.f;

		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat3x4ScalePre(configuration, distance, distance, distance);
		}

	}else if(flagsAreSet(data.flags, BILLBOARD_LOCK_XYZ)){

		vec3 eye, target, up;

		///const vec3 translation = *((vec3 *)&configuration.m[3][0]);

		// Set the up vector. This corresponds
		// to the billboard rotation axis.
		if(data.axis != NULL){
			up = *data.axis;
		}else{
			up = cam->up.render;
		}

		// Set the eye and the target.
		if(flagsAreSet(data.flags, BILLBOARD_TARGET_SPRITE)){
			eye = cam->target.render;
			target = cam->position.render;
		}else if(data.target != NULL){
			eye = centroid;
			target = *data.target;
		}else{
			eye = centroid;
			target = cam->position.render;
		}

		// Lock certain axes if needed.
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_X)){
			target.y = eye.y;
		}
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_Y)){
			target.x = eye.x;
		}
		if(flagsAreUnset(data.flags, BILLBOARD_LOCK_Z)){
			up = vec3New(0.f, 1.f, 0.f);
		}

		// Translate the transformation to the "origin".
		configuration = mat3x4Translate(-centroid.x, -centroid.y, -centroid.z, configuration);
		///*((vec3 *)&configuration.m[3][0]) = g_vec3Zero;

		// Scale based on distance if necessary.
		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat3x4ScalePre(configuration, distance, distance, distance);
		}

		// Rotate to face and translate back.
		configuration = mat3x4Translate(
			centroid.x, centroid.y, centroid.z,
			mat3x4MMultM(
				mat3x4RotateToFace(eye, target, up),
				configuration
			)
		);

		// Translate back.
		///*((vec3 *)&configuration.m[3][0]) = translation;

	}else{

		if(flagsAreSet(data.flags, BILLBOARD_SCALE)){
			const float distance = camDistance(cam, centroid) * data.scale;
			configuration = mat3x4ScalePre(configuration, distance, distance, distance);
		}

	}

	return configuration;

}

unsigned int billboardLenticular(const billboard data, const camera *const __RESTRICT__ cam, const mat3x4 configuration){

	if(data.sectors > 1){

		unsigned int frame = 0;

		const float sectorHalf = M_PI / (float)data.sectors;
		const float sector = 2.f*sectorHalf;

		// Normalize the billboard's basis vectors.
		const vec3 billboardUp = vec3NormalizeFast(*((vec3 *)&configuration.m[1][0]));
		const vec3 billboardBackward = vec3Negate(vec3NormalizeFast(*((vec3 *)&configuration.m[2][0])));

		// Project the camera's forward vector onto the plane with normal
		// given by the specified configuration's up basis vector and
		// parallel vector given by the specified configuration's forward
		// basis vector.
		//
		// We do this because we can't guarantee that objects will always
		// be axis-aligned.
		const vec3 projection = vec3NormalizeFast(
			pointPlaneProject(
				billboardUp, billboardBackward,
				vec3VSubV(cam->target.render, cam->position.render)
			)
		);

		// Use the dot and triple products to obtain the signed angle.
		float angle = atan2f(
			vec3Triple(billboardUp, billboardBackward, projection),
			vec3Dot(billboardBackward, projection)
		);

		// Put the angle into the range [0, 2pi].
		if(angle < 0.f){
			// Exit instantly for "forward" facing angles.
			if(angle >= -sectorHalf){
				return 0;
			}
			angle += 2.f*M_PI;
		}else if(angle <= sectorHalf){
			// Exit instantly for "forward" facing angles.
			return 0;
		}

		// Find out which circular sector the angle lies within.
		while(angle > sectorHalf){
			angle -= sector;
			++frame;
		}

		return frame;

	}

	return 0;

}
