#include "engine/memoryManager.h"
#include "engine/graphicsRenderer.h"
#include "engine/colliderHull.h"
#include "engine/constantsMath.h"
#include "engine/helpersFileIO.h"
#include "engine/sleep.h"
#include <stdio.h>
#include <string.h>
#include <fenv.h>

/****/
#include "engine/graphicsDebug.h"

/****/
#include "engine/moduleObject.h"
#include "engine/modulePhysics.h"
#include "engine/moduleRenderable.h"
#include "engine/moduleModel.h"
#include "engine/moduleSkeleton.h"
#include "engine/moduleTextureWrapper.h"
#include "engine/moduleTexture.h"
#include "engine/moduleGUI.h"
#include "engine/moduleScene.h"
#include "engine/moduleCamera.h"
#include "engine/moduleInput.h"

#include "engine/texture.h"
#include "engine/model.h"
#include "engine/renderable.h"
#include "engine/physicsRigidBody.h"
#include "engine/physicsCollider.h"
#include "engine/physicsJoint.h"
#include "engine/object.h"
#include "engine/scene.h"
#include "engine/camera.h"

#include "engine/gui.h"
#include "engine/particleSystem.h"
#include "engine/particleInitializer.h"
#include "engine/particleEmitter.h"
#include "engine/particleOperator.h"

#include "game/player.h"
#include "game/playerCamera.h"

/**
*** TREE ITERATION FIXES
**/

/** Objects should make a single big heap allocation rather than multiple smaller ones. **/

/** Improve maths functions: not everything has to be 'passed by reference'. Possibly add support for SIMD intrinsics. **/

