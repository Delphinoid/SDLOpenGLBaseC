#include "interpState.h"
#include <string.h>

/*
** INTERPOLATED FLOAT FUNCTIONS
*/
unsigned char iFloatInit(interpFloat *iFloat, const size_t stateNum, const float s){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iFloat->render);
		iFloat->previous = malloc(bytes);
		if(iFloat->previous != NULL){
			iFloat->value = malloc(bytes);
			if(iFloat->value != NULL){
				// Initialize stateNum states.
				size_t i = 0;
				while(i < stateNum){
					iFloat->previous[i] = s;
					iFloat->value[i] = s;
					++i;
				}
				iFloat->render = s;
				return 1;
			}
			free(iFloat->previous);
		}
	}
	return 0;
}
unsigned char iFloatResize(interpFloat *iFloat, const size_t oldStateNum, const size_t newStateNum){
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
					size_t i = newStateNum;
					while(i > oldStateNum){
						--i;
						tempPreviousBuffer[i] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[i]    = tempValueBuffer[oldStateNum-1];
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
	for(i = stateNum-1; i > 0; --i){
		// Shift each value over to make room for the new one.
		iFloat->previous[i] = iFloat->previous[i-1];
		iFloat->value[i] = iFloat->value[i-1];
	}
	iFloat->previous[0] = iFloat->value[0];
}
unsigned char iFloatUpdate(interpFloat *iFloat, const size_t state, const float interpT){
	if(iFloat->previous[state] == iFloat->value[state]){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iFloat->render == iFloat->value[state]){
			return 0;
		}
		iFloat->render = iFloat->value[state];
	}else{
		iFloat->render = iFloat->previous[state] +
		                 (iFloat->value[state] - iFloat->previous[state]) *
		                 interpT;
	}
	return 1;
}
void iFloatDelete(interpFloat *iFloat){
	if(iFloat->previous == NULL){
		free(iFloat->previous);
	}
	if(iFloat->value == NULL){
		free(iFloat->value);
	}
}




/*
** INTERPOLATED SIZE_T FUNCTIONS
*/
unsigned char iSizeTInit(interpSizeT *iSizeT, const size_t stateNum, const size_t s){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iSizeT->render);
		iSizeT->previous = malloc(bytes);
		if(iSizeT->previous != NULL){
			iSizeT->value = malloc(bytes);
			if(iSizeT->value != NULL){
				// Initialize stateNum states.
				size_t i = 0;
				while(i < stateNum){
					iSizeT->previous[i] = s;
					iSizeT->value[i] = s;
					++i;
				}
				iSizeT->render = s;
				return 1;
			}
			free(iSizeT->previous);
		}
	}
	return 0;
}
unsigned char iSizeTResize(interpSizeT *iSizeT, const size_t oldStateNum, const size_t newStateNum){
	if(oldStateNum > 0 && newStateNum > 0){
		const size_t bytes = newStateNum * sizeof(iSizeT->render);
		size_t *tempPreviousBuffer = malloc(bytes);
		if(tempPreviousBuffer != NULL){
			size_t *tempValueBuffer = malloc(bytes);
			if(tempValueBuffer != NULL){
				if(newStateNum < oldStateNum){
					// New state vectors are smaller than the old ones.
					memcpy(tempPreviousBuffer, iSizeT->previous, bytes);
					memcpy(tempValueBuffer,    iSizeT->value,    bytes);
				}else{
					const size_t bytesCopy = oldStateNum * sizeof(iSizeT->render);
					memcpy(tempPreviousBuffer, iSizeT->previous, bytesCopy);
					memcpy(tempValueBuffer,    iSizeT->value,    bytesCopy);
					// Pad extra values with the last valid values.
					size_t i = newStateNum;
					while(i > oldStateNum){
						--i;
						tempPreviousBuffer[i] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[i]    = tempValueBuffer[oldStateNum-1];
					}

				}
				free(iSizeT->previous);
				free(iSizeT->value);
				iSizeT->previous = tempPreviousBuffer;
				iSizeT->value    = tempValueBuffer;
				return 1;
			}
			free(tempPreviousBuffer);
		}
	}
	return 0;
}
void iSizeTSkipCurrentInterp(interpSizeT *iSizeT){
	iSizeT->previous[0] = iSizeT->value[0];
}
void iSizeTResetInterp(interpSizeT *iSizeT, const size_t stateNum){
	size_t i;
	for(i = stateNum-1; i > 0; --i){
		// Shift each value over to make room for the new one.
		iSizeT->previous[i] = iSizeT->previous[i-1];
		iSizeT->value[i] = iSizeT->value[i-1];
	}
	iSizeT->previous[0] = iSizeT->value[0];
}
unsigned char iSizeTUpdate(interpSizeT *iSizeT, const size_t state, const float interpT){
	if(iSizeT->previous[state] == iSizeT->value[state]){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iSizeT->render == iSizeT->value[state]){
			return 0;
		}
		iSizeT->render = iSizeT->value[state];
	}
	if(interpT < 1){
		iSizeT->render = iSizeT->previous[state];
	}else{
		iSizeT->render = iSizeT->value[state];
	}
	return 1;
}
void iSizeTDelete(interpSizeT *iSizeT){
	if(iSizeT->previous == NULL){
		free(iSizeT->previous);
	}
	if(iSizeT->value == NULL){
		free(iSizeT->value);
	}
}




