#include "physicsRigidBody.h"
#include "physicsCollider.h"
#include "physicsJoint.h"
#include "physicsIsland.h"
#include "modulePhysics.h"
#include "skeleton.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PHYSICS_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Skeletons"FILE_PATH_DELIMITER_STRING"Physics"FILE_PATH_DELIMITER_STRING
#define PHYSICS_RESOURCE_DIRECTORY_LENGTH 30

/** Use Parallel Axis Theorem for loading colliders. **/

void physRigidBodyBaseInit(physRigidBodyBase *const __RESTRICT__ local){
	local->flags = PHYSICS_BODY_ASLEEP;
	local->hull = NULL;
	local->mass = 0.f;
	local->inverseMass = 0.f;
	local->linearDamping = 0.f;
	local->angularDamping = 0.f;
	vec3ZeroP(&local->centroid);
	mat3Identity(&local->inverseInertiaTensor);
}

__FORCE_INLINE__ static void physRigidBodyBaseAddCollider(physRigidBodyBase *const __RESTRICT__ local, physCollider *const c, const float **const vertexMassArray){

	// Adds a single collider to the body.

	float mass;
	float inverseMass;
	vec3 centroid;

	// Generate the new collider's mass properties.
	physColliderGenerateMass(&c->c, &mass, &inverseMass, &centroid, vertexMassArray);

	if(mass != 0.f){

		mat3 inertiaTensor;

		const vec3 difference = vec3VSubV(local->centroid, centroid);
		const vec3 differenceWeighted = vec3VMultS(difference, mass);
		const vec3 differenceWeightedSquared = vec3VMultV(difference, differenceWeighted);

		// Generate and add the new collider's inverse moment
		// of inertia using the Parallel Axis Theorem.
		physColliderGenerateMoment(&c->c, &inertiaTensor, &centroid, vertexMassArray);

		// Translate the inertia tensor using the rigid body's centroid.
		inertiaTensor.m[0][0] += differenceWeightedSquared.y + differenceWeightedSquared.z;
		inertiaTensor.m[0][1] -= differenceWeighted.x * difference.y;
		inertiaTensor.m[0][2] -= differenceWeighted.x * difference.z;
		inertiaTensor.m[1][1] += differenceWeightedSquared.x + differenceWeightedSquared.z;
		inertiaTensor.m[1][2] -= differenceWeighted.y * difference.z;
		inertiaTensor.m[2][2] += differenceWeightedSquared.x + differenceWeightedSquared.y;

		// Add the collider's contribution to the body's inertia tensor.
		local->inverseInertiaTensor = mat3Invert(local->inverseInertiaTensor);
		local->inverseInertiaTensor.m[0][0] += inertiaTensor.m[0][0];
		local->inverseInertiaTensor.m[0][1] += inertiaTensor.m[0][1];
		local->inverseInertiaTensor.m[1][0] += inertiaTensor.m[0][1];
		local->inverseInertiaTensor.m[0][2] += inertiaTensor.m[0][2];
		local->inverseInertiaTensor.m[2][0] += inertiaTensor.m[0][2];
		local->inverseInertiaTensor.m[1][1] += inertiaTensor.m[1][1];
		local->inverseInertiaTensor.m[1][2] += inertiaTensor.m[1][2];
		local->inverseInertiaTensor.m[2][1] += inertiaTensor.m[1][2];
		local->inverseInertiaTensor.m[2][2] += inertiaTensor.m[2][2];
		local->inverseInertiaTensor = mat3Invert(local->inverseInertiaTensor);

		// Calculate the new, weighted centroid.
		local->inverseMass = 1.f / (local->mass + mass);
		local->centroid = vec3VMultS(vec3VAddV(vec3VMultS(local->centroid, local->mass), vec3VMultS(centroid, mass)), local->inverseMass);
		local->mass += mass;

	}

}

