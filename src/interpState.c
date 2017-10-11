#include "interpState.h"

void iFloatInit(interpFloat *iFloat, const float v){
	iFloat->previous = v;
	iFloat->render = v;
	iFloat->value = v;
}
void iFloatResetInterp(interpFloat *iFloat){
	iFloat->previous = iFloat->value;
}
unsigned char iFloatUpdate(interpFloat *iFloat, const float interpT){
	if(iFloat->previous == iFloat->value){
		// If the value hasn't changed, there's nothing to interpolate
		if(iFloat->render == iFloat->value){
			return 0;
		}
		iFloat->render = iFloat->value;
	}else{
		iFloat->render = iFloat->previous +
		                 (iFloat->value - iFloat->previous) *
		                 interpT;
	}
	return 1;
}

void iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z){
	vec3Set(&iVec3->previous, x, y, z);
	vec3Set(&iVec3->render, x, y, z);
	vec3Set(&iVec3->value, x, y, z);
}
void iVec3ResetInterp(interpVec3 *iVec3){
	iVec3->previous = iVec3->value;
}
unsigned char iVec3Update(interpVec3 *iVec3, const float interpT){
	if(iVec3->previous.x == iVec3->value.x &&
	   iVec3->previous.y == iVec3->value.y &&
	   iVec3->previous.z == iVec3->value.z){
		// If the value hasn't changed, there's nothing to interpolate
		if(iVec3->render.x == iVec3->value.x &&
		   iVec3->render.y == iVec3->value.y &&
		   iVec3->render.z == iVec3->value.z){
			return 0;
		}
		iVec3->render = iVec3->value;
	}else{
		vec3Lerp(&iVec3->previous,
		         &iVec3->value,
		         interpT,
		         &iVec3->render);
	}
	return 1;
}

void iQuatInit(interpQuat *iQuat){
	quatSetIdentity(&iQuat->previous);
	quatSetIdentity(&iQuat->render);
	quatSetIdentity(&iQuat->value);
}
void iQuatResetInterp(interpQuat *iQuat){
	iQuat->previous = iQuat->value;
}
unsigned char iQuatUpdate(interpQuat *iQuat, const float interpT){
	if(iQuat->previous.w   == iQuat->value.w   &&
	   iQuat->previous.v.x == iQuat->value.v.x &&
	   iQuat->previous.v.y == iQuat->value.v.y &&
	   iQuat->previous.v.z == iQuat->value.v.z){
		// If the value hasn't changed, there's nothing to interpolate
		if(iQuat->render.w   == iQuat->value.w   &&
		   iQuat->render.v.x == iQuat->value.v.x &&
		   iQuat->render.v.y == iQuat->value.v.y &&
		   iQuat->render.v.z == iQuat->value.v.z){
			return 0;
		}
		iQuat->render = iQuat->value;
	}else{
		quatSlerp(&iQuat->previous,
		          &iQuat->value,
		          interpT,
		          &iQuat->render);
	}
	return 1;
}
