#ifndef MODEL_H
#define MODEL_H

#include "gfxProgram.h"
#include "quat.h"
#include "cVector.h"
#include "vertex3D.h"
#include "textureWrapper.h"
#include "camera.h"

/** Merge with sprite and cam (?) struct, add an "interpolation object" **/
typedef struct model {

	vertex3D *vertices;
	unsigned int vertexNum;
	unsigned int *indices;
	unsigned int indexNum;
	GLuint vaoID;  // Vertex array object. Mutable so glDeleteVertexArrays() isn't called when pushing a model into a vector
	GLuint vboID;  // Vertex buffer object. Ditto but with glDeleteBuffers()
	GLuint iboID;  // Index buffer object. Ditto but with glDeleteBuffers()

	/** Everything below here should be stored in a separate struct **/
	vec3 position;        // Position of the sprite
	vec3 relPivot;        // The point the object is rotated around (relative to its position)
	quat orientation;     // Quaternion representing rotation
	vec3 target;          // Target used for billboards
	vec3 changeRot;       // Change in orientation, in eulers. Should be private
	vec3 scale;           // Scale of the object (z = 1)
	float alpha;          // A value to multiply against the alpha values of the pixels

	unsigned char billboardX;       // Whether or not the object uses the camera's rotated X axis
	unsigned char billboardY;       // Whether or not the object uses the camera's rotated Y axis
	unsigned char billboardZ;       // Whether or not the object uses the camera's rotated Z axis
	unsigned char simpleBillboard;  // Whether or not to use a simple billboard method, without the target vector
	unsigned char hudElement;       // Whether or not the object is part of the HUD. Should be private
	unsigned int hudScaleMode;      // 0 = nothing special, 1 = position scaled off window size, 2 = width and height scaled off window size, 3 = both

	textureWrapper *texture;    // Pointer to the texture wrapper being used. Could be pretty dangerous, might need to change it
	unsigned int currentAnim;   // Position of the current animation in the texture wrapper's animations vector
	unsigned int currentFrame;  // The current frame of the animation
	int timesLooped;
	float frameProgress;  // When this is greater than 1, it progresses the animation by frameProgress (rounded towards 0) frames
	                      // and subtracts frameProgress (rounded towards 0) from itself. The lower the FPS, the higher the value
	                      // that will be added to frameProgress and vice versa

} model;

unsigned char mdlLoad(model *mdl, const char *prgPath, const char *filePath, cVector *allTexWrappers);
void mdlCreateVAO(model *mdl);
unsigned int mdlRenderMethod(model *mdl);  // Returns 0 if the model is fully opaque, 1 if the model contains translucency and 2 if the model is fully transparent
void mdlHudElement(model *mdl, unsigned char isHudElement);
void mdlSetRotation(model *mdl, float newX, float newY, float newZ);
void mdlRotateX(model *mdl, float changeX);
void mdlRotateY(model *mdl, float changeY);
void mdlRotateZ(model *mdl, float changeZ);
void mdlAnimate(model *mdl);
void mdlAnimateTex(model *mdl);
void mdlRender(model *mdl, gfxProgram *gfxPrg, camera *cam);
void mdlDelete(model *mdl);

#endif