__FORCE_INLINE__ static void physRigidBodyBaseGenerateProperties(physRigidBodyBase *const __RESTRICT__ local, const float **const vertexMassArray){

	// Calculates the rigid body's total mass, inverse mass,
	// centroid and inertia tensor, as well as the mass
	// properties for each of its colliders.

	physCollider *c;
	const float **m;

	float tempMass = 0.f;
	vec3 tempCentroid = {.x = 0.f, .y = 0.f, .z = 0.f};
	float inertiaTensor[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	c = local->hull;
	m = vertexMassArray;
	while(c != NULL){

		float colliderMass;
		float colliderInverseMass;
		vec3 colliderCentroid;

		physColliderGenerateMass(&c->c, &colliderMass, &colliderInverseMass, &colliderCentroid, m);

		colliderCentroid = vec3VMultS(colliderCentroid, colliderMass);
		tempCentroid = vec3VAddV(tempCentroid, colliderCentroid);
		tempMass += colliderMass;

		c = (physCollider *)memSLinkNext(c);
		if(m != NULL){
			++m;
		}

	}

	if(tempMass != 0.f){

		local->inverseMass = 1.f / tempMass;
		tempCentroid = vec3VMultS(tempCentroid, local->inverseMass);

		// Calculate the combined inverse moment of inertia for
		// the collider as the sum of its collider's moments.
		c = local->hull;
		m = vertexMassArray;
		while(c != NULL){

			mat3 colliderInertiaTensor;
			physColliderGenerateMoment(&c->c, &colliderInertiaTensor, &tempCentroid, m);

			inertiaTensor[0] += colliderInertiaTensor.m[0][0];
			inertiaTensor[1] += colliderInertiaTensor.m[1][1];
			inertiaTensor[2] += colliderInertiaTensor.m[2][2];
			inertiaTensor[3] += colliderInertiaTensor.m[0][1];
			inertiaTensor[4] += colliderInertiaTensor.m[0][2];
			inertiaTensor[5] += colliderInertiaTensor.m[1][2];

			c = (physCollider *)memSLinkNext(c);
			if(m != NULL){
				++m;
			}

		}

		local->inverseInertiaTensor.m[0][0] = inertiaTensor[0];
		local->inverseInertiaTensor.m[1][1] = inertiaTensor[1];
		local->inverseInertiaTensor.m[2][2] = inertiaTensor[2];
		local->inverseInertiaTensor.m[0][1] = inertiaTensor[3];
		local->inverseInertiaTensor.m[0][2] = inertiaTensor[4];
		local->inverseInertiaTensor.m[1][2] = inertiaTensor[5];
		// No point calculating the same numbers twice.
		local->inverseInertiaTensor.m[1][0] = inertiaTensor[3];
		local->inverseInertiaTensor.m[2][0] = inertiaTensor[4];
		local->inverseInertiaTensor.m[2][1] = inertiaTensor[5];
		local->inverseInertiaTensor = mat3Invert(local->inverseInertiaTensor);

	}else{
		local->inverseMass = 0.f;
		mat3ZeroP(&local->inverseInertiaTensor);
	}

	local->mass = tempMass;
	local->centroid = tempCentroid;

}

/** TEMPORARY **/
static return_t physColliderResizeToFit(physCollider *const __RESTRICT__ local){

	if(local->c.type == COLLIDER_TYPE_MESH){

		cMesh *const cHull = (cMesh *)&local->c.data;

		if(cHull->vertexNum != 0){
			vec3 *const tempBuffer = memReallocate(cHull->vertices, cHull->vertexNum*sizeof(vec3));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			cHull->vertices = tempBuffer;
		}else{
			if(cHull->vertices != NULL){
				memFree(cHull->vertices);
				cHull->vertices = NULL;
			}
		}

		if(cHull->faceNum != 0){
			cMeshFace *tempBuffer2;
			vec3 *tempBuffer1 = memReallocate(cHull->normals, cHull->faceNum*sizeof(vec3));
			if(tempBuffer1 == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			tempBuffer2 = memReallocate(cHull->faces, cHull->faceNum*sizeof(cMeshFace));
			if(tempBuffer2 == NULL){
				/** Memory allocation failure. **/
				memFree(tempBuffer1);
				return -1;
			}
			cHull->normals = tempBuffer1;
			cHull->faces = tempBuffer2;
		}else{
			if(cHull->normals != NULL){
				memFree(cHull->normals);
				cHull->normals = NULL;
			}
			if(cHull->faces != NULL){
				memFree(cHull->faces);
				cHull->faces = NULL;
			}
		}

		if(cHull->edgeNum != 0){
			cMeshEdge *const tempBuffer = memReallocate(cHull->edges, cHull->edgeNum*sizeof(cMeshEdge));
			if(tempBuffer == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			cHull->edges = tempBuffer;
		}else if(cHull->edges != NULL){
			memFree(cHull->edges);
			cHull->edges = NULL;
		}

	}

	return 1;

}

/** Everything about this function sucks and I hate it. **/
return_t physRigidBodyBaseLoad(physRigidBodyBase **const __RESTRICT__ bodies, physicsBodyIndex_t **bodyIDs, physicsBodyIndex_t *const bodyNum, const skeleton *const __RESTRICT__ skl, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	// Loads a series of rigid bodies.
	//
	// If skeleton is not NULL, it constrains them using
	// the specified bone names.

	return_t success = 0;

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *__RESTRICT__ rbInfo;

	fileGenerateFullPath(fullPath, PHYSICS_RESOURCE_DIRECTORY_STRING, PHYSICS_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);
	rbInfo = fopen(fullPath, "r");

	if(rbInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		physicsBodyIndex_t idCapacity = 0;
		cVertexIndex_t vertexCapacity = 0;
		cFaceIndex_t normalCapacity = 0;
		cEdgeIndex_t edgeCapacity = 0;
		float **vertexMassArrays = NULL;  // Array of vertex masses for each collider.

		int isSkeleton = -1;         // Whether or not a skeleton is being described.
		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = rigid body, 1 = collider).
		fileLine_t currentLine = 0;  // Current file line being read.

		physRigidBodyBase *currentBody = NULL;
		physCollider *currentCollider = NULL;
		size_t currentBodyColliderNum = 0;

		while(fileParseNextLine(rbInfo, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			++currentLine;

			// Rigid body
			if(lineLength >= 11 && strncmp(line, "rigidbody", 9) == 0){

				char *end;
				while(line[10] == ' ' || line[10] == '\t'){
					++line;
					--lineLength;
				}

				// Make sure a brace exists.
				end = strrchr(line+10, '{');
				if(end){

					physicsBodyIndex_t currentBodyID = 0;

					// Check if the rigid body has a name.
					if(end != &line[10] && skl != NULL){

						line += 10;

						if(isSkeleton){

							// Parse the name.
							while(*end == ' ' || *end == '\t'){
								--end;
							}
							lineLength = end-line;

							if(line[0] == '"'){
								while(*end != '"'){
									--end;
								}
								if(end != line){
									++line;
									lineLength = end-line;
								}
							}

							// Loop through the skeleton to find a bone with the given name.
							for(; currentBodyID < skl->boneNum; ++currentBodyID){
								if(strncmp(line, skl->bones[currentBodyID].name, lineLength) == 0){
									break;
								}
							}

							isSkeleton = 1;

						}else{
							printf("Error loading rigid bodies \"%s\": Rigid body at line %u appears to be attached to a bone when "
							       "a previous body was not. Ignoring.\n", fullPath, currentLine);
						}

					}else{
						isSkeleton = 0;
					}

					// Close any current commands.
					if(currentCommand != -1){
						printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
							   "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
						if(currentCommand == 0){
							// Generate various mass properties for the rigid body.
							physRigidBodyBaseGenerateProperties(currentBody, (const float **)vertexMassArrays);
							// Free the collider mass arrays.
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
								vertexMassArrays = NULL;
							}
							if(!isSkeleton){
								break;
							}
						}else if(currentCommand == 1){
							if(physColliderResizeToFit(currentCollider) < 0){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									float **array = vertexMassArrays;
									float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
									for(; array < arrayLast; ++array){
										if(array != NULL){
											memFree(*array);
										}
									}
									memFree(vertexMassArrays);
								}
								if(*bodyIDs != NULL){
									memFree(*bodyIDs);
								}
								modulePhysicsRigidBodyBaseFreeArray(bodies);
								fclose(rbInfo);
								return -1;
							}
						}
					}
					// Initialize a new rigid body.
					currentBody = modulePhysicsRigidBodyBaseAppend(bodies);
					if(currentBody == NULL){
						/** Memory allocation failure. **/
						if(vertexMassArrays != NULL){
							float **array = vertexMassArrays;
							float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
							for(; array < arrayLast; ++array){
								if(array != NULL){
									memFree(*array);
								}
							}
							memFree(vertexMassArrays);
						}
						if(*bodyIDs != NULL){
							memFree(*bodyIDs);
						}
						modulePhysicsRigidBodyBaseFreeArray(bodies);
						fclose(rbInfo);
						return -1;
					}
					// Reallocate ID array if necessary.
					if(*bodyNum == idCapacity){
						if(idCapacity == 0){
							idCapacity = 1;
						}else{
							idCapacity *= 2;
						}
						physicsBodyIndex_t *tempBuffer = memReallocate(*bodyIDs, idCapacity*sizeof(physicsBodyIndex_t));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						*bodyIDs = tempBuffer;
					}
					physRigidBodyBaseInit(currentBody);
					(*bodyIDs)[*bodyNum] = currentBodyID;
					currentBodyColliderNum = 0;
					currentBody->flags = PHYSICS_BODY_DEFAULT_STATE;
					currentCommand = 0;
					++(*bodyNum);

				}


			// Collider
			}else if(lineLength >= 10 && strncmp(line, "collider ", 9) == 0){
				if(strchr(line+9, '{')){
					if(currentCommand >= 0){

						float **tempBuffer;

						// Close any current commands.
						if(currentCommand > 0){
							printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
							       "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
							if(currentCommand == 1){
								if(physColliderResizeToFit(currentCollider) < 0){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										float **array = vertexMassArrays;
										float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
										for(; array < arrayLast; ++array){
											if(array != NULL){
												memFree(*array);
											}
										}
										memFree(vertexMassArrays);
									}
									if(*bodyIDs != NULL){
										memFree(*bodyIDs);
									}
									modulePhysicsRigidBodyBaseFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						// Allocate memory for the new collider.
						currentCollider = modulePhysicsColliderAppend(&currentBody->hull);
						if(currentCollider == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer = memReallocate(vertexMassArrays, (currentBodyColliderNum+1)*sizeof(float *));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}

						vertexMassArrays = tempBuffer;
						vertexMassArrays[currentBodyColliderNum] = NULL;
						vertexCapacity = 0;
						normalCapacity = 0;
						edgeCapacity = 0;

						physColliderInit(currentCollider, COLLIDER_TYPE_MESH, currentBody);
						++currentBodyColliderNum;

						currentCommand = 1;

					}else{
						printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"constraint\" invoked on line %u "
						       "without specifying a rigid body.\n", fullPath, currentLine);
					}

				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"collider\" at line %u "
					       "does not contain a brace.\n", fullPath, currentLine);
				}


			// Collider vertex
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){

				if(currentBodyColliderNum > 0){

					cMesh *const cHull = (cMesh *)&currentCollider->c.data;
					const char *token;

					// Reallocate vertex array if necessary.
					if(cHull->vertexNum == vertexCapacity){
						if(vertexCapacity == 0){
							vertexCapacity = 1;
						}else{
							vertexCapacity *= 2;
						}
						float *tempBuffer2;
						vec3 *tempBuffer1 = memReallocate(cHull->vertices, vertexCapacity*sizeof(vec3));
						if(tempBuffer1 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer2 = memReallocate(vertexMassArrays[currentBodyColliderNum-1], vertexCapacity*sizeof(float));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							fclose(rbInfo);
							memFree(tempBuffer1);
							return -1;
						}
						cHull->vertices = tempBuffer1;
						vertexMassArrays[currentBodyColliderNum-1] = tempBuffer2;
					}

					token = strtok(line+2, " ");
					cHull->vertices[cHull->vertexNum].x = strtod(token, NULL);
					token = strtok(NULL, " ");
					cHull->vertices[cHull->vertexNum].y = strtod(token, NULL);
					token = strtok(NULL, " ");
					cHull->vertices[cHull->vertexNum].z = strtod(token, NULL);
					token = strtok(NULL, " ");
					if(token != NULL){
						vertexMassArrays[currentBodyColliderNum-1][cHull->vertexNum] = strtod(token, NULL);
					}
					++cHull->vertexNum;

				}


			// Collider face
			}else if(lineLength >= 7 && strncmp(line, "f ", 2) == 0){

				if(currentBodyColliderNum > 0){

					const char *token = strtok(line+2, " ");
					if(token != NULL){

						cMesh *const cHull = (cMesh *)&currentCollider->c.data;

						cEdgeIndex_t start;
						cEdgeIndex_t end = strtoul(token, NULL, 0)-1;

						cEdgeIndex_t oldNum = cHull->edgeNum;
						cEdgeIndex_t addNum = 0;

						cEdgeIndex_t first = (cEdgeIndex_t)-1;
						cEdgeIndex_t last = oldNum;

						byte_t firstTwin = 0;
						byte_t lastTwin = 0;

						byte_t exit = 0;

						vec3 BsA;
						vec3 CsA;

						// Recursively add the face's edges.
						for(;;){

							cEdgeIndex_t i = 0;

							token = strtok(NULL, " ");
							if(token == NULL){
								// If this was the last vertex, loop back
								// to the first one and create an edge with it.
								token = strtok(line+2, " ");
								exit = 1;
							}

							// Reallocate edge array if necessary.
							if(cHull->edgeNum == edgeCapacity){
								cMeshEdge *tempBuffer;
								if(edgeCapacity == 0){
									edgeCapacity = 3;
								}else{
									edgeCapacity *= 2;
								}
								tempBuffer = memReallocate(cHull->edges, edgeCapacity*sizeof(cMeshEdge));
								if(tempBuffer == NULL){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										float **array = vertexMassArrays;
										float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
										for(; array < arrayLast; ++array){
											if(array != NULL){
												memFree(*array);
											}
										}
										memFree(vertexMassArrays);
									}
									if(*bodyIDs != NULL){
										memFree(*bodyIDs);
									}
									modulePhysicsRigidBodyBaseFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
								cHull->edges = tempBuffer;
							}

							start = end;
							end = strtoul(token, NULL, 0)-1;

							// Look for a twin edge. If it is found,
							// we don't have to store the new edge.
							while(i < oldNum){
								// Check if the current edge is the next edge or a twin.
								if(start == cHull->edges[i].end && end == cHull->edges[i].start){
									// If the new edge is a twin, don't add it.
									// Instead, set the current edge's twin.
									if(first == (cEdgeIndex_t)-1){
										// If this is the first vertex, set it.
										first = i;
										firstTwin = 1;
									}else{
										// Set this edge as the last edge's next.
										if(lastTwin){
											cHull->edges[last].twinNext = i;
										}else{
											cHull->edges[last].next = i;
										}
									}
									cHull->edges[i].twinFace = cHull->faceNum;
									last = i;
									lastTwin = 1;
									break;
								}
								++i;
							}

							if(i >= oldNum){
								// No twin was found.
								if(first == (cEdgeIndex_t)-1){
									// If this is the first vertex, set it.
									first = cHull->edgeNum;
									firstTwin = 0;
								}else{
									// Set this edge as the last edge's next.
									if(lastTwin){
										cHull->edges[last].twinNext = cHull->edgeNum;
									}else{
										cHull->edges[last].next = cHull->edgeNum;
									}
								}
								last = cHull->edgeNum;
								lastTwin = 0;
								cHull->edges[last].start = start;
								cHull->edges[last].end = end;
								cHull->edges[last].face = cHull->faceNum;
								cHull->edges[last].twinFace = cHull->faceNum;
								++cHull->edgeNum;
							}

							++addNum;

							if(exit){
								if(lastTwin){
									cHull->edges[last].twinNext = first;
								}else{
									cHull->edges[last].next = first;
								}
								break;
							}

						}

						// Reallocate normal and offset arrays if necessary.
						if(cHull->faceNum == normalCapacity){
							vec3 *tempBuffer1;
							cMeshFace *tempBuffer2;
							if(normalCapacity == 0){
								normalCapacity = 1;
							}else{
								normalCapacity *= 2;
							}
							tempBuffer1 = memReallocate(cHull->normals, normalCapacity*sizeof(vec3));
							if(tempBuffer1 == NULL){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									float **array = vertexMassArrays;
									float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
									for(; array < arrayLast; ++array){
										if(array != NULL){
											memFree(*array);
										}
									}
									memFree(vertexMassArrays);
								}
								if(*bodyIDs != NULL){
									memFree(*bodyIDs);
								}
								modulePhysicsRigidBodyBaseFreeArray(bodies);
								fclose(rbInfo);
								return -1;
							}
							tempBuffer2 = memReallocate(cHull->faces, normalCapacity*sizeof(cMeshFace));
							if(tempBuffer2 == NULL){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									float **array = vertexMassArrays;
									float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
									for(; array < arrayLast; ++array){
										if(array != NULL){
											memFree(*array);
										}
									}
									memFree(vertexMassArrays);
								}
								if(*bodyIDs != NULL){
									memFree(*bodyIDs);
								}
								modulePhysicsRigidBodyBaseFreeArray(bodies);
								fclose(rbInfo);
								memFree(tempBuffer1);
								return -1;
							}
							cHull->normals = tempBuffer1;
							cHull->faces = tempBuffer2;
						}

						// Generate a normal for the face.
						if(firstTwin){
							BsA = vec3VSubV(cHull->vertices[cHull->edges[first].start], cHull->vertices[cHull->edges[first].end]);
							if(lastTwin){
								CsA = vec3VSubV(cHull->vertices[cHull->edges[last].end], cHull->vertices[cHull->edges[first].end]);
							}else{
								CsA = vec3VSubV(cHull->vertices[cHull->edges[last].start], cHull->vertices[cHull->edges[first].end]);
							}
						}else{
							BsA = vec3VSubV(cHull->vertices[cHull->edges[first].end], cHull->vertices[cHull->edges[first].start]);
							if(lastTwin){
								CsA = vec3VSubV(cHull->vertices[cHull->edges[last].end], cHull->vertices[cHull->edges[first].start]);
							}else{
								CsA = vec3VSubV(cHull->vertices[cHull->edges[last].start], cHull->vertices[cHull->edges[first].start]);
							}
						}
						cHull->normals[cHull->faceNum] = vec3NormalizeFastAccurate(vec3Cross(BsA, CsA));

						//cHull->faces[cHull->faceNum].edgeNum = addNum;
						if(addNum > cHull->edgeMax){
							// Update the maximum edge num.
							cHull->edgeMax = addNum;
						}
						cHull->faces[cHull->faceNum].edge = first;

						++cHull->faceNum;

					}else{
						printf("Error loading rigid bodies \"%s\": Collider face at line %u "
						       "must have at least three vertices.\n", fullPath, currentLine);
					}

				}


			// Density
			}else if(lineLength >= 9 && strncmp(line, "density ", 8) == 0){
				if(currentCommand == 1){
					currentCollider->density = strtof(line+8, NULL);
				}else if(currentCommand <= 0){
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"density\" at line %u does not belong "
					       "in any other multiline command.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"density\" invoked on line %u without "
					       "specifying a collider.\n", fullPath, currentLine);
				}


			// Friction
			}else if(lineLength >= 10 && strncmp(line, "friction ", 9) == 0){
				if(currentCommand == 1){
					currentCollider->friction = strtof(line+9, NULL);
				}else if(currentCommand <= 0){
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"friction\" at line %u does not belong "
					       "in any other multiline command.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"friction\" invoked on line %u without "
					       "specifying a collider.\n", fullPath, currentLine);
				}


			// Coefficient of restitution
			}else if(lineLength >= 13 && strncmp(line, "restitution ", 12) == 0){
				if(currentCommand == 1){
					currentCollider->restitution = strtof(line+12, NULL);
					if(currentCollider->restitution > 1.f){
						currentCollider->restitution = 1.f;
					}else if(currentCollider->restitution < 0.f){
						currentCollider->restitution = 0.f;
					}
				}else if(currentCommand <= 0){
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"restitution\" at line %u does not belong "
					       "in any other multiline command.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Collider sub-command \"restitution\" invoked on line %u without "
					       "specifying a collider.\n", fullPath, currentLine);
				}


			// Collision flag
			}else if(lineLength >= 11 && strncmp(line, "collision ", 10) == 0){
				if(currentCommand == 0){
					if(strtoul(line+10, NULL, 0)){
						flagsSet(currentBody->flags, PHYSICS_BODY_COLLIDE);
					}else{
						flagsUnset(currentBody->flags, PHYSICS_BODY_COLLIDE);
					}
				}else if(currentCommand == 1){
					currentCollider->layers = strtoul(line+10, NULL, 0);
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body and collider sub-command \"collision\" invoked on line %u without "
					       "specifying a rigid body or collider.\n", fullPath, currentLine);
				}


			// Active flag
			}else if(lineLength >= 8 && strncmp(line, "active ", 7) == 0){
				if(currentCommand == 0){
					if(strtoul(line+7, NULL, 0)){
						flagsSet(currentBody->flags, PHYSICS_BODY_DEFAULT_STATE);
					}else{
						flagsUnset(currentBody->flags, PHYSICS_BODY_DEFAULT_STATE);
					}
				}else if(currentCommand > 0){
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"active\" at line %u does not belong "
					       "in any other multiline command.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"active\" invoked on line %u without "
					       "specifying a rigid body.\n", fullPath, currentLine);
				}
			}

			if(lineLength > 0 && line[lineLength-1] == '}'){
				if(currentCommand == 0){

					if(currentBodyColliderNum > 0){

						// Generate various mass properties for the rigid body.
						physRigidBodyBaseGenerateProperties(currentBody, (const float **)vertexMassArrays);

						// Free the collider mass arrays.
						if(vertexMassArrays != NULL){
							float **array = vertexMassArrays;
							float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
							for(; array < arrayLast; ++array){
								if(array != NULL){
									memFree(*array);
								}
							}
							memFree(vertexMassArrays);
							vertexMassArrays = NULL;
						}

						success = 1;

						if(!isSkeleton){
							break;
						}

					}

					currentCommand = -1;

				}else if(currentCommand == 1){

					const cMesh *const cHull = (cMesh *)&currentCollider->c.data;

					if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

						if(physColliderResizeToFit(currentCollider) < 0){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								float **array = vertexMassArrays;
								float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
								for(; array < arrayLast; ++array){
									if(array != NULL){
										memFree(*array);
									}
								}
								memFree(vertexMassArrays);
							}
							if(*bodyIDs != NULL){
								memFree(*bodyIDs);
							}
							modulePhysicsRigidBodyBaseFreeArray(bodies);
							return -1;
						}

					}else{
						printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
						--currentBodyColliderNum;
						physColliderResizeToFit(currentCollider);
					}
					currentCommand = 0;

				}else{
					printf("Error loading rigid bodies \"%s\": Stray brace on line %u.\n", fullPath, currentLine);
				}
			}

		}

		fclose(rbInfo);

		if(currentCommand == 0){

			if(currentBodyColliderNum > 0){

				// Generate various mass properties for the rigid body.
				physRigidBodyBaseGenerateProperties(currentBody, (const float **)vertexMassArrays);

				// Free the collider mass arrays.
				if(vertexMassArrays != NULL){
					float **array = vertexMassArrays;
					float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
					for(; array < arrayLast; ++array){
						if(array != NULL){
							memFree(*array);
						}
					}
					memFree(vertexMassArrays);
					vertexMassArrays = NULL;
				}

			}

		}else if(currentCommand == 1){

			const cMesh *const cHull = (cMesh *)&currentCollider->c.data;

			if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

				if(physColliderResizeToFit(currentCollider) < 0){
					/** Memory allocation failure. **/
					if(vertexMassArrays != NULL){
						float **array = vertexMassArrays;
						float **const arrayLast = &vertexMassArrays[currentBodyColliderNum];
						for(; array < arrayLast; ++array){
							if(array != NULL){
								memFree(*array);
							}
						}
						memFree(vertexMassArrays);
					}
					if(*bodyIDs != NULL){
						memFree(*bodyIDs);
					}
					modulePhysicsRigidBodyBaseFreeArray(bodies);
					return -1;
				}

			}else{
				printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
				--currentBodyColliderNum;
				physColliderResizeToFit(currentCollider);
			}
			currentCommand = 0;

		}

	}else{
		printf("Error loading rigid bodies \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	return success;

}

void physRigidBodyBaseDelete(physRigidBodyBase *const __RESTRICT__ local){
	modulePhysicsColliderFreeArray(&local->hull);
}

void physRigidBodyInit(physRigidBody *const __RESTRICT__ body){
	body->flags = PHYSICS_BODY_ASLEEP;
	body->base = NULL;
	body->hull = NULL;
	boneInit(&body->configuration);
	vec3ZeroP(&body->linearVelocity);
	vec3ZeroP(&body->angularVelocity);
	vec3ZeroP(&body->netForce);
	vec3ZeroP(&body->netTorque);
}

return_t physRigidBodyInstantiate(physRigidBody *const __RESTRICT__ body, const physRigidBodyBase *const __RESTRICT__ local){

	physCollider *cBody = NULL;
	physCollider *cLocal = local->hull;

	physRigidBodyInit(body);

	// Copy the colliders so we can transform them into global space.
	while(cLocal != NULL){
		cBody = modulePhysicsColliderInsertAfter(&body->hull, cBody);
		physColliderInstantiate(cBody, cLocal, body);
		cLocal = (physCollider *)memSLinkNext(cLocal);
	}

	// Copy physical properties.
	body->mass = local->mass;
	body->inverseMass = local->inverseMass;
	body->linearDamping = local->linearDamping;
	body->angularDamping = local->angularDamping;
	#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
	body->centroidLocal = local->centroid;
	#ifdef PHYSICS_BODY_SCALE_INERTIA_TENSORS
	body->inertiaTensorLocal = mat3Invert(local->inverseInertiaTensor);
	#else
	body->inverseInertiaTensorLocal = local->inverseInertiaTensor;
	#endif
	#endif

	body->base = local;
	body->flags = local->flags;

	return 1;

}

__FORCE_INLINE__ void physRigidBodySetUninitialized(physRigidBody *const __RESTRICT__ body){
	flagsSet(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetInitialized(physRigidBody *const __RESTRICT__ body){
	flagsUnset(body->flags, PHYSICS_BODY_UNINITIALIZED);
	flagsSet(body->flags, PHYSICS_BODY_INITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetInitializedFull(physRigidBody *const __RESTRICT__ body){
	flagsUnset(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetAsleep(physRigidBody *const __RESTRICT__ body){
	body->flags &= PHYSICS_BODY_ASLEEP;
}
__FORCE_INLINE__ void physRigidBodySetAwake(physRigidBody *const __RESTRICT__ body, const flags_t flags){
	body->flags = flags;
}

__FORCE_INLINE__ void physRigidBodySimulateCollisions(physRigidBody *const __RESTRICT__ body){
	flagsSet(body->flags, PHYSICS_BODY_COLLIDE | PHYSICS_BODY_COLLISION_MODIFIED);
}
__FORCE_INLINE__ void physRigidBodySimulateLinear(physRigidBody *const __RESTRICT__ body){
	flagsSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR);
}
__FORCE_INLINE__ void physRigidBodySimulateAngular(physRigidBody *const __RESTRICT__ body){
	flagsSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR);
}

__FORCE_INLINE__ void physRigidBodyIgnoreCollisions(physRigidBody *const __RESTRICT__ body){
	flagsUnset(body->flags, PHYSICS_BODY_COLLIDE);
	flagsSet(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);
}
__FORCE_INLINE__ void physRigidBodyIgnoreLinear(physRigidBody *const __RESTRICT__ body){
	flagsUnset(body->flags, PHYSICS_BODY_SIMULATE_LINEAR);
}
__FORCE_INLINE__ void physRigidBodyIgnoreAngular(physRigidBody *const __RESTRICT__ body){
	flagsUnset(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR);
}
__FORCE_INLINE__ return_t physRigidBodyIsUninitialized(const physRigidBody *const __RESTRICT__ body){
	return flagsAreSet(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ return_t physRigidBodyIsSimulated(const physRigidBody *const __RESTRICT__ body){
	return flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE);
}
__FORCE_INLINE__ return_t physRigidBodyIsCollidable(const physRigidBody *const __RESTRICT__ body){
	return flagsAreSet(body->flags, PHYSICS_BODY_COLLIDE);
}
__FORCE_INLINE__ return_t physRigidBodyIsAsleep(physRigidBody *const __RESTRICT__ body){
	return body->flags;
}
__FORCE_INLINE__ return_t physRigidBodyWasInitialized(const physRigidBody *const __RESTRICT__ body){
	return flagsAreSet(body->flags, PHYSICS_BODY_INITIALIZED);
}

__HINT_INLINE__ void physRigidBodyApplyLinearForce(physRigidBody *const __RESTRICT__ body, const vec3 F){
	// Apply a linear force.
	body->netForce = vec3VAddV(body->netForce, F);
}

__HINT_INLINE__ void physRigidBodyApplyAngularForce(physRigidBody *const __RESTRICT__ body, const vec3 F, const vec3 r){
	// Apply an angular force in global space.
	// T = r x F
	body->netTorque = vec3VAddV(body->netTorque, vec3Cross(vec3VSubV(r, body->centroidGlobal), F));
}

__HINT_INLINE__ void physRigidBodyApplyForce(physRigidBody *const __RESTRICT__ body, const vec3 F, const vec3 r){

	// Accumulate the net force and torque.
	// r is where the force F is applied, in global space.

	// Accumulate torque.
	physRigidBodyApplyAngularForce(body, F, r);

	// Accumulate force.
	physRigidBodyApplyLinearForce(body, F);

}

__HINT_INLINE__ void physRigidBodyApplyVelocityImpulse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J){

	// Applies an impulse J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VAddV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VAddV(body->angularVelocity, mat3MMultVKet(body->inverseInertiaTensorGlobal, vec3Cross(x, J)));

	///}

}
__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J){

	// Applies an impulse -J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VSubV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VSubV(body->angularVelocity, mat3MMultVKet(body->inverseInertiaTensorGlobal, vec3Cross(x, J)));

	///}

}

__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseAngular(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J, const vec3 a){

	// Applies an impulse J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VAddV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VAddV(body->angularVelocity, mat3MMultVKet(body->inverseInertiaTensorGlobal, vec3VAddV(vec3Cross(x, J), a)));

	///}

}
__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseAngularInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J, const vec3 a){

	// Applies an impulse -J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VSubV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VSubV(body->angularVelocity, mat3MMultVKet(body->inverseInertiaTensorGlobal, vec3VAddV(vec3Cross(x, J), a)));

	///}

}

__HINT_INLINE__ void physRigidBodyApplyConfigurationImpulse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J){

	// Applies an impulse J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)// &&
			///(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)
		){
			body->centroidGlobal = vec3VAddV(body->centroidGlobal, vec3VMultS(J, body->inverseMass));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}
		// Angular impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)// &&
			///(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)
		){
			body->configuration.orientation = quatNormalizeFastAccurate(
				quatQAddQ(
					body->configuration.orientation,
					quatDifferentiate(
						body->configuration.orientation,
						mat3MMultVKet(
							body->inverseInertiaTensorGlobal,
							vec3Cross(x, J)
						)
					)
				)
			);
			physRigidBodyGenerateGlobalInertia(body);
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}

	///}

}
__HINT_INLINE__ void physRigidBodyApplyConfigurationImpulseInverse(physRigidBody *const __RESTRICT__ body, const vec3 x, const vec3 J){

	// Applies an impulse -J at point x in global space.

	///if(body->inverseMass > 0.f){

		// Linear impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)// &&
			///(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)
		){
			body->centroidGlobal = vec3VSubV(body->centroidGlobal, vec3VMultS(J, body->inverseMass));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}
		// Angular impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)// &&
			///(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)
		){
			body->configuration.orientation = quatNormalizeFastAccurate(
				quatQSubQ(
					body->configuration.orientation,
					quatDifferentiate(
						body->configuration.orientation,
						mat3MMultVKet(
							body->inverseInertiaTensorGlobal,
							vec3Cross(x, J)
						)
					)
				)
			);
			physRigidBodyGenerateGlobalInertia(body);
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}

	///}

}

