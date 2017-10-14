#include "interpState.h"
#include <string.h>

unsigned char iFloatInit(interpFloat *iFloat, const float s, size_t stateNum){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iFloat->render);
		iFloat->previous = malloc(bytes);
		if(iFloat->previous != NULL){
			iFloat->value = malloc(bytes);
			if(iFloat->value != NULL){
				// Initialize stateNum states.
				while(stateNum > 0){
					--stateNum;
					iFloat->previous[stateNum] = s;
					iFloat->value[stateNum] = s;
				}
				iFloat->render = s;
				return 1;
			}
			free(iFloat->previous);
		}
	}
	return 0;
}
unsigned char iFloatResize(interpFloat *iFloat, const size_t oldStateNum, size_t newStateNum){
	if(oldStateNum > 0 && newStateNum > 0){
		const size_t bytes = newStateNum * sizeof(iFloat->render);
		float *tempPreviousBuffer = malloc(bytes);
		if(tempPreviousBuffer != NULL){
			float *tempValueBuffer = malloc(bytes);
			if(tempValueBuffer != NULL){
				if(newStateNum < oldStateNum){
					// New state vectors are smaller than the old ones.
					memcpy(tempPreviousBuffer, iFloat->previous, bytes);
					memcpy(tempValueBuffer,    iFloat->value,    bytes);
				}else{
					const size_t bytesCopy = oldStateNum * sizeof(iFloat->render);
					memcpy(tempPreviousBuffer, iFloat->previous, bytesCopy);
					memcpy(tempValueBuffer,    iFloat->value,    bytesCopy);
					// Pad extra values with the last valid values.
					while(newStateNum > oldStateNum){
						--newStateNum;
						tempPreviousBuffer[newStateNum] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[newStateNum]    = tempValueBuffer[oldStateNum-1];
					}

				}
				free(iFloat->previous);
				free(iFloat->value);
				iFloat->previous = tempPreviousBuffer;
				iFloat->value    = tempValueBuffer;
				return 1;
			}
			free(tempPreviousBuffer);
		}
	}
	return 0;
}
void iFloatSkipCurrentInterp(interpFloat *iFloat){
	iFloat->previous[0] = iFloat->value[0];
}
void iFloatResetInterp(interpFloat *iFloat, const size_t stateNum){
	size_t i;
	for(i = 1; i < stateNum; ++i){
		// Shift each value over to make room for the new one.
		iFloat->previous[i] = iFloat->value[i-1];
		iFloat->value[i] = iFloat->value[i-1];
	}
	iFloat->previous[0] = iFloat->value[0];
}
void iFloatPreviousState(interpFloat *iFloat, float *value, const size_t state, const float interpT){
	if(iFloat->previous[state] == iFloat->value[state]){
		// If the value hasn't changed, there's nothing to interpolate
		*value = iFloat->value[state];
	}else{
		*value = iFloat->previous[state] +
		         (iFloat->value[state] - iFloat->previous[state]) *
		         interpT;
	}
}
unsigned char iFloatUpdate(interpFloat *iFloat, const float interpT){
	if(iFloat->previous[0] == iFloat->value[0]){
		// If the value hasn't changed, there's nothing to interpolate
		if(iFloat->render == iFloat->value[0]){
			return 0;
		}
		iFloat->render = iFloat->value[0];
	}else{
		iFloat->render = iFloat->previous[0] +
		                 (iFloat->value[0] - iFloat->previous[0]) *
		                 interpT;
	}
	return 1;
}




unsigned char iVec3Init(interpVec3 *iVec3, const float x, const float y, const float z, size_t stateNum){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iVec3->render);
		iVec3->previous = malloc(bytes);
		if(iVec3->previous != NULL){
			iVec3->value = malloc(bytes);
			if(iVec3->value != NULL){
				// Initialize stateNum states.
				while(stateNum > 0){
					--stateNum;
					vec3Set(&iVec3->previous[stateNum], x, y, z);
					vec3Set(&iVec3->value[stateNum],    x, y, z);
				}
				vec3Set(&iVec3->render, x, y, z);
				return 1;
			}
			free(iVec3->previous);
		}
	}
	return 0;
}
unsigned char iVec3Resize(interpVec3 *iVec3, const size_t oldStateNum, size_t newStateNum){
	if(oldStateNum > 0 && newStateNum > 0){
		const size_t bytes = newStateNum * sizeof(iVec3->render);
		vec3 *tempPreviousBuffer = malloc(bytes);
		if(tempPreviousBuffer != NULL){
			vec3 *tempValueBuffer = malloc(bytes);
			if(tempValueBuffer != NULL){
				if(newStateNum < oldStateNum){
					// New state vectors are smaller than the old ones.
					memcpy(tempPreviousBuffer, iVec3->previous, bytes);
					memcpy(tempValueBuffer,    iVec3->value,    bytes);
				}else{
					const size_t bytesCopy = oldStateNum * sizeof(iVec3->render);
					memcpy(tempPreviousBuffer, iVec3->previous, bytesCopy);
					memcpy(tempValueBuffer,    iVec3->value,    bytesCopy);
					// Pad extra values with the last valid values.
					while(newStateNum > oldStateNum){
						--newStateNum;
						tempPreviousBuffer[newStateNum] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[newStateNum]    = tempValueBuffer[oldStateNum-1];
					}

				}
				free(iVec3->previous);
				free(iVec3->value);
				iVec3->previous = tempPreviousBuffer;
				iVec3->value    = tempValueBuffer;
				return 1;
			}
			free(tempPreviousBuffer);
		}
	}
	return 0;
}
void iVec3SkipCurrentInterp(interpVec3 *iVec3){
	iVec3->previous[0] = iVec3->value[0];
}
void iVec3ResetInterp(interpVec3 *iVec3, const size_t stateNum){
	size_t i;
	for(i = 1; i < stateNum; ++i){
		// Shift each value over to make room for the new one.
		iVec3->previous[i] = iVec3->value[i-1];
		iVec3->value[i] = iVec3->value[i-1];
	}
	iVec3->previous[0] = iVec3->value[0];
}
void iVec3PreviousState(interpVec3 *iVec3, vec3 *value, const size_t state, const float interpT){
	if(iVec3->previous[0].x == iVec3->value[0].x &&
	   iVec3->previous[0].y == iVec3->value[0].y &&
	   iVec3->previous[0].z == iVec3->value[0].z){
		// If the value hasn't changed, there's nothing to interpolate
		*value = iVec3->value[state];
	}else{
		vec3Lerp(&iVec3->previous[state],
		         &iVec3->value[state],
		         interpT,
		         value);
	}
}
unsigned char iVec3Update(interpVec3 *iVec3, const float interpT){
	if(iVec3->previous[0].x == iVec3->value[0].x &&
	   iVec3->previous[0].y == iVec3->value[0].y &&
	   iVec3->previous[0].z == iVec3->value[0].z){
		// If the value hasn't changed, there's nothing to interpolate
		if(iVec3->render.x == iVec3->value[0].x &&
		   iVec3->render.y == iVec3->value[0].y &&
		   iVec3->render.z == iVec3->value[0].z){
			return 0;
		}
		iVec3->render = iVec3->value[0];
	}else{
		vec3Lerp(&iVec3->previous[0],
		         &iVec3->value[0],
		         interpT,
		         &iVec3->render);
	}
	return 1;
}