/*
** INTERPOLATED 3D VECTOR FUNCTIONS
*/
unsigned char iVec3Init(interpVec3 *iVec3, const size_t stateNum, const float x, const float y, const float z){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iVec3->render);
		iVec3->previous = malloc(bytes);
		if(iVec3->previous != NULL){
			iVec3->value = malloc(bytes);
			if(iVec3->value != NULL){
				// Initialize stateNum states.
				size_t i = 0;
				while(i < stateNum){
					vec3Set(&iVec3->previous[i], x, y, z);
					vec3Set(&iVec3->value[i],    x, y, z);
					++i;
				}
				vec3Set(&iVec3->render, x, y, z);
				return 1;
			}
			free(iVec3->previous);
		}
	}
	return 0;
}
unsigned char iVec3Resize(interpVec3 *iVec3, const size_t oldStateNum, const size_t newStateNum){
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
					size_t i = newStateNum;
					while(i > oldStateNum){
						--i;
						tempPreviousBuffer[i] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[i]    = tempValueBuffer[oldStateNum-1];
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
	for(i = stateNum-1; i > 0; --i){
		// Shift each value over to make room for the new one.
		iVec3->previous[i] = iVec3->previous[i-1];
		iVec3->value[i] = iVec3->value[i-1];
	}
	iVec3->previous[0] = iVec3->value[0];
}
unsigned char iVec3Update(interpVec3 *iVec3, const size_t state, const float interpT){
	if(iVec3->previous[state].x == iVec3->value[state].x &&
	   iVec3->previous[state].y == iVec3->value[state].y &&
	   iVec3->previous[state].z == iVec3->value[state].z){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iVec3->render.x == iVec3->value[state].x &&
		   iVec3->render.y == iVec3->value[state].y &&
		   iVec3->render.z == iVec3->value[state].z){
			return 0;
		}
		iVec3->render = iVec3->value[state];
	}else{
		vec3Lerp(&iVec3->previous[state],
		         &iVec3->value[state],
		         interpT,
		         &iVec3->render);
	}
	return 1;
}
void iVec3Delete(interpVec3 *iVec3){
	if(iVec3->previous == NULL){
		free(iVec3->previous);
	}
	if(iVec3->value == NULL){
		free(iVec3->value);
	}
}




/*
** INTERPOLATED QUATERNION FUNCTIONS
*/
unsigned char iQuatInit(interpQuat *iQuat, const size_t stateNum){
	if(stateNum > 0){
		const size_t bytes = stateNum * sizeof(iQuat->render);
		iQuat->previous = malloc(bytes);
		if(iQuat->previous != NULL){
			iQuat->value = malloc(bytes);
			if(iQuat->value != NULL){
				// Initialize stateNum states.
				size_t i = 0;
				while(i < stateNum){
					quatSetIdentity(&iQuat->previous[i]);
					quatSetIdentity(&iQuat->value[i]);
					++i;
				}
				quatSetIdentity(&iQuat->render);
				return 1;
			}
			free(iQuat->previous);
		}
	}
	return 0;
}
unsigned char iQuatResize(interpQuat *iQuat, const size_t oldStateNum, const size_t newStateNum){
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
					size_t i = newStateNum;
					while(i > oldStateNum){
						--i;
						tempPreviousBuffer[i] = tempPreviousBuffer[oldStateNum-1];
						tempValueBuffer[i]    = tempValueBuffer[oldStateNum-1];
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
	for(i = stateNum-1; i > 0; --i){
		// Shift each value over to make room for the new one.
		iQuat->previous[i] = iQuat->previous[i-1];
		iQuat->value[i] = iQuat->value[i-1];
	}
	iQuat->previous[0] = iQuat->value[0];
}
unsigned char iQuatUpdate(interpQuat *iQuat, const size_t state, const float interpT){
	if(iQuat->previous[state].w   == iQuat->value[state].w   &&
	   iQuat->previous[state].v.x == iQuat->value[state].v.x &&
	   iQuat->previous[state].v.y == iQuat->value[state].v.y &&
	   iQuat->previous[state].v.z == iQuat->value[state].v.z){
		// If the value hasn't changed, there's nothing to interpolate.
		if(iQuat->render.w   == iQuat->value[state].w   &&
		   iQuat->render.v.x == iQuat->value[state].v.x &&
		   iQuat->render.v.y == iQuat->value[state].v.y &&
		   iQuat->render.v.z == iQuat->value[state].v.z){
			return 0;
		}
		iQuat->render = iQuat->value[state];
	}else{
		quatSlerp(&iQuat->previous[state],
		          &iQuat->value[state],
		          interpT,
		          &iQuat->render);
	}
	return 1;
}
void iQuatDelete(interpQuat *iQuat){
	if(iQuat->previous == NULL){
		free(iQuat->previous);
	}
	if(iQuat->value == NULL){
		free(iQuat->value);
	}
}
