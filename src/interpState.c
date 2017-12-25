#include "interpState.h"
#include <string.h>

/*
** INTERPOLATED FLOAT FUNCTIONS
*/
void iFloatInit(interpFloat *iFloat, const float s){
	iFloat->previous = s;
	iFloat->value = s;
	iFloat->render = s;
}
void iFloatResetInterp(interpFloat *iFloat){
	iFloat->previous = iFloat->value;
}
unsigned char iFloatUpdate(interpFloat *iFloat, const float interpT){
	if(interpT == 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iFloat->render == iFloat->previous){
			return 0;
		}
		iFloat->render = iFloat->previous;
	}else if(interpT == 1.f || iFloat->previous == iFloat->value){
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
/*
void iFloatInterpolate(const interpFloat *iFloat, const float interpT, float *r){
	if(iFloat->previous == iFloat->value){
		*r = iFloat->value;
	}else{
		*r = iFloat->previous + (iFloat->value - iFloat->previous) * interpT;
	}
	return 1;
}
*/




/*
** INTERPOLATED SIZE_T FUNCTIONS
*/
void iSizeTInit(interpSizeT *iSizeT, const size_t s){
	iSizeT->previous = s;
	iSizeT->value = s;
	iSizeT->render = s;
}
void iSizeTResetInterp(interpSizeT *iSizeT){
	iSizeT->previous = iSizeT->value;
}
unsigned char iSizeTUpdate(interpSizeT *iSizeT, const float interpT){
	if(interpT == 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iSizeT->render == iSizeT->previous){
			return 0;
		}
		iSizeT->render = iSizeT->previous;
	}else if(interpT == 1.f || iSizeT->previous == iSizeT->value){
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
/*
void iSizeTInterpolate(const interpSizeT *iSizeT, const float interpT, size_t *r){
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
*/




/*
** INTERPOLATED 3D VECTOR FUNCTIONS
*/
void iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z){
	vec3Set(&iVec3->previous, x, y, z);
	vec3Set(&iVec3->value,    x, y, z);
	vec3Set(&iVec3->render,   x, y, z);
}
void iVec3ResetInterp(interpVec3 *iVec3){
	iVec3->previous = iVec3->value;
}
unsigned char iVec3Update(interpVec3 *iVec3, const float interpT){
	if(interpT == 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iVec3->render.x == iVec3->previous.x &&
		   iVec3->render.y == iVec3->previous.y &&
		   iVec3->render.z == iVec3->previous.z){
			return 0;
		}
		iVec3->render = iVec3->previous;
	}else if(interpT == 1.f ||
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
		vec3Lerp(&iVec3->previous,
		         &iVec3->value,
		         interpT,
		         &iVec3->render);
	}
	return 1;
}
/*
void iVec3Interpolate(const interpVec3 *iVec3, const float interpT, vec3 *r){
	if(iVec3->previous.x == iVec3->value.x &&
	   iVec3->previous.y == iVec3->value.y &&
	   iVec3->previous.z == iVec3->value.z){
		*r = iVec3->value;
	}else{
		vec3Lerp(&iVec3->previous,
		         &iVec3->value,
		         interpT, r);
	}
	return 1;
}
*/




/*
** INTERPOLATED QUATERNION FUNCTIONS
*/
void iQuatInit(interpQuat *iQuat){
	quatSetIdentity(&iQuat->previous);
	quatSetIdentity(&iQuat->value);
	quatSetIdentity(&iQuat->render);
}
void iQuatResetInterp(interpQuat *iQuat){
	iQuat->previous = iQuat->value;
}
unsigned char iQuatUpdate(interpQuat *iQuat, const float interpT){
	if(interpT == 0.f){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iQuat->render.w   == iQuat->previous.w   &&
		   iQuat->render.v.x == iQuat->previous.v.x &&
		   iQuat->render.v.y == iQuat->previous.v.y &&
		   iQuat->render.v.z == iQuat->previous.v.z){
			return 0;
		}
		iQuat->render = iQuat->previous;
	}else if(interpT == 1.f ||
			 (iQuat->previous.w   == iQuat->value.w   &&
	          iQuat->previous.v.x == iQuat->value.v.x &&
	          iQuat->previous.v.y == iQuat->value.v.y &&
	          iQuat->previous.v.z == iQuat->value.v.z)){
		// If the value hasn't changed, there's nothing to interpolate.
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
/*
void iQuatInterpolate(const interpQuat *iQuat, const float interpT, quat *r){
	if(iQuat->previous.w   == iQuat->value.w   &&
	   iQuat->previous.v.x == iQuat->value.v.x &&
	   iQuat->previous.v.y == iQuat->value.v.y &&
	   iQuat->previous.v.z == iQuat->value.v.z){
		*r = iQuat->value;
	}else{
		quatSlerp(&iQuat->previous,
		          &iQuat->value,
		          interpT, r);
	}
	return 1;
}
*/