__FORCE_INLINE__ void physRigidBodyCentroidFromPosition(physRigidBody *const __RESTRICT__ body){
	body->centroidGlobal = vec3VAddV(
		vec3VMultV(
			quatRotateVec3FastApproximate(
				body->configuration.orientation,
				#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
				body->centroidLocal
				#else
				body->base->centroid
				#endif
			),
			body->configuration.scale
		),
		body->configuration.position
	);
}

__FORCE_INLINE__ void physRigidBodyPositionFromCentroid(physRigidBody *const __RESTRICT__ body){
	body->configuration.position = vec3VAddV(
		vec3VMultV(
			quatRotateVec3FastApproximate(
				body->configuration.orientation,
				#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
				vec3Negate(body->centroidLocal)
				#else
				vec3Negate(body->base->centroid)
				#endif
			),
			body->configuration.scale
		),
		body->centroidGlobal
	);
}

#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
#ifdef PHYSICS_BODY_SCALE_INERTIA_TENSORS
__FORCE_INLINE__ static mat3 physRigidBodyScaleInertia(mat3 I, const vec3 scale){

	const float xy = scale.x * scale.y;
	const float xz = scale.x * scale.z;
	const float yz = scale.y * scale.z;

	float sqrZ = (I.m[0][0] + I.m[1][1] - I.m[2][2]) * 0.5f;
	const float sqrY = (I.m[0][0] - sqrZ) * scale.y * scale.y;
	const float sqrX = (I.m[1][1] - sqrZ) * scale.x * scale.x;
	sqrZ *= scale.z * scale.z;

	// I[0][0] = J[0] = y^2 + z^2
	// I[1][1] = J[1] = x^2 + z^2
	// I[2][2] = J[2] = x^2 + y^2
	//
	// Therefore:
	// x^2 = (J[1] - J[0] + J[2])/2
	// y^2 = (J[0] - J[1] + J[2])/2
	// z^2 = (J[0] - J[2] + J[1])/2
	//
	// To scale, multiply by the scale coefficient squared.
	I.m[0][0] = sqrY + sqrZ;
	I.m[1][1] = sqrX + sqrZ;
	I.m[2][2] = sqrX + sqrY;

	// I[0][1] = I[1][0] = J[3] -= x * y;
	// I[0][2] = I[2][0] = J[4] -= x * z;
	// I[1][2] = I[2][1] = J[5] -= y * z;
	//
	// To scale, just multiply each element by the
	// product of the two scale coefficients.
	I.m[0][1] *= xy;
	I.m[0][2] *= xz;
	I.m[1][2] *= yz;
	I.m[1][0] *= xy;
	I.m[2][0] *= xz;
	I.m[2][1] *= yz;

	// Inertia tensors are symmetric and positive definite --> invertible.
	return mat3Invert(I);

}
#else
void physRigidBodyScale(physRigidBody *const __RESTRICT__ body, const vec3 scale){

	// Inertia tensors are symmetric and positive definite --> invertible.
	mat3 I = mat3Invert(body->inverseInertiaTensorLocal);

	const float xy = scale.x * scale.y;
	const float xz = scale.x * scale.z;
	const float yz = scale.y * scale.z;

	float sqrZ = (I.m[0][0] + I.m[1][1] - I.m[2][2]) * 0.5f;
	const float sqrY = (I.m[0][0] - sqrZ) * scale.y * scale.y;
	const float sqrX = (I.m[1][1] - sqrZ) * scale.x * scale.x;
	sqrZ *= scale.z * scale.z;

	// I[0][0] = J[0] = y^2 + z^2
	// I[1][1] = J[1] = x^2 + z^2
	// I[2][2] = J[2] = x^2 + y^2
	//
	// Therefore:
	// x^2 = (J[1] - J[0] + J[2])/2
	// y^2 = (J[0] - J[1] + J[2])/2
	// z^2 = (J[0] - J[2] + J[1])/2
	//
	// To scale, multiply by the scale coefficient squared.
	I.m[0][0] = sqrY + sqrZ;
	I.m[1][1] = sqrX + sqrZ;
	I.m[2][2] = sqrX + sqrY;

	// I[0][1] = I[1][0] = J[3] -= x * y;
	// I[0][2] = I[2][0] = J[4] -= x * z;
	// I[1][2] = I[2][1] = J[5] -= y * z;
	//
	// To scale, just multiply each element by the
	// product of the two scale coefficients.
	I.m[0][1] *= xy;
	I.m[0][2] *= xz;
	I.m[1][2] *= yz;
	I.m[1][0] *= xy;
	I.m[2][0] *= xz;
	I.m[2][1] *= yz;

	body->inverseInertiaTensorLocal = mat3Invert(I);
	body->configuration.scale = vec3VMultV(body->configuration.scale, scale);

}
void physRigidBodySetScale(physRigidBody *const __RESTRICT__ body, const vec3 scale){

	// Inertia tensors are symmetric and positive definite --> invertible.
	mat3 I = mat3Invert(body->inverseInertiaTensorLocal);

	const vec3 scaleRelative = vec3VDivV(scale, body->configuration.scale);

	const float xy = scaleRelative.x * scaleRelative.y;
	const float xz = scaleRelative.x * scaleRelative.z;
	const float yz = scaleRelative.y * scaleRelative.z;

	float sqrZ = (I.m[0][0] + I.m[1][1] - I.m[2][2]) * 0.5f;
	const float sqrY = (I.m[0][0] - sqrZ) * scaleRelative.y * scaleRelative.y;
	const float sqrX = (I.m[1][1] - sqrZ) * scaleRelative.x * scaleRelative.x;
	sqrZ *= scaleRelative.z * scaleRelative.z;

	// I[0][0] = J[0] = y^2 + z^2
	// I[1][1] = J[1] = x^2 + z^2
	// I[2][2] = J[2] = x^2 + y^2
	//
	// Therefore:
	// x^2 = (J[1] - J[0] + J[2])/2
	// y^2 = (J[0] - J[1] + J[2])/2
	// z^2 = (J[0] - J[2] + J[1])/2
	//
	// To scale, multiply by the scale coefficient squared.
	I.m[0][0] = sqrY + sqrZ;
	I.m[1][1] = sqrX + sqrZ;
	I.m[2][2] = sqrX + sqrY;

	// I[0][1] = I[1][0] = J[3] -= x * y;
	// I[0][2] = I[2][0] = J[4] -= x * z;
	// I[1][2] = I[2][1] = J[5] -= y * z;
	//
	// To scale, just multiply each element by the
	// product of the two scale coefficients.
	I.m[0][1] *= xy;
	I.m[0][2] *= xz;
	I.m[1][2] *= yz;
	I.m[1][0] *= xy;
	I.m[2][0] *= xz;
	I.m[2][1] *= yz;

	body->inverseInertiaTensorLocal = mat3Invert(I);
	body->configuration.scale = scale;

}
#endif
#endif

