#include "memoryManager.h"
#include "graphicsRenderer.h"
#include "colliderMesh.h"
#include "constantsMath.h"
#include "helpersFileIO.h"
#include "sleep.h"
#include <stdio.h>
#include <fenv.h>

/****/
#include "moduleTexture.h"
#include "moduleTextureWrapper.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "moduleRenderable.h"
#include "modulePhysics.h"
#include "moduleObject.h"
#include "moduleScene.h"
#include "moduleCamera.h"

#include "texture.h"
#include "model.h"
#include "renderable.h"
#include "physicsRigidBody.h"
#include "physicsCollider.h"
#include "physicsJoint.h"
#include "object.h"
#include "scene.h"
#include "camera.h"

/**
*** TREE ITERATION FIXES
**/

/** Objects should make a single big heap allocation rather than multiple smaller ones. **/

/** Improve maths functions: not everything has to be 'passed by reference'. Possibly add support for SIMD intrinsics. **/

/** Remember to do regular searches for these important comments when possible. **/
int main(int argc, char **argv){

	char prgPath[FILE_MAX_PATH_LENGTH];
	graphicsManager gfxMngr;

	// Removes program name (everything after the last backslash) from the path.
	{
		const size_t length = strrchr(argv[0], FILE_PATH_DELIMITER_CHAR) + 1 - argv[0];
		strncpy(&prgPath[0], argv[0], length);
		prgPath[length] = '\0';
		/** prgPath can be replaced with ".\\", but it may present some problems when running directly from Code::Blocks. **/
	}

	// Initialize the memory manager.
	if(memMngrInit(MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE, 1) < 0){
		return 1;
	}
	// Initialize the graphics subsystem.
	if(!gfxMngrInit(&gfxMngr, prgPath)){
		return 1;
	}

	/** Configs should be loaded here. **/
	//

	/** Most of the code below this comment will be removed eventually. **/
	moduleTextureResourcesInit();
	moduleTextureWrapperResourcesInit();
	moduleSkeletonResourcesInit();
	moduleModelResourcesInit();
	moduleRenderableResourcesInit();
	modulePhysicsResourcesInit();
	moduleObjectResourcesInit();
	moduleSceneResourcesInit();
	moduleCameraResourcesInit();

	/**printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_TEXTURE_SIZE)*RESOURCE_DEFAULT_TEXTURE_NUM);

	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_TEXTURE_WRAPPER_SIZE)*RESOURCE_DEFAULT_TEXTURE_WRAPPER_NUM);

	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_SIZE)*RESOURCE_DEFAULT_SKELETAL_ANIMATION_FRAGMENT_NUM);
	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_SIZE)*RESOURCE_DEFAULT_SKELETAL_ANIMATION_INSTANCE_NUM);
	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_SKELETAL_ANIMATION_SIZE)*RESOURCE_DEFAULT_SKELETAL_ANIMATION_NUM);
	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_SKELETON_SIZE)*RESOURCE_DEFAULT_SKELETON_NUM);

	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_MODEL_SIZE)*RESOURCE_DEFAULT_MODEL_NUM);

	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_RENDERABLE_BASE_SIZE)*RESOURCE_DEFAULT_RENDERABLE_BASE_NUM);
	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_RENDERABLE_SIZE)*RESOURCE_DEFAULT_RENDERABLE_NUM);

	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_RIGID_BODY_LOCAL_SIZE)*RESOURCE_DEFAULT_RIGID_BODY_LOCAL_NUM);
	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_RIGID_BODY_SIZE)*RESOURCE_DEFAULT_RIGID_BODY_NUM);
	printf("%u + ", memSLinkBlockSize(RESOURCE_DEFAULT_COLLIDER_SIZE)*RESOURCE_DEFAULT_COLLIDER_NUM);
	printf("%u + ", memQLinkBlockSize(RESOURCE_DEFAULT_JOINT_SIZE)*RESOURCE_DEFAULT_JOINT_NUM);
	printf("%u + ", memQLinkBlockSize(RESOURCE_DEFAULT_CONTACT_PAIR_SIZE)*RESOURCE_DEFAULT_CONTACT_PAIR_NUM);
	printf("%u + ", memQLinkBlockSize(RESOURCE_DEFAULT_SEPARATION_PAIR_SIZE)*RESOURCE_DEFAULT_SEPARATION_PAIR_NUM);
	printf("%u + ", memListBlockSize(RESOURCE_DEFAULT_AABB_NODE_SIZE)*RESOURCE_DEFAULT_AABB_NODE_NUM);

	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_OBJECT_BASE_SIZE)*RESOURCE_DEFAULT_OBJECT_BASE_NUM);
	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_OBJECT_SIZE)*RESOURCE_DEFAULT_OBJECT_NUM);

	printf("%u + ", memPoolBlockSize(RESOURCE_DEFAULT_SCENE_SIZE)*RESOURCE_DEFAULT_SCENE_NUM);

	printf("%u\n", memPoolBlockSize(RESOURCE_DEFAULT_CAMERA_SIZE)*RESOURCE_DEFAULT_CAMERA_NUM);**/

	// Textures.
	texture *tempTex = moduleTextureAllocate();
	//tLoad(tempTex, prgPath, "Luna"FILE_PATH_DELIMITER_STRING"Luna.tdt");
	//tempTex = moduleTextureAllocate();
	//tLoad(tempTex, prgPath, "Misc"FILE_PATH_DELIMITER_STRING"ntrance.tdt");
	//tempTex = moduleTextureAllocate();
	//tLoad(tempTex, prgPath, "Misc"FILE_PATH_DELIMITER_STRING"ntrance.tdt");
	//tempTex = moduleTextureAllocate();
	tLoad(tempTex, prgPath, "Misc"FILE_PATH_DELIMITER_STRING"Kobold.tdt");
	/*texture *tempTex;
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tDefault(tempTex, &memMngr);
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, prgPath, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Luna"FILE_PATH_DELIMITER_STRING"Luna1.png");
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, prgPath, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Luna"FILE_PATH_DELIMITER_STRING"Luna2.png");
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, prgPath, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Misc"FILE_PATH_DELIMITER_STRING"ntrance.png");*/

	// Skeletons.
	skeleton *tempSkl = moduleSkeletonAllocate();
	sklLoad(tempSkl, prgPath, "CubeTestSkeleton2.tds");

	// Texture Wrappers.
	textureWrapper *tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, prgPath, "Animated"FILE_PATH_DELIMITER_STRING"LunaSpr2.tdw");
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, prgPath, "Static"FILE_PATH_DELIMITER_STRING"ntrance2.tdw");
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, prgPath, "Static"FILE_PATH_DELIMITER_STRING"KoboldStatic.tdw");
	/*textureWrapper tempTexWrap;
	twDefault(&tempTexWrap, &resMngr, &memMngr);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	//twLoad(&tempTexWrap, prgPath, "Animated"FILE_PATH_DELIMITER_STRING"LunaSpr2.tdt", &allTextures);
	//cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, &resMngr, &memMngr, prgPath, "Static"FILE_PATH_DELIMITER_STRING"ntrance2.tdt");
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));*/

	// Models.
	model *tempMdl = moduleModelAllocate();
	mdlLoad(tempMdl, prgPath, "CubeAnimated.obj");
	//mdlLoad(&tempMdl, prgPath, "ntrance.obj", &allSkeletons);
	//cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	// Objects.
	objectBase *tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, prgPath, "CubeTest.tdo");
	tempObj->stateMax = 1;
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, prgPath, "Wall.tdo");
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, prgPath, "CubeTest2.tdo");
	tempObj->stateMax = 1;
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, prgPath, "Kobold.tdo");
	//objLoad(&tempObj, prgPath, "ntrance.tdo");
	//cvPush(&allObjects, (void *)&tempObj, sizeof(tempObj));

	// Object Instances.
	object *tempObji2, *tempObji3, *tempObji4;
	object *tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo"));
	//sklaiChange(skliAnimationNew(&tempObji->skeletonData), tempObji->skeletonData.skl, tempObji->base->animations[0], 0, 0.f);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), (skeleton *)cvGet(&allSkeletons, 1));
	//objNewRenderable(objGetState(&gameStateManager, tempID, 0), (model *)cvGet(&allModels, 1), (textureWrapper *)cvGet(&allTexWrappers, 1));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), (skeleton *)cvGet(&allSkeletons, 1));
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objGetState(&gameStateManager, tempID, 0)->configuration[0].position.y = 9.5f;
	//objGetState(&gameStateManager, tempID, 0)->configuration[0].position.x = 9.f;
	//objGetState(&gameStateManager, tempID, 0)->configuration[0].position.y = -3.f;
	//quatSetEuler(&tempObji->configuration[0].orientation, 45.f*RADIAN_RATIO, 0.f, 45.f*RADIAN_RATIO);
	///skliLoad(&tempObji->skeletonData, NULL, NULL);
	//tempObji->configuration[0].position.x = 1.5f;
	tempObji->configuration[0].position.y = -1.9f;
	tempObji->skeletonBodies->hull->restitution = 0.f;
	tempObji3 = tempObji;
	//tempObji->configuration[0].position = vec3New(6.032421, -1.907336, -6.143989);
	//tempObji->configuration[0].orientation = quatNew(-0.537948, 0.455196, 0.453047, -0.541063);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest2.tdo"));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 4.f-1.9f;//-0.65f;
	tempObji->skeletonBodies->hull->restitution = 0.f;
	//physRigidBodyIgnoreLinear(tempObji->skeletonBodies);
	tempObji4 = tempObji;
	//tempObji->configuration[0].scale.x = 8.f;
	//tempObji->configuration[0].scale.y = 0.25f;
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo"));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 10.f;
	//tempObji->configuration[0].position = vec3New(6.013734, -1.933293, -6.431198);
	//tempObji->configuration[0].orientation = quatNew(1.000000, 0.000000, 0.000000, 0.000000);
	tempObji->skeletonBodies->hull->friction = 0.75f;
	tempObji->skeletonBodies->flags &= ~(0x04);
	tempObji->skeletonBodies->hull->restitution = 0.f;
	tempObji2 = tempObji;
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo"));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.x = 11.f;
	tempObji->configuration[0].position.y = 28.f;
	//tempObji->skeletonBodies->hull->restitution = 1.f;
	tempObji->skeletonBodies->hull->restitution = 0.f;
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo"));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 6.f;
	tempObji->skeletonBodies->hull->restitution = 0.f;
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("Wall.tdo"));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 1);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_COLLIDE);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	tempObji->configuration[0].position.y = -3.f;
	tempObji->configuration[0].scale.x = 100.f;
	tempObji->configuration[0].scale.y = 0.1f;
	tempObji->configuration[0].scale.z = 100.f;
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("Kobold.tdo"));
	tempObji->configuration[0].orientation = quatNewEuler(0.f, 0.f*RADIAN_RATIO, 0.f);
	tempObji->configuration[0].position.x = 6.f;
	tempObji->configuration[0].position.y = -2.9f;
	tempObji->configuration[0].position.z = 2.f;
	tempObji->renderables->state.flags = BILLBOARD_TARGET_SPRITE | BILLBOARD_LOCK_Y;

	// Sprite Object Instances.
	//

	// Scenes.
	scene *scnMain = moduleSceneAllocate();
	scnInit(scnMain, 3, 3);
	*scnAllocate(scnMain) = moduleObjectIndex(0);
	*scnAllocate(scnMain) = moduleObjectIndex(1);
	*scnAllocate(scnMain) = moduleObjectIndex(2);
	*scnAllocate(scnMain) = moduleObjectIndex(3);
	//*scnAllocate(scnMain) = moduleObjectIndex(4);
	*scnAllocate(scnMain) = moduleObjectIndex(5);
	*scnAllocate(scnMain) = moduleObjectIndex(6);
	//
	scene *scnHUD = moduleSceneAllocate();
	scnInit(scnHUD, 0, 0);

	// Cameras.
	camera *camMain = moduleCameraAllocate();
	camInit(camMain);
	//camMain->orientation.value = quatNewEuler(0.f, 0.f, 45.f*RADIAN_RATIO);
	camMain->position.value = vec3New(0.f, 1.f, 7.f);
	//
	camera *camHUD = moduleCameraAllocate();
	camInit(camHUD);
	camHUD->position.value = vec3New(0.f, 0.f, 0.f);
	flagsSet(camHUD->flags, CAM_PROJECTION_ORTHOGRAPHIC);

	physJoint *joint = modulePhysicsJointAllocate();
	physJointInit(joint, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_DISTANCE);
	physJointAdd(joint, tempObji4->skeletonBodies, tempObji3->skeletonBodies);

	/*physJointSphereInit(
		&joint->data.sphere, tempObji3->skeletonBodies, tempObji4->skeletonBodies, vec3Zero(), vec3Zero(),
		PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED, vec3New(1.f, 0.f, 0.f), 45.f*RADIAN_RATIO
	);*/

	physJointDistanceInit(&joint->data.distance, vec3Zero(), vec3Zero(), 4.f, 0.f, 0.f);



	//memPrintAllBlocks();
	//memPrintFreeBlocks(0);

	/*uint32_t blahstart = SDL_GetTicks();
	uint32_t blahend;
	uint32_t count;
	hbCollisionInfo separationInfo;
	hbCollisionContactManifold collisionData;

	for(count = 0; count < 1000000; ++count){
		//hbCollision(&objGetState(&gameStateManager, 0, 0)->skeletonBodies[0].colliders[0].hb, &objGetState(&gameStateManager, 0, 0)->skeletonBodies[0].centroid,
		//	        &objGetState(&gameStateManager, 2, 0)->skeletonBodies[0].colliders[0].hb, &objGetState(&gameStateManager, 2, 0)->skeletonBodies[0].centroid,
		//	        &separationInfo, &collisionData);
	}

	blahend = SDL_GetTicks();
	printf("%u - %u\n%u\n", blahstart, blahend, blahend-blahstart);*/

	signed char prgRunning = 1;

	float timeMod = 1.f;
	float updateMod = 1.f;

	float framerate = 1000.f / 125.f;  // Desired milliseconds per render.
	float tickrate = 1000.f / 125.f;   // Desired milliseconds per update.
	float tickratio = tickrate / 1000.f;  // Desired seconds per update.

	float tickrateTimeMod = tickrate * timeMod;
	float tickratioTimeMod = tickratio * timeMod;
	float tickratioTimeModFrequency = 1.f / tickratioTimeMod;
	float tickrateUpdateMod = tickrate / updateMod;

	float startUpdate;
	float nextUpdate = (float)SDL_GetTicks();
	float startRender;
	float nextRender = (float)SDL_GetTicks();

	uint_least32_t updates = 0;
	uint_least32_t renders = 0;
	uint_least32_t lastPrint = 0;

	SDL_Event prgEventHandler;

	signed char UP    = 0;
	signed char DOWN  = 0;
	signed char LEFT  = 0;
	signed char RIGHT = 0;

	signed char lockMouse = 0;
	int mouseRelX;
	int mouseRelY;

    while(prgRunning){

		gfxMngrUpdateWindow(&gfxMngr);


		// Take input.
		/** Use command queuing system and poll input on another thread? Probably not actually. **/
		// Detect input.
		SDL_PollEvent(&prgEventHandler);

		// Exit.
		if(prgEventHandler.type == SDL_QUIT){
			prgRunning = 0;
		}

		// Key presses.
		if(prgEventHandler.type == SDL_KEYDOWN){
			if(prgEventHandler.key.keysym.sym == SDLK_ESCAPE){
				prgRunning = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_l){
				lockMouse = !lockMouse;
				SDL_SetRelativeMouseMode(lockMouse);
			}
			if(prgEventHandler.key.keysym.sym == SDLK_UP){
				UP = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_DOWN){
				DOWN = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_LEFT){
				LEFT = 1;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_RIGHT){
				RIGHT = 1;
			}
		}

		// Key releases.
		if(prgEventHandler.type == SDL_KEYUP){
			if(prgEventHandler.key.keysym.sym == SDLK_UP){
				UP = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_DOWN){
				DOWN = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_LEFT){
				LEFT = 0;
			}
			if(prgEventHandler.key.keysym.sym == SDLK_RIGHT){
				RIGHT = 0;
			}
		}

		// Get mouse position relative to its position in the last call.
		SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);


		startUpdate = (float)SDL_GetTicks();
		if(startUpdate >= nextUpdate){

			// Prepare the next game state.
			/**smPrepareNextState(&gameStateManager);**/
			moduleCameraPrepare();

			// Handle inputs.
			if(UP){
				//tempObji2->skeletonBodies->flags &= ~(0x04);
				//moduleObjectIndex(0)->renderables[0].alpha = 1.f;
				//globalTimeMod = 1.f;
				//tickrateTimeMod = tickrate*globalTimeMod;
				//tickratioTimeMod = tickratio*globalTimeMod;
				//const quat changeRotation = quatNewEuler(-90.f*RADIAN_RATIO, 0.f, 0.f);
				//quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				camMain->position.value.z += -5.f * tickratio;
				//objGetState(&gameStateManager, 6, 0)->tempRndrConfig.target.value = camMain->position.value;
				if(tempObji2->skeletonBodies->linearVelocity.z > -9.f){
					tempObji2->skeletonBodies->linearVelocity.z -= 62.5f * tickratio;
				}else if(tempObji2->skeletonBodies->linearVelocity.z < -9.f){
					tempObji2->skeletonBodies->linearVelocity.z = -9.f;
				}
			}
			if(DOWN){
				//tempObji2->skeletonBodies->flags |= (0x04);
				//moduleObjectIndex(0)->renderables[0].alpha = 0.f;
				//globalTimeMod = -1.f;
				//tickrateTimeMod = tickrate*globalTimeMod;
				//tickratioTimeMod = tickratio*globalTimeMod;
				//const quat changeRotation = quatNewEuler(90.f*RADIAN_RATIO, 0.f, 0.f);
				//quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				camMain->position.value.z += 5.f * tickratio;
				//objGetState(&gameStateManager, 6, 0)->tempRndrConfig.target.value = camMain->position.value;
				if(tempObji2->skeletonBodies->linearVelocity.z < 9.f){
					tempObji2->skeletonBodies->linearVelocity.z += 62.5f * tickratio;
				}else if(tempObji2->skeletonBodies->linearVelocity.z > 9.f){
					tempObji2->skeletonBodies->linearVelocity.z = 9.f;
				}
			}
			if(LEFT){
				//const quat changeRotation =
				//quatNewEuler(&changeRotation, 0.f, -90.f*RADIAN_RATIO, 0.f);
				//quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				//quatNewEuler(0.f, 0.f, -90.f*RADIAN_RATIO);
				//quatRotate(&objGetState(&gameStateManager, 5, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 5, 0)->configuration[0].orientation);
				camMain->position.value.x += -5.f * tickratio;
				//objGetState(&gameStateManager, 6, 0)->tempRndrConfig.target.value = camMain->position.value;
				if(tempObji2->skeletonBodies->linearVelocity.x > -9.f){
					tempObji2->skeletonBodies->linearVelocity.x -= 62.5f * tickratio;
				}else if(tempObji2->skeletonBodies->linearVelocity.x < -9.f){
					tempObji2->skeletonBodies->linearVelocity.x = -9.f;
				}
			}
			if(RIGHT){
				//const quat changeRotation =
				//quatNewEuler(&changeRotation, 0.f, 90.f*RADIAN_RATIO, 0.f);
				//quatRotate(&objGetState(&gameStateManager, 0, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 0, 0)->configuration[0].orientation);
				//quatNewEuler(0.f, 0.f, 90.f*RADIAN_RATIO);
				//quatRotate(&objGetState(&gameStateManager, 5, 0)->configuration[0].orientation, &changeRotation, tickratioTimeMod, &objGetState(&gameStateManager, 5, 0)->configuration[0].orientation);
				camMain->position.value.x += 5.f * tickratio;
				//objGetState(&gameStateManager, 6, 0)->tempRndrConfig.target.value = camMain->position.value;
				if(tempObji2->skeletonBodies->linearVelocity.x < 9.f){
					tempObji2->skeletonBodies->linearVelocity.x += 62.5f * tickratio;
				}else if(tempObji2->skeletonBodies->linearVelocity.x > 9.f){
					tempObji2->skeletonBodies->linearVelocity.x = 9.f;
				}
			}

			// Update scenes.
			moduleSceneUpdate(tickrateTimeMod);

			// Query physics islands.
			#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
			moduleSceneQueryIslands(tickratioTimeModFrequency);
			#else
			moduleSceneQueryIslands();
			#endif

			// Solve physics constraints.
			modulePhysicsSolveConstraints(tickratioTimeMod);

			// Next frame.
			nextUpdate += tickrateUpdateMod;
			++updates;

		}

		startRender = (float)SDL_GetTicks();
		if(startRender >= nextRender){

			// Progress between current and next frame.
			const float interpT = (startRender - (nextUpdate - tickrateUpdateMod)) / tickrateUpdateMod;

			/** Remove later **/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene.
			gfxRendererDrawScene(&gfxMngr, camMain, scnMain, interpT);
			// Render the HUD.
			gfxRendererDrawScene(&gfxMngr, camHUD, scnHUD, interpT);
			// Update the window.
			SDL_GL_SwapWindow(gfxMngr.window);

			// Next frame.
			//nextRender = startRender + framerate;
			nextRender += framerate;
			++renders;

		}

		// Sleep until the next event.
		sleepm((int)(nextUpdate <= nextRender ? nextUpdate : nextRender) - (int)SDL_GetTicks());


		if(SDL_GetTicks() - lastPrint > 1000){
			printf("Updates: %u\n", updates);
			printf("Renders: %u\n", renders);
			lastPrint = SDL_GetTicks();
			updates = 0;
			renders = 0;
		}

    }

	moduleCameraResourcesDelete();
	moduleSceneResourcesDelete();
	moduleObjectResourcesDelete();
	modulePhysicsResourcesDelete();
	moduleRenderableResourcesDelete();
	moduleModelResourcesDelete();
	moduleSkeletonResourcesDelete();
	moduleTextureWrapperResourcesDelete();
	moduleTextureResourcesDelete();

	gfxMngrDestroyProgram(&gfxMngr);

	memPrintAllBlocks();
	memPrintFreeBlocks(0);
	memMngrDelete();

	return 0;

}