unsigned char iQuatInit(interpQuat *iQuat, size_t stateNum){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iQuat->render);
		iQuat->previous = malloc(bytes);
		if(iQuat->previous != NULL){
			iQuat->value = malloc(bytes);
			if(iQuat->value != NULL){
				// Initialize stateNum states.
				while(stateNum > 0){
					--stateNum;
					quatSetIdentity(&iQuat->previous[stateNum]);
					quatSetIdentity(&iQuat->value[stateNum]);
				}
				quatSetIdentity(&iQuat->render);
				return 1;
			}
			free(iQuat->previous);
		}
	}
	return 0;
}
unsigned char iQuatResize(interpQuat *iQuat, const size_t oldStateNum, size_t newStateNum){
	if(oldStateNum > 0 && newStateNum > 0){
		const size_t bytes = newStateNum * sizeof(iQuat->render);
		quat *tempPreviousBuffer = malloc(bytes);
		if(tempPreviousBuffer != NULL){
			quat *tempValueBuffer = malloc(bytes);
			if(tempValueBuffer != NULL){
				if(newStateNum < oldStateNum){
					// New state vectors are smaller than the old ones.
					memcpy(tempPreviousBuffer, iQuat->previous, bytes);
					memcpy(tempValueBuffer,    iQuat->value,    bytes);
				}else{
					const size_t bytesCopy = oldStateNum * sizeof(iQuat->render);
					memcpy(tempPreviousBuffer, iQuat->previous, bytesCopy);
					memcpy(tempValueBuffer,    iQuat->value,    bytesCopy);
					// Pad extra values with the last valid values.
					while(newStateNum > oldStateNum){
						--newStateNum;
						tempPreviousBuffer[newStateNum] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[newStateNum]    = tempValueBuffer[oldStateNum-1];
					}

				}
				free(iQuat->previous);
				free(iQuat->value);
				iQuat->previous = tempPreviousBuffer;
				iQuat->value    = tempValueBuffer;
				return 1;
			}
			free(tempPreviousBuffer);
		}
	}
	return 0;
}
void iQuatSkipCurrentInterp(interpQuat *iQuat){
	iQuat->previous[0] = iQuat->value[0];
}
void iQuatResetInterp(interpQuat *iQuat, const size_t stateNum){
	size_t i;
	for(i = 1; i < stateNum; ++i){
		// Shift each value over to make room for the new one.
		iQuat->previous[i] = iQuat->value[i-1];
		iQuat->value[i] = iQuat->value[i-1];
	}
	iQuat->previous[0] = iQuat->value[0];
}
void iQuatPreviousState(interpQuat *iQuat, quat *value, const size_t state, const float interpT){
	if(iQuat->previous[0].w   == iQuat->value[0].w   &&
	   iQuat->previous[0].v.x == iQuat->value[0].v.x &&
	   iQuat->previous[0].v.y == iQuat->value[0].v.y &&
	   iQuat->previous[0].v.z == iQuat->value[0].v.z){
		// If the value hasn't changed, there's nothing to interpolate
		*value = iQuat->value[state];
	}else{
		quatSlerp(&iQuat->previous[state],
		          &iQuat->value[state],
		          interpT,
		          value);
	}
}
unsigned char iQuatUpdate(interpQuat *iQuat, const float interpT){
	if(iQuat->previous[0].w   == iQuat->value[0].w   &&
	   iQuat->previous[0].v.x == iQuat->value[0].v.x &&
	   iQuat->previous[0].v.y == iQuat->value[0].v.y &&
	   iQuat->previous[0].v.z == iQuat->value[0].v.z){
		// If the value hasn't changed, there's nothing to interpolate
		if(iQuat->render.w   == iQuat->value[0].w   &&
		   iQuat->render.v.x == iQuat->value[0].v.x &&
		   iQuat->render.v.y == iQuat->value[0].v.y &&
		   iQuat->render.v.z == iQuat->value[0].v.z){
			return 0;
		}
		iQuat->render = iQuat->value[0];
	}else{
		quatSlerp(&iQuat->previous[0],
		          &iQuat->value[0],
		          interpT,
		          &iQuat->render);
	}
	return 1;
}