__FORCE_INLINE__ void physRigidBodyGenerateGlobalInertia(physRigidBody *const __RESTRICT__ body){

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	const mat3 orientationMatrix = mat3Quaternion(body->configuration.orientation);
	const mat3 inverseOrientationMatrix = mat3Transpose(orientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	body->inverseInertiaTensorGlobal = mat3MMultM(
		inverseOrientationMatrix,
		mat3MMultM(
			#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
			#ifdef PHYSICS_BODY_SCALE_INERTIA_TENSORS
			physRigidBodyScaleInertia(body->inertiaTensorLocal, body->configuration.scale),
			#else
			body->inverseInertiaTensorLocal,
			#endif
			#else
			body->base->inverseInertiaTensor,
			#endif
			orientationMatrix
		)
	);

}

__FORCE_INLINE__ void physRigidBodyUpdateConfiguration(physRigidBody *const __RESTRICT__ body){
	if(flagsAreSet(body->flags, PHYSICS_BODY_TRANSFORMED)){
		physRigidBodyPositionFromCentroid(body);
	}
}

__FORCE_INLINE__ void physRigidBodyResetAccumulators(physRigidBody *const __RESTRICT__ body){
	// Reset force and torque accumulators.
	vec3ZeroP(&body->netForce);
	vec3ZeroP(&body->netTorque);
}

void physRigidBodyIntegrateVelocity(physRigidBody *const __RESTRICT__ body, const float dt){

	///if(body->inverseMass > 0.f){

		///const float modifier = body->inverseMass * dt;

		// Integrate linear velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)){
			// Apply damping.
			body->linearVelocity = vec3VMultS(vec3VAddV(body->linearVelocity, vec3VMultS(body->netForce, body->inverseMass*dt)),  1.f / (1.f + dt * body->linearDamping));
		}else{
			vec3ZeroP(&body->linearVelocity);
		}

		// Integrate angular velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)){

			// Update moment of inertia.
			physRigidBodyGenerateGlobalInertia(body);

			// Apply damping.
			body->angularVelocity = vec3VMultS(
				vec3VAddV(
					body->angularVelocity,
					vec3VMultS(
						mat3MMultVKet(
							body->inverseInertiaTensorGlobal,
							body->netTorque
						),
						dt
					)
				),
				1.f / (1.f + dt * body->angularDamping)
			);

		}else{
			mat3ZeroP(&body->inverseInertiaTensorGlobal);
			vec3ZeroP(&body->angularVelocity);
		}

	/**}else{

		mat3ZeroP(&body->inverseInertiaTensorGlobal);

	}**/

}