/** Remember to do regular searches for these important comments when possible. **/
int main(int argc, char **argv){

	graphicsManager gfxMngr;

	// Change the current working directory for file loading.
	fileChangeDirectory(argv[0], NULL);

	// Initialize the memory manager.
	if(memMngrInit(MEMORY_MANAGER_DEFAULT_VIRTUAL_HEAP_SIZE, 1) < 0){
		return 1;
	}
	// Initialize the graphics subsystem.
	if(!gfxMngrInit(&gfxMngr)){
		return 1;
	}

	/** Configs should be loaded here. **/
	//

	/** Most of the code below this comment will be removed eventually. **/
	moduleInputResourcesInit();
	moduleCameraResourcesInit();
	moduleSceneResourcesInit();
	moduleGUIResourcesInit();
	moduleTextureResourcesInit();
	moduleTextureWrapperResourcesInit();
	moduleSkeletonResourcesInit();
	moduleModelResourcesInit();
	moduleRenderableResourcesInit();
	modulePhysicsResourcesInit();
	moduleObjectResourcesInit();

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

	// Scenes.
	scene *scnMain = moduleSceneAllocate();
	scnInit(scnMain);
	//
	scene *scnHUD = moduleSceneAllocate();
	scnInit(scnHUD);

	// Cameras.
	camera *camMain = moduleCameraAllocate();
	camInit(camMain);
	//camMain->orientation.value = quatNewEuler(0.f, 0.f, 45.f*RADIAN_RATIO);
	camMain->position.value = vec3New(0.f, 1.f, 7.f);
	flagsSet(camMain->flags, CAM_PROJECTION_FRUSTUM);
	//
	camera *camHUD = moduleCameraAllocate();
	camInit(camHUD);
	camHUD->position.value = vec3New(0.f, 0.f, 0.f);
	flagsSet(camHUD->flags, CAM_PROJECTION_ORTHOGRAPHIC);
	//
	camera *camGUI = moduleCameraAllocate();
	camInit(camGUI);
	camGUI->position.value = vec3New(0.f, 0.f, 0.f);
	flagsSet(camGUI->flags, CAM_PROJECTION_FIXED_SIZE);

	// Textures.
	texture *tempTex = moduleTextureAllocate();
	//tLoad(tempTex, "Luna"FILE_PATH_DELIMITER_STRING"Luna.tdt");
	//tempTex = moduleTextureAllocate();
	//tLoad(tempTex, "Misc"FILE_PATH_DELIMITER_STRING"ntrance.tdt");
	//tempTex = moduleTextureAllocate();
	//tLoad(tempTex, "Misc"FILE_PATH_DELIMITER_STRING"ntrance.tdt");
	//tempTex = moduleTextureAllocate();
	tLoad(tempTex, "Misc"FILE_PATH_DELIMITER_STRING"Kobold.tdt", 15);
	/*texture *tempTex;
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tDefault(tempTex, &memMngr);
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Luna"FILE_PATH_DELIMITER_STRING"Luna1.png");
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Luna"FILE_PATH_DELIMITER_STRING"Luna2.png");
	resMngrPushResource(&resMngr, &memMngr, RESOURCE_TEXTURE, (byte_t **)&tempTex);
	tLoad(tempTex, &memMngr, "Resources"FILE_PATH_DELIMITER_STRING"Images"FILE_PATH_DELIMITER_STRING"Misc"FILE_PATH_DELIMITER_STRING"ntrance.png");*/

	// Skeletons.
	skeleton *tempSkl = moduleSkeletonAllocate();
	sklLoad(tempSkl, "CubeTestSkeleton2.tds", 21);

	// Texture Wrappers.
	textureWrapper *tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, "Animated"FILE_PATH_DELIMITER_STRING"LunaSpr2.tdw", 21);
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, "Static"FILE_PATH_DELIMITER_STRING"ntrance2.tdw", 19);
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, "Static"FILE_PATH_DELIMITER_STRING"KoboldStatic.tdw", 23);
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, "gui"FILE_PATH_DELIMITER_STRING"body.tdw", 12);
	tempTexWrap = moduleTextureWrapperAllocate();
	twLoad(tempTexWrap, "gui"FILE_PATH_DELIMITER_STRING"border.tdw", 14);
	/*textureWrapper tempTexWrap;
	twDefault(&tempTexWrap, &resMngr, &memMngr);
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	//twLoad(&tempTexWrap, "Animated"FILE_PATH_DELIMITER_STRING"LunaSpr2.tdt", &allTextures);
	//cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));
	twLoad(&tempTexWrap, &resMngr, &memMngr, "Static"FILE_PATH_DELIMITER_STRING"ntrance2.tdt");
	cvPush(&allTexWrappers, (void *)&tempTexWrap, sizeof(tempTexWrap));*/

	// Models.
	model *tempMdl = moduleModelAllocate();
	mdlLoad(tempMdl, "CubeAnimated.obj", 16);
	//mdlLoad(&tempMdl, "ntrance.obj", &allSkeletons);
	//cvPush(&allModels, (void *)&tempMdl, sizeof(tempMdl));

	// Objects.
	objectBase *tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, "CubeTest.tdo", 12);
	tempObj->stateMax = 1;
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, "Wall.tdo", 8);
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, "CubeTest2.tdo", 13);
	tempObj->stateMax = 1;
	tempObj = moduleObjectBaseAllocate();
	objBaseLoad(tempObj, "Lenticular.tdo", 14);
	//objLoad(&tempObj, "ntrance.tdo");
	//cvPush(&allObjects, (void *)&tempObj, sizeof(tempObj));

	// Object Instances.
	object *tempObji2, *tempObji3, *tempObji4;
	object *tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo", 12));
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
	objPhysicsPrepare(tempObji);
	tempObji->skeletonBodies->hull->restitution = 0.f;
	tempObji3 = tempObji;
	//tempObji->configuration[0].position = vec3New(6.032421, -1.907336, -6.143989);
	//tempObji->configuration[0].orientation = quatNew(-0.537948, 0.455196, 0.453047, -0.541063);
	scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest2.tdo", 13));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 4.f-1.9f;//-0.65f;
	objPhysicsPrepare(tempObji);
	tempObji->skeletonBodies->hull->restitution = 0.f;
	physRigidBodyIgnoreLinear(tempObji->skeletonBodies);
	tempObji4 = tempObji;
	//tempObji->configuration[0].scale.x = 8.f;
	//tempObji->configuration[0].scale.y = 0.25f;
	scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo", 12));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 10.f;
	objPhysicsPrepare(tempObji);
	//tempObji->configuration[0].position = vec3New(6.013734, -1.933293, -6.431198);
	//tempObji->configuration[0].orientation = quatNew(1.000000, 0.000000, 0.000000, 0.000000);
	tempObji->skeletonBodies->hull->friction = 0.75f;
	tempObji->skeletonBodies->flags &= ~(0x04);
	tempObji->skeletonBodies->hull->restitution = 0.f;
	tempObji->renderables->state.alpha = 0.5f;
	tempObji->renderables->state.flags = RENDERABLE_STATE_ALPHA_DITHER;
	tempObji2 = tempObji;
	scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo", 12));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.x = 11.f;
	tempObji->configuration[0].position.y = 28.f;
	objPhysicsPrepare(tempObji);
	//tempObji->skeletonBodies->hull->restitution = 1.f;
	tempObji->skeletonBodies->hull->restitution = 0.f;
	scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("CubeTest.tdo", 12));
	//objInstantiate(objGetState(&gameStateManager, tempID, 0), (object *)cvGet(&allObjects, 0));
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl = (model *)cvGet(&allModels, 1);
	//objGetState(&gameStateManager, tempID, 0)->renderables[0].twi.tw = (textureWrapper *)cvGet(&allTexWrappers, 0);
	//objInitSkeleton(objGetState(&gameStateManager, tempID, 0), objGetState(&gameStateManager, tempID, 0)->renderables[0].mdl->skl);
	//objBoneSetPhysicsFlags(objGetState(&gameStateManager, tempID, 0), 0, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE);
	//objInitPhysics(objGetState(&gameStateManager, tempID, 0));
	tempObji->configuration[0].position.y = 6.f;
	objPhysicsPrepare(tempObji);
	tempObji->skeletonBodies->hull->restitution = 0.f;
	///scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("Wall.tdo", 8));
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
	objPhysicsPrepare(tempObji);
	scnInsertObject(scnMain, tempObji);
	//
	tempObji = moduleObjectAllocate();
	objInstantiate(tempObji, moduleObjectBaseFind("Lenticular.tdo", 14));
	tempObji->configuration[0].orientation = quatNewEuler(0.f, 0.f*RADIAN_RATIO, 0.f);
	tempObji->configuration[0].position.y = 6.f;
	tempObji->skeletonBodies->flags &= ~(0x04);
	tempObji->renderables->billboardData.flags = BILLBOARD_TARGET_SPRITE | BILLBOARD_INVERT_ORIENTATION | BILLBOARD_LOCK_Y;
	tempObji->renderables->billboardData.sectors = 8;
	objPhysicsPrepare(tempObji);
	scnInsertObject(scnMain, tempObji);

	// Player.
	player p; playerCamera pc;
	pInit(&p, tempObji);
	pcInit(&pc, camMain);
	pc.pivot = &tempObji->configuration->position;
	pc.pivotStatic = vec3New(0.f, 3.66f, 0.f);
	pcLook(&pc, vec3New(0.f, 0.f, 5.f), vec3New(0.f, 0.f, -5.f));

	// Sprite Object Instances.
	//

	physJoint *joint = modulePhysicsJointAllocate();
	physJointInit(joint, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_DISTANCE);
	physJointAdd(joint, tempObji4->skeletonBodies, tempObji3->skeletonBodies);
	/*physJointSphereInit(
		&joint->data.sphere, tempObji3->skeletonBodies, tempObji4->skeletonBodies, vec3Zero(), vec3Zero(),
		PHYSICS_JOINT_SPHERE_CONE_LIMIT_ENABLED, vec3New(1.f, 0.f, 0.f), 45.f*RADIAN_RATIO
	);*/
	physJointDistanceInit(&joint->data.distance, vec3Zero(), vec3Zero(), 4.f, 0.f, 0.f);
	///scnInsertJoint(scnMain, joint);

	physJoint *joint_carry = modulePhysicsJointAllocate();
	physJointInit(joint_carry, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_UNKNOWN);
	scnInsertJoint(scnMain, joint_carry);

	guiElement gui, *gEl, *gTxt;
	guiInit(&gui, GUI_ELEMENT_TYPE_CONTROLLER);
	/*vec2ZeroP(&gEl.data.panel.configuration[0]);
	vec2ZeroP(&gEl.data.panel.configuration[1]);
	vec2ZeroP(&gEl.data.panel.configuration[2]);
	vec2ZeroP(&gEl.data.panel.configuration[3]);*/
	/*rectangle areas[4] = {
		{.x = 0.f, .y = 0.f, .w = 1.f/3.f, .h = 1.f/3.f},
		{.x = 2.f/3.f, .y = 0.f, .w = 1.f/3.f, .h = 1.f/3.f},
		{.x = 0.f, .y = 2.f/3.f, .w = 1.f/3.f, .h = 1.f/3.f},
		{.x = 2.f/3.f, .y = 2.f/3.f, .w = 1.f/3.f, .h = 1.f/3.f}
	};*/
	gEl = guiNewChild(&gui);
	guiInit(gEl, GUI_ELEMENT_TYPE_WINDOW);
	gEl->root.position.z = -1.f;
	//guiPanelInit(&gEl, areas);
	//rndrStateInit(&gEl.data.panel.rndr.state);
	gEl->data.window.flags = 0;
	twiInit(&gEl->data.window.body, moduleTextureWrapperFind("gui"FILE_PATH_DELIMITER_STRING"body.tdw", 12));
	twiInit(&gEl->data.window.border, moduleTextureWrapperFind("gui"FILE_PATH_DELIMITER_STRING"border.tdw", 14));
	gEl->data.window.offsets[0].x = 3.f/4.f; gEl->data.window.offsets[0].y = 4.f/5.f; gEl->data.window.offsets[0].w = 1.f/4.f; gEl->data.window.offsets[0].h = 1.f/5.f;
	gEl->data.window.offsets[1].x = 0.f; gEl->data.window.offsets[1].y = 4.f/5.f; gEl->data.window.offsets[1].w = 1.f/4.f; gEl->data.window.offsets[1].h = 1.f/5.f;
	gEl->data.window.offsets[2].x = 1.f/4.f; gEl->data.window.offsets[2].y = 4.f/5.f; gEl->data.window.offsets[2].w = 1.f/4.f; gEl->data.window.offsets[2].h = 1.f/5.f;
	gEl->data.window.offsets[3].x = 2.f/4.f; gEl->data.window.offsets[3].y = 4.f/5.f; gEl->data.window.offsets[3].w = 1.f/4.f; gEl->data.window.offsets[3].h = 1.f/5.f;
	gEl->data.window.offsets[4].x = 0.f; gEl->data.window.offsets[4].y = 2.f/5.f; gEl->data.window.offsets[4].w = 1.f; gEl->data.window.offsets[4].h = 1.f/5.f;
	gEl->data.window.offsets[5].x = 0.f; gEl->data.window.offsets[5].y = 0.f; gEl->data.window.offsets[5].w = 1.f; gEl->data.window.offsets[5].h = 1.f/5.f;
	gEl->data.window.offsets[6].x = 0.f; gEl->data.window.offsets[6].y = 1.f/5.f; gEl->data.window.offsets[6].w = 1.f; gEl->data.window.offsets[6].h = 1.f/5.f;
	gEl->data.window.offsets[7].x = 0.f; gEl->data.window.offsets[7].y = 3.f/5.f; gEl->data.window.offsets[7].w = 1.f; gEl->data.window.offsets[7].h = 1.f/5.f;
	gEl->root.position.x = -((float)(gfxMngr.viewport.width>>1));
	gEl->root.position.y = (float)(gfxMngr.viewport.height>>1);
	gEl->root.scale.x = 400.f;//(float)(gfxMngr.viewport.width>>1);
	gEl->root.scale.y = 40.f;

	txtFont testFont;
	gTxt = guiNewChild(&gui);
	guiInit(gTxt, GUI_ELEMENT_TYPE_TEXT);
	gTxt->root.position.x = -((float)(gfxMngr.viewport.width>>1))+24;
	gTxt->root.position.y = (float)(gfxMngr.viewport.height>>1)-24;
	gTxt->data.text.width = (float)(gfxMngr.viewport.width>>1);
	gTxt->data.text.height = (float)(gfxMngr.viewport.height>>1);
	gTxt->data.text.format.font = &testFont;
	gTxt->data.text.format.size = 0.5f;
	gTxt->data.text.format.colour = vec4New(1.f, 1.f, 1.f, 1.f);
	gTxt->data.text.format.background = vec4New(1.f, 1.f, 1.f, 0.f);
	gTxt->data.text.format.style = 0;
	gTxt->data.text.stream.front = memAllocate(1024*sizeof(char));
	gTxt->data.text.stream.back = &gTxt->data.text.stream.front[1023];
	gTxt->data.text.stream.offset = gTxt->data.text.stream.front;
	memcpy(gTxt->data.text.stream.front, "Speed: 0", 8*sizeof(char));
	txtFontLoad(
		&testFont, TEXT_FONT_TYPE_MSDF,
		"IBM_BIOS", 8,
		"fonts"FILE_PATH_DELIMITER_STRING"IBM_BIOS"FILE_PATH_DELIMITER_STRING"PxPlusIBMBIOS.0.tdt", 34,
		"fonts"FILE_PATH_DELIMITER_STRING"IBM_BIOS"FILE_PATH_DELIMITER_STRING"PxPlusIBMBIOS.1.tdt", 34,
		FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Fonts"FILE_PATH_DELIMITER_STRING"IBM_BIOS"FILE_PATH_DELIMITER_STRING"PxPlus_IBM_BIOS-msdf-temp.csv", 38+18,
		FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Fonts"FILE_PATH_DELIMITER_STRING"IBM_BIOS"FILE_PATH_DELIMITER_STRING"PxPlus_IBM_BIOS.ttf", 28+18
	);


	particleBase a; particleBaseInit(&a); a.rndr.mdl = &g_mdlSprite; a.rndr.tw = &g_twDefault;
	particleSystemBase b; particleSystemBaseInit(&b); b.properties = a;
	b.initializers = memAllocate(sizeof(particleInitializer));
	b.initializers->func = &particleInitializerSphereRandom;
	b.initializerLast = b.initializers+1;
	b.emitters = memAllocate(sizeof(particleEmitterBase));
	b.emitters->func = &particleEmitterContinuous;
	b.emitterNum = 1;
	b.operators = memAllocate(sizeof(particleOperator));
	b.operators->func = &particleOperatorAddGravity;
	b.operatorLast = b.operators+1;
	particleSystem c; particleSystemInstantiate(&c, &b);




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
	float timestep = tickrate / 1000.f;  // Desired seconds per update.

	// We store dt in terms of both milliseconds and seconds.
	// Milliseconds are most helpful for animating, while engine
	// physics tend to be expressed most naturally w.r.t. seconds.
	// dt_ms
	float tickrateTimeMod = tickrate * timeMod;
	// dt_s
	float timestepTimeMod = timestep * timeMod;
	// dt_s^{-1}
	float frequencyTimeMod = 1.f / timestepTimeMod;
	float tickrateUpdateMod = tickrate / updateMod;

	float startUpdate;
	float nextUpdate = (float)SDL_GetTicks();
	float startRender;
	float nextRender = (float)SDL_GetTicks();

	uint_least32_t updates = 0;
	uint_least32_t renders = 0;
	uint_least32_t lastPrint = 0;

	const unsigned char *state = SDL_GetKeyboardState(NULL);

	signed char released = 1;
	signed char UP    = 0;
	signed char DOWN  = 0;
	signed char LEFT  = 0;
	signed char RIGHT = 0;
	signed char SPACE = 0;

	signed char lockMouse = 0;
	unsigned int mstate;
	int mx = 0; int my = 0;
	int mx_accumulator = 0; int my_accumulator = 0;

	signed char carry = 0;
	signed char mreleased = 1;

	float speed = 1600.f * timestepTimeMod;
	float acceleration = 250 * timestepTimeMod;
	float stopspeed = 500 * timestepTimeMod;
	float friction = 5 * timestepTimeMod;
	float jump = 2000.f * timestepTimeMod;
	uint_least32_t on_ground = 0;

    while(prgRunning){

		gfxMngrUpdateWindow(&gfxMngr);


		gEl->root.position.x = -((float)(gfxMngr.viewport.width>>1));
		gEl->root.position.y = (float)(gfxMngr.viewport.height>>1);
		gEl->root.scale.x = 400.f;//(float)(gfxMngr.viewport.width>>1);
		gTxt->root.position.x = -((float)(gfxMngr.viewport.width>>1))+24;
		gTxt->root.position.y = (float)(gfxMngr.viewport.height>>1)-24;
		gTxt->data.text.width = (float)(gfxMngr.viewport.width>>1);
		gTxt->data.text.height = (float)(gfxMngr.viewport.height>>1);


		// Take input.
		// This is soon to be replaced by inputManager.
		SDL_PumpEvents();

		if(state[SDL_SCANCODE_ESCAPE]){
			prgRunning = 0;
		}
		if(state[SDL_SCANCODE_L]){
			if(released){
				lockMouse = !lockMouse;
				SDL_SetRelativeMouseMode(lockMouse);
				released = 0;
			}
		}else{
			released = 1;
		}
		if(state[SDL_SCANCODE_T]){
			pcLook(&pc, vec3New(0.f, 0.f, 5.f), vec3New(0.f, 0.f, -5.f));
			p.obj->renderables[0].state.alpha = 1.f;
			if(carry){
				physJointDelete(joint_carry);
				physJointInit(joint_carry, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_UNKNOWN);
				carry = 0;
			}
		}
		if(state[SDL_SCANCODE_F]){
			pcLook(&pc, vec3New(0.f, 0.f, 0.f), vec3New(0.f, 0.f, -5.f));
			p.obj->renderables[0].state.alpha = 0.f;
		}
		if(state[SDL_SCANCODE_W]){
			UP = 1;
		}else{
			UP = 0;
		}
		if(state[SDL_SCANCODE_S]){
			DOWN = 1;
		}else{
			DOWN = 0;
		}
		if(state[SDL_SCANCODE_A]){
			LEFT = 1;
		}else{
			LEFT = 0;
		}
		if(state[SDL_SCANCODE_D]){
			RIGHT = 1;
		}else{
			RIGHT = 0;
		}
		if(state[SDL_SCANCODE_SPACE]){
			SPACE = 1;
		}else{
			SPACE = 0;
		}

		// Get mouse position relative to its position in the last call.
		mstate |= SDL_GetRelativeMouseState(&mx_accumulator, &my_accumulator);
		mx += mx_accumulator; my += my_accumulator;


		startUpdate = (float)SDL_GetTicks();
		if(startUpdate >= nextUpdate){

			// Prepare the next game state.
			/**smPrepareNextState(&gameStateManager);**/
			moduleCameraPrepare();

			/** TEMPORARILY ADD GRAVITY. **/
			physRigidBodyApplyLinearForce(tempObji2->skeletonBodies, vec3New(0.f, -9.80665f*tempObji2->skeletonBodies->mass, 0.f));
			physRigidBodyApplyLinearForce(tempObji3->skeletonBodies, vec3New(0.f, -9.80665f*tempObji3->skeletonBodies->mass, 0.f));
			physRigidBodyApplyLinearForce(tempObji4->skeletonBodies, vec3New(0.f, -9.80665f*tempObji4->skeletonBodies->mass, 0.f));

			pcTick(&pc, mx, my);
			pBasisPC(&p, &pc);
			pInput(&p, (float)(RIGHT-LEFT), (float)(UP-DOWN), SPACE*INPUT_KEY_STATE_PRESSED);
			pTick(&p, timestepTimeMod);
			pRotateVelocity(&p);
			if(p.movement.airborne){
				if(p.movement.velocity.y >= 0.f){
					p.obj->renderables[0].twi.currentAnim = 16;
				}else{
					p.obj->renderables[0].twi.currentAnim = 24;
				}
			}else if((LEFT | RIGHT | UP | DOWN) != 0){
				p.obj->renderables[0].twi.currentAnim = 8;
			}else{
				p.obj->renderables[0].twi.currentAnim = 0;
			}
			if(mstate & SDL_BUTTON(SDL_BUTTON_LEFT)){
				if(mreleased){
					if(p.obj->renderables[0].state.alpha == 0.f){
						if(carry){
							physJointDelete(joint_carry);
							physJointInit(joint_carry, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_UNKNOWN);
							carry = 0;
						}else{
							const vec3 pos = vec3VAddV(*pc.pivot, pc.pivotStatic);
							const vec3 target = vec3VSubV(pc.cam->target.value, pos);
							const vec3 v = vec3VSubV(tempObji2->configuration->position, pos);
							if(vec3Magnitude(v) <= 5.f && vec3Dot(vec3NormalizeFast(target), vec3NormalizeFast(v)) >= 0.9f){
								physJointInit(joint_carry, PHYSICS_JOINT_COLLISION, PHYSICS_JOINT_TYPE_DISTANCE);
								physJointAdd(joint_carry, tempObji2->skeletonBodies, p.obj->skeletonBodies);
								physJointDistanceInit(&joint_carry->data.distance, vec3Zero(), vec3Zero(), 0.f, 0.f, 0.f);
								carry = 1;
							}
						}
					}
					mreleased = 0;
				}
				mstate &= ~SDL_BUTTON(SDL_BUTTON_LEFT);
			}else{
				mreleased = 1;
			}
			if(carry){
				const vec3 target = vec3VSubV(
					pc.cam->target.value,
					*pc.pivot
				);
				joint_carry->data.distance.anchorB = vec3Negate(target);
			}
			sprintf(
				(char *)&gTxt->data.text.stream.front[7], "%f",
				sqrt(p.movement.velocity.x*p.movement.velocity.x + p.movement.velocity.z*p.movement.velocity.z)
			);

			///
			guiTick(&gui, tickrateTimeMod);
			particleSystemTick(&c, timestepTimeMod);
			///
			// Update scenes.
			#ifndef PHYSICS_CONSTRAINT_SOLVER_GAUSS_SEIDEL
			moduleSceneTick(tickrateTimeMod, timestepTimeMod, frequencyTimeMod);
			#else
			moduleSceneTick(tickrateTimeMod, timestepTimeMod);
			#endif

			// Reset mouse movement.
			mx = 0.f;
			my = 0.f;

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


			///
			glUseProgram(gfxMngr.shdrPrgSpr.id);

			// Switch to the camera's view.
			gfxMngrSwitchView(&gfxMngr, &camMain->view);
			// Feed the camera's view-projection matrix into the shader.
			glUniformMatrix4fv(gfxMngr.shdrPrgSpr.vpMatrixID, 1, GL_FALSE, &camMain->viewProjectionMatrix.m[0][0]);
			particleSystemRender(&c, &gfxMngr, camMain, 0.f, interpT);

			// Update the camera's VP matrix.
			camUpdateViewProjectionMatrix(
				camGUI,
				gfxMngr.windowModified,
				gfxMngr.viewport.width,
				gfxMngr.viewport.height,
				interpT
			);
			// Switch to the camera's view.
			gfxMngrSwitchView(&gfxMngr, &camGUI->view);
			// Feed the camera's view-projection matrix into the shader.
			glUniformMatrix4fv(gfxMngr.shdrPrgSpr.vpMatrixID, 1, GL_FALSE, &camGUI->viewProjectionMatrix.m[0][0]);
			guiRender(&gui, &gfxMngr, camGUI, 0.f, interpT);

			glUseProgram(gfxMngr.shdrPrgObj.id);
			///


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

	/****/
	memFree(gTxt->data.text.stream.front);
	txtFontDelete(&testFont);
	particleSystemBaseDelete(&b);
	particleSystemDelete(&c);
	/****/

	moduleObjectResourcesDelete();
	modulePhysicsResourcesDelete();
	moduleRenderableResourcesDelete();
	moduleModelResourcesDelete();
	moduleSkeletonResourcesDelete();
	moduleTextureWrapperResourcesDelete();
	moduleTextureResourcesDelete();
	moduleGUIResourcesDelete();
	moduleSceneResourcesDelete();
	moduleCameraResourcesDelete();
	moduleInputResourcesDelete();

	/** Debug shader program. **/
	gfxDebugDeleteShaderProgram();

	gfxMngrDestroyProgram(&gfxMngr);

	memPrintAllBlocks();
	memPrintFreeBlocks(0);
	memMngrDelete();

	return 0;

}