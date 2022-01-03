#include "interpState.h"
#include <string.h>

// INTERPOLATED FLOAT FUNCTIONS
void iFloatInit(interpFloat *const __RESTRICT__ iFloat, const float s){
	iFloat->previous = s;
	iFloat->value = s;
	iFloat->render = s;
}
void iFloatResetInterp(interpFloat *const __RESTRICT__ iFloat){
	iFloat->previous = iFloat->value;
}
return_t iFloatUpdate(interpFloat *const __RESTRICT__ iFloat, const float interpT){
	if(interpT <= 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iFloat->render == iFloat->previous){
			return 0;
		}
		iFloat->render = iFloat->previous;
	}else if(interpT >= 1.f || iFloat->previous == iFloat->value){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iFloat->render == iFloat->value){
			return 0;
		}
		iFloat->render = iFloat->value;
	}else{
		iFloat->render = iFloat->previous + (iFloat->value - iFloat->previous) * interpT;
	}
	return 1;
}
/**
void iFloatInterpolate(const interpFloat *const __RESTRICT__ iFloat, const float interpT, float *const __RESTRICT__ r){
	if(iFloat->previous == iFloat->value){
		*r = iFloat->value;
	}else{
		*r = iFloat->previous + (iFloat->value - iFloat->previous) * interpT;
	}
	return 1;
}
**/




// INTERPOLATED SIZE_T FUNCTIONS
void iSizeTInit(interpSizeT *const __RESTRICT__ iSizeT, const size_t s){
	iSizeT->previous = s;
	iSizeT->value = s;
	iSizeT->render = s;
}
void iSizeTResetInterp(interpSizeT *const __RESTRICT__ iSizeT){
	iSizeT->previous = iSizeT->value;
}
return_t iSizeTUpdate(interpSizeT *const __RESTRICT__ iSizeT, const float interpT){
	if(interpT <= 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iSizeT->render == iSizeT->previous){
			return 0;
		}
		iSizeT->render = iSizeT->previous;
	}else if(interpT >= 1.f || iSizeT->previous == iSizeT->value){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iSizeT->render == iSizeT->value){
			return 0;
		}
		iSizeT->render = iSizeT->value;
	}
	if(interpT < 1){
		iSizeT->render = iSizeT->previous;
	}else{
		iSizeT->render = iSizeT->value;
	}
	return 1;
}
/**
void iSizeTInterpolate(const interpSizeT *const __RESTRICT__ iSizeT, const float interpT, size_t *const __RESTRICT__ r){
	if(iSizeT->previous == iSizeT->value){
		*r = iSizeT->value;
	}
	if(interpT < 1){
		*r = iSizeT->previous;
	}else{
		*r = iSizeT->value;
	}
	return 1;
}
**/




// INTERPOLATED 3D VECTOR FUNCTIONS
void iVec3Init(interpVec3 *const __RESTRICT__ iVec3, const float x, const float y, const float z){
	iVec3->previous = vec3New(x, y, z);
	iVec3->value    = vec3New(x, y, z);
	iVec3->render   = vec3New(x, y, z);
}
void iVec3ResetInterp(interpVec3 *const __RESTRICT__ iVec3){
	iVec3->previous = iVec3->value;
}
return_t iVec3Update(interpVec3 *const __RESTRICT__ iVec3, const float interpT){
	if(interpT <= 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iVec3->render.x == iVec3->previous.x &&
		   iVec3->render.y == iVec3->previous.y &&
		   iVec3->render.z == iVec3->previous.z){
			return 0;
		}
		iVec3->render = iVec3->previous;
	}else if(interpT >= 1.f ||
			 (iVec3->previous.x == iVec3->value.x &&
	          iVec3->previous.y == iVec3->value.y &&
	          iVec3->previous.z == iVec3->value.z)){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iVec3->render.x == iVec3->value.x &&
		   iVec3->render.y == iVec3->value.y &&
		   iVec3->render.z == iVec3->value.z){
			return 0;
		}
		iVec3->render = iVec3->value;
	}else{
		iVec3->render = vec3Lerp(iVec3->previous, iVec3->value, interpT);
	}
	return 1;
}
/**
void iVec3Interpolate(const interpVec3 *const __RESTRICT__ iVec3, const float interpT, vec3 *const __RESTRICT__ r){
	if(iVec3->previous.x == iVec3->value.x &&
	   iVec3->previous.y == iVec3->value.y &&
	   iVec3->previous.z == iVec3->value.z){
		*r = iVec3->value;
	}else{
		vec3LerpR(&iVec3->previous,
		          &iVec3->value,
		          interpT, r);
	}
	return 1;
}
**/




// INTERPOLATED QUATERNION FUNCTIONS
void iQuatInit(interpQuat *const __RESTRICT__ iQuat){
	iQuat->previous = g_quatIdentity;
	iQuat->value = g_quatIdentity;
	iQuat->render = g_quatIdentity;
}
void iQuatResetInterp(interpQuat *const __RESTRICT__ iQuat){
	iQuat->previous = iQuat->value;
}
return_t iQuatUpdate(interpQuat *const __RESTRICT__ iQuat, const float interpT){
	if(interpT <= 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iQuat->render.x == iQuat->previous.x &&
		   iQuat->render.y == iQuat->previous.y &&
		   iQuat->render.z == iQuat->previous.z &&
		   iQuat->render.w == iQuat->previous.w){
			return 0;
		}
		iQuat->render = iQuat->previous;
	}else if(interpT >= 1.f ||
			 (iQuat->previous.x == iQuat->value.x &&
	          iQuat->previous.y == iQuat->value.y &&
	          iQuat->previous.z == iQuat->value.z &&
	          iQuat->previous.w == iQuat->value.w)){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iQuat->render.x == iQuat->value.x &&
		   iQuat->render.y == iQuat->value.y &&
		   iQuat->render.z == iQuat->value.z &&
		   iQuat->render.w == iQuat->value.w){
			return 0;
		}
		iQuat->render = iQuat->value;
	}else{
		iQuat->render = quatSlerpFast(iQuat->previous, iQuat->value, interpT);
	}
	return 1;
}
/**
void iQuatInterpolate(const interpQuat *const __RESTRICT__ iQuat, const float interpT, quat *const __RESTRICT__ r){
	if(iQuat->previous.v.x == iQuat->value.v.x &&
	   iQuat->previous.v.y == iQuat->value.v.y &&
	   iQuat->previous.v.z == iQuat->value.v.z &&
	   iQuat->previous.w   == iQuat->value.w){
		*r = iQuat->value;
	}else{
		quatSlerpFastR(&iQuat->previous,
		           &iQuat->value,
		           interpT, r);
	}
	return 1;
}
**/