void physRigidBodyIntegrateConfiguration(physRigidBody *const __RESTRICT__ body, const float dt){

	///if(body->inverseMass > 0.f){

		// Integrate position.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)// &&
			///(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)
		){
			body->centroidGlobal = vec3VAddV(body->centroidGlobal, vec3VMultS(body->linearVelocity, dt));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}else{
			flagsUnset(body->flags, PHYSICS_BODY_TRANSLATED);
		}

		// Integrate orientation.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)// &&
			///(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)
		){
			body->configuration.orientation = quatNormalizeFastAccurate(quatIntegrate(body->configuration.orientation, body->angularVelocity, dt));
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}else{
			flagsUnset(body->flags, PHYSICS_BODY_ROTATED);
		}

	///}

}

__FORCE_INLINE__ void physRigidBodyIntegrateSymplecticEuler(physRigidBody *const __RESTRICT__ body, const float dt){

	// Integrate the body's velocities.
	physRigidBodyIntegrateVelocity(body, dt);
	physRigidBodyResetAccumulators(body);

	// Integrate the body's configuration.
	physRigidBodyIntegrateConfiguration(body, dt);

}

__FORCE_INLINE__ void physRigidBodyIntegrateLeapfrog(physRigidBody *const __RESTRICT__ body, const float dt){

	// "Kick".
	physRigidBodyIntegrateVelocity(body, dt*0.5f);

	// "Drift".
	physRigidBodyIntegrateConfiguration(body, dt);

	// "Kick".
	physRigidBodyIntegrateVelocity(body, dt*0.5f);
	physRigidBodyResetAccumulators(body);

}

__FORCE_INLINE__ void physRigidBodyIntegrateLeapfrogTest(physRigidBody *const __RESTRICT__ body, const float dt){

	if(physRigidBodyWasInitialized(body)){

		// "Kick".
		physRigidBodyIntegrateVelocity(body, dt*0.5f);

		// Remove the body's "just initialized" flag.
		flagsUnset(body->flags, PHYSICS_BODY_INITIALIZED);

	}else if(!physRigidBodyIsUninitialized(body)){

		// "Drift".
		physRigidBodyIntegrateConfiguration(body, dt);

		// "Kick-kick."
		physRigidBodyIntegrateVelocity(body, dt);

	}

	physRigidBodyResetAccumulators(body);

}

return_t physRigidBodyPermitCollision(const physRigidBody *const body1, const physRigidBody *const body2){

	// Check if two rigid bodies may collide.

	// Check if they share any joints that forbid collision.
	// "Ownership" of the joint is delegated to the body with
	// the greater address; while the joint will appear in
	// both arrays, it will be closer to the front in the
	// owner's array.

	if(body1 > body2){

		// Body 1's joints.
		unsigned int found = 0;
		const physJoint *i = body1->joints;
		while(i != NULL && body2 >= (physRigidBody *)i->bodyB){
			// Check if the child body is the same.
			if(body2 == (physRigidBody *)i->bodyB){
				if(flagsAreUnset(i->flags, PHYSICS_JOINT_COLLISION)){
					return 0;
				}
				found = 1;
			}else if(found){
				// Exit once we know for certain
				// the bodies may collide.
				return 1;
			}
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			i = (physJoint *)memQLinkNextA(i);
			#else
			i = i->nextA;
			#endif
		}

		return 1;

	}else if(body2 > body1){

		// Body 2's joints.
		unsigned int found = 0;
		const physJoint *i = body2->joints;
		while(i != NULL && body1 >= (physRigidBody *)i->bodyB){
			// Check if the child body is the same.
			if(body1 == (physRigidBody *)i->bodyB){
				if(flagsAreUnset(i->flags, PHYSICS_JOINT_COLLISION)){
					return 0;
				}
				found = 1;
			}else if(found){
				// Exit once we know for certain
				// the bodies may collide.
				return 1;
			}
			#ifdef PHYSICS_CONSTRAINT_USE_ALLOCATOR
			i = (physJoint *)memQLinkNextA(i);
			#else
			i = i->nextA;
			#endif
		}

		return 1;

	}

	return 0;

}

#ifdef PHYSICS_BODY_STORE_LOCAL_TENSORS
void physRigidBodyAddCollider(physRigidBody *const __RESTRICT__ body, physCollider *const c, const float **const vertexMassArray){

	// Adds a single collider to the body.

	float mass;
	float inverseMass;
	vec3 centroid;

	// Generate the new collider's mass properties.
	physColliderGenerateMass(&c->c, &mass, &inverseMass, &centroid, vertexMassArray);

	if(mass != 0.f){

		mat3 inertiaTensor;

		const vec3 difference = vec3VSubV(body->centroidLocal, centroid);
		const vec3 differenceWeighted = vec3VMultS(difference, mass);
		const vec3 differenceWeightedSquared = vec3VMultV(difference, differenceWeighted);

		// Generate and add the new collider's inverse moment
		// of inertia using the Parallel Axis Theorem.
		physColliderGenerateMoment(&c->c, &inertiaTensor, &centroid, vertexMassArray);

		// Translate the inertia tensor using the rigid body's centroid.
		inertiaTensor.m[0][0] += differenceWeightedSquared.y + differenceWeightedSquared.z;
		inertiaTensor.m[0][1] -= differenceWeighted.x * difference.y;
		inertiaTensor.m[0][2] -= differenceWeighted.x * difference.z;
		inertiaTensor.m[1][1] += differenceWeightedSquared.x + differenceWeightedSquared.z;
		inertiaTensor.m[1][2] -= differenceWeighted.y * difference.z;
		inertiaTensor.m[2][2] += differenceWeightedSquared.x + differenceWeightedSquared.y;

		#ifdef PHYSICS_BODY_SCALE_INERTIA_TENSORS
		body->inertiaTensorLocal.m[0][0] += inertiaTensor.m[0][0];
		body->inertiaTensorLocal.m[0][1] += inertiaTensor.m[0][1];
		body->inertiaTensorLocal.m[1][0] += inertiaTensor.m[0][1];
		body->inertiaTensorLocal.m[0][2] += inertiaTensor.m[0][2];
		body->inertiaTensorLocal.m[2][0] += inertiaTensor.m[0][2];
		body->inertiaTensorLocal.m[1][1] += inertiaTensor.m[1][1];
		body->inertiaTensorLocal.m[1][2] += inertiaTensor.m[1][2];
		body->inertiaTensorLocal.m[2][1] += inertiaTensor.m[1][2];
		body->inertiaTensorLocal.m[2][2] += inertiaTensor.m[2][2];
		#else
		body->inverseInertiaTensorLocal = mat3Invert(body->inverseInertiaTensorLocal);
		body->inverseInertiaTensorLocal.m[0][0] += inertiaTensor.m[0][0];
		body->inverseInertiaTensorLocal.m[0][1] += inertiaTensor.m[0][1];
		body->inverseInertiaTensorLocal.m[1][0] += inertiaTensor.m[0][1];
		body->inverseInertiaTensorLocal.m[0][2] += inertiaTensor.m[0][2];
		body->inverseInertiaTensorLocal.m[2][0] += inertiaTensor.m[0][2];
		body->inverseInertiaTensorLocal.m[1][1] += inertiaTensor.m[1][1];
		body->inverseInertiaTensorLocal.m[1][2] += inertiaTensor.m[1][2];
		body->inverseInertiaTensorLocal.m[2][1] += inertiaTensor.m[1][2];
		body->inverseInertiaTensorLocal.m[2][2] += inertiaTensor.m[2][2];
		body->inverseInertiaTensorLocal = mat3Invert(body->inverseInertiaTensorLocal);
		#endif

		// Calculate the new, weighted centroid.
		body->inverseMass = 1.f / (body->mass + mass);
		body->centroidLocal = vec3VMultS(vec3VAddV(vec3VMultS(body->centroidLocal, body->mass), vec3VMultS(centroid, mass)), body->inverseMass);
		body->mass += mass;

	}

}
#endif

void physRigidBodyDelete(physRigidBody *const __RESTRICT__ body){
	modulePhysicsColliderFreeArray(&body->hull);
}
