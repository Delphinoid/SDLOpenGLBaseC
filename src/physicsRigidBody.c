#include "modulePhysics.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PHYSICS_RESOURCE_DIRECTORY_STRING "Resources\\Skeletons\\Physics\\"
#define PHYSICS_RESOURCE_DIRECTORY_LENGTH 28

void physRigidBodyBaseInit(physRigidBodyBase *const restrict local){
	local->id = (physicsBodyIndex_t)-1;
	local->flags = PHYSICS_BODY_ASLEEP;
	local->hull = NULL;
	local->inverseMass = 0.f;
	local->linearDamping = 0.f;
	local->angularDamping = 0.f;
	vec3Zero(&local->centroid);
	mat3Identity(&local->inverseInertiaTensor);
}

__FORCE_INLINE__ void physRigidBodyBaseGenerateProperties(physRigidBodyBase *const restrict local, const float **const vertexMassArray){

	/*
	** Calculates the rigid body's total mass, inverse mass,
	** centroid and inertia tensor, as well as the mass
	** properties for each of its colliders.
	*/

	physCollider *c;
	const float **m;

	float tempMass = 0.f;
	vec3 tempCentroid = {.x = 0.f, .y = 0.f, .z = 0.f};
	float tempInertiaTensor[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};


	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	c = local->hull;
	m = vertexMassArray;
	while(c != NULL){

		float colliderMass;
		float colliderInverseMass;
		vec3 colliderCentroid;

		physColliderGenerateMass(&c->c, &colliderMass, &colliderInverseMass, &colliderCentroid, m);

		tempCentroid.x += colliderCentroid.x * colliderMass;
		tempCentroid.y += colliderCentroid.y * colliderMass;
		tempCentroid.z += colliderCentroid.z * colliderMass;
		tempMass += colliderMass;

		c = (physCollider *)memSLinkNext(c);
		if(m != NULL){
			++m;
		}

	}

	if(tempMass != 0.f){
		const float tempInverseMass = 1.f / tempMass;
		tempCentroid.x *= tempInverseMass;
		tempCentroid.y *= tempInverseMass;
		tempCentroid.z *= tempInverseMass;
		local->inverseMass = tempInverseMass;
	}else{
		local->inverseMass = 0.f;
	}
	local->centroid = tempCentroid;


	// Calculate the combined moment of inertia for the
	// collider as the sum of its collider's moments.
	c = local->hull;
	m = vertexMassArray;
	while(c != NULL){

		mat3 colliderInertiaTensor;
		physColliderGenerateMoment(&c->c, &colliderInertiaTensor, &tempCentroid, m);

		tempInertiaTensor[0] += colliderInertiaTensor.m[0][0];
		tempInertiaTensor[1] += colliderInertiaTensor.m[1][1];
		tempInertiaTensor[2] += colliderInertiaTensor.m[2][2];
		tempInertiaTensor[3] += colliderInertiaTensor.m[0][1];
		tempInertiaTensor[4] += colliderInertiaTensor.m[0][2];
		tempInertiaTensor[5] += colliderInertiaTensor.m[1][2];

		c = (physCollider *)memSLinkNext(c);
		if(m != NULL){
			++m;
		}

	}

	local->inverseInertiaTensor.m[0][0] = tempInertiaTensor[0];
	local->inverseInertiaTensor.m[1][1] = tempInertiaTensor[1];
	local->inverseInertiaTensor.m[2][2] = tempInertiaTensor[2];
	local->inverseInertiaTensor.m[0][1] = tempInertiaTensor[3];
	local->inverseInertiaTensor.m[0][2] = tempInertiaTensor[4];
	local->inverseInertiaTensor.m[1][2] = tempInertiaTensor[5];
	// No point calculating the same numbers twice.
	local->inverseInertiaTensor.m[1][0] = tempInertiaTensor[3];
	local->inverseInertiaTensor.m[2][0] = tempInertiaTensor[4];
	local->inverseInertiaTensor.m[2][1] = tempInertiaTensor[5];

	mat3Invert(&local->inverseInertiaTensor);

}

/** TEMPORARY **/
static return_t physColliderResizeToFit(physCollider *const restrict local){

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

return_t physRigidBodyBaseLoad(physRigidBodyBase **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath){

	/*
	** Loads a series of rigid bodies.
	**
	** If skeleton is not NULL, it constrains them using
	** the specified bone names.
	*/

	return_t success = 0;

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *restrict rbInfo;

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), PHYSICS_RESOURCE_DIRECTORY_STRING, PHYSICS_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);
	rbInfo = fopen(fullPath, "r");

	if(rbInfo != NULL){

		char lineFeed[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

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
								if(end != &line[0]){
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
						modulePhysicsRigidBodyBaseFreeArray(bodies);
						fclose(rbInfo);
						return -1;
					}
					physRigidBodyBaseInit(currentBody);
					currentBody->id = currentBodyID;
					currentBodyColliderNum = 0;
					currentBody->flags = PHYSICS_BODY_DEFAULT_STATE;
					currentCommand = 0;

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
								if(edgeCapacity == 0){
									edgeCapacity = 3;
								}else{
									edgeCapacity *= 2;
								}
								cMeshEdge *const tempBuffer = memReallocate(cHull->edges, edgeCapacity*sizeof(cMeshEdge));
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
									modulePhysicsRigidBodyBaseFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
								cHull->edges = tempBuffer;
							}

							start = end;
							end = strtoul(token, NULL, 0)-1;

							// Look for a twin edge. If it is found,
							// the current edge will be stored directly
							// after it.
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
							vec3SubVFromVR(&cHull->vertices[cHull->edges[first].start], &cHull->vertices[cHull->edges[first].end], &BsA);
							if(lastTwin){
								vec3SubVFromVR(&cHull->vertices[cHull->edges[last].end], &cHull->vertices[cHull->edges[first].end], &CsA);
							}else{
								vec3SubVFromVR(&cHull->vertices[cHull->edges[last].start], &cHull->vertices[cHull->edges[first].end], &CsA);
							}
						}else{
							vec3SubVFromVR(&cHull->vertices[cHull->edges[first].end], &cHull->vertices[cHull->edges[first].start], &BsA);
							if(lastTwin){
								vec3SubVFromVR(&cHull->vertices[cHull->edges[last].end], &cHull->vertices[cHull->edges[first].start], &CsA);
							}else{
								vec3SubVFromVR(&cHull->vertices[cHull->edges[last].start], &cHull->vertices[cHull->edges[first].start], &CsA);
							}
						}
						vec3CrossR(&BsA, &CsA, &cHull->normals[cHull->faceNum]);
						vec3NormalizeFastAccurate(&cHull->normals[cHull->faceNum]);

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

void physRigidBodyBaseDelete(physRigidBodyBase *const restrict local){
	modulePhysicsColliderFreeArray(&local->hull);
}

void physRigidBodyInit(physRigidBody *const restrict body){
	body->flags = PHYSICS_BODY_ASLEEP;
	body->base = NULL;
	body->hull = NULL;
	body->configuration = NULL;
	vec3Zero(&body->linearVelocity);
	vec3Zero(&body->angularVelocity);
	vec3Zero(&body->netForce);
	vec3Zero(&body->netTorque);
}

return_t physRigidBodyInstantiate(physRigidBody *const restrict body, physRigidBodyBase *const restrict local, bone *const restrict configuration){

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
	body->inverseMass = local->inverseMass;
	body->linearDamping = local->linearDamping;
	body->angularDamping = local->angularDamping;
	body->centroidLocal = local->centroid;
	body->inverseInertiaTensorLocal = local->inverseInertiaTensor;

	body->base = local;
	body->configuration = configuration;
	body->flags = local->flags;

	return 1;

}

__FORCE_INLINE__ void physRigidBodySetInitialized(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetAsleep(physRigidBody *const restrict body){
	body->flags = 0;
}
__FORCE_INLINE__ void physRigidBodySetAwake(physRigidBody *const restrict body, const flags_t flags){
	body->flags = flags;
}

__FORCE_INLINE__ void physRigidBodySimulateCollisions(physRigidBody *const restrict body){
	flagsSet(body->flags, PHYSICS_BODY_COLLIDE | PHYSICS_BODY_COLLISION_MODIFIED);
}
__FORCE_INLINE__ void physRigidBodySimulateLinear(physRigidBody *const restrict body){
	flagsSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR);
}
__FORCE_INLINE__ void physRigidBodySimulateAngular(physRigidBody *const restrict body){
	flagsSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR);
}

__FORCE_INLINE__ void physRigidBodyIgnoreCollisions(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_COLLIDE);
	flagsSet(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);
}
__FORCE_INLINE__ void physRigidBodyIgnoreLinear(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_SIMULATE_LINEAR);
}
__FORCE_INLINE__ void physRigidBodyIgnoreAngular(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR);
}

__FORCE_INLINE__ return_t physRigidBodyIsUninitialized(const physRigidBody *const restrict body){
	return flagsAreSet(body->flags, PHYSICS_BODY_UNINITIALIZED);
}

__FORCE_INLINE__ return_t physRigidBodyIsSimulated(const physRigidBody *const restrict body){
	return flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE);
}

__FORCE_INLINE__ return_t physRigidBodyIsCollidable(const physRigidBody *const restrict body){
	return flagsAreSet(body->flags, PHYSICS_BODY_COLLIDE);
}
__FORCE_INLINE__ return_t physRigidBodyIsAsleep(physRigidBody *const restrict body){
	return body->flags;
}

__FORCE_INLINE__ return_t physRigidBodyUpdateColliders(physRigidBody *const restrict body, physIsland *const restrict island){

	/*
	** Transform the vertices of each body into global space.
	** If the body is set to not collide, remove the colliders
	** from the island that they are a part of.
	*/

	if(physRigidBodyIsCollidable(body)){

		// Update each collider.
		physCollider *c = body->hull;
		while(c != NULL){
			if(physColliderTransform(c, island) < 0){
				/** Memory allocation failure. **/
				return -1;
			}
			c = (physCollider *)memSLinkNext(c);
		}

		flagsUnset(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);

	}else if(flagsAreSet(body->flags, PHYSICS_BODY_COLLISION_MODIFIED)){

		// Remove each collider.
		physCollider *c = body->hull;
		while(c != NULL){
			physIslandRemoveCollider(island, c);
			c = (physCollider *)memSLinkNext(c);
		}

		flagsUnset(body->flags, PHYSICS_BODY_COLLISION_MODIFIED);

	}

	return 1;

}

void physRigidBodyApplyLinearForce(physRigidBody *const restrict body, const vec3 *const restrict F){
	/*
	** Apply a linear force.
	*/
	body->netForce.x += F->x;
	body->netForce.y += F->y;
	body->netForce.z += F->z;
}

void physRigidBodyApplyAngularForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r){
	/*
	** Apply an angular force.
	*/
	// T = r x F
	vec3 rsR, rxF;
	vec3SubVFromVR(r, &body->centroidGlobal, &rsR);
	vec3CrossR(&rsR, F, &rxF);
	vec3AddVToV(&body->netTorque, &rxF);
}

void physRigidBodyApplyForceGlobal(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in world space.
	*/

	// Accumulate torque.
	physRigidBodyApplyAngularForceGlobal(body, F, r);

	// Accumulate force.
	physRigidBodyApplyLinearForce(body, F);

}

static __FORCE_INLINE__ void physRigidBodyCentroidFromPosition(physRigidBody *const restrict body){
	quatRotateVec3FastR(&body->configuration->orientation, &body->centroidLocal, &body->centroidGlobal);
	vec3AddVToV(&body->centroidGlobal, &body->configuration->position);
}

static __FORCE_INLINE__ void physRigidBodyPositionFromCentroid(physRigidBody *const restrict body){
	body->configuration->position.x = -body->centroidLocal.x;
	body->configuration->position.y = -body->centroidLocal.y;
	body->configuration->position.z = -body->centroidLocal.z;
	quatRotateVec3Fast(&body->configuration->orientation, &body->configuration->position);
	vec3AddVToV(&body->configuration->position, &body->centroidGlobal);
}

static __FORCE_INLINE__ void physRigidBodyGenerateGlobalInertia(physRigidBody *const restrict body){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &body->configuration->orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &body->inverseInertiaTensorLocal, &body->inverseInertiaTensorGlobal);
	mat3MultMByM1(&body->inverseInertiaTensorGlobal, &inverseOrientationMatrix);

}

void physRigidBodyIntegrateVelocity(physRigidBody *const restrict body, const float dt){

	// Update moment of inertia.
	physRigidBodyGenerateGlobalInertia(body);

	if(body->inverseMass > 0.f){

		const float modifier = body->inverseMass * dt;

		// Integrate linear velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)){
			body->linearVelocity.x += body->netForce.x * modifier;
			body->linearVelocity.y += body->netForce.y * modifier;
			body->linearVelocity.z += body->netForce.z * modifier;
			// Apply damping.
			vec3MultVByS(&body->linearVelocity,  1.f / (1.f + dt * body->linearDamping));
		}else{
			vec3Zero(&body->linearVelocity);
		}

		// Integrate angular velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)){
			vec3 momentum;
			mat3MultMByVBraR(&body->inverseInertiaTensorGlobal, &body->netTorque, &momentum);
			body->angularVelocity.x += momentum.x * dt;
			body->angularVelocity.y += momentum.y * dt;
			body->angularVelocity.z += momentum.z * dt;
			// Apply damping.
			vec3MultVByS(&body->angularVelocity, 1.f / (1.f + dt * body->angularDamping));
		}else{
			vec3Zero(&body->angularVelocity);
		}

	}

	// Reset force and torque accumulators.
	vec3Zero(&body->netForce);
	vec3Zero(&body->netTorque);

	// Update the centroid from the position.
	if(flagsAreSet(body->flags, PHYSICS_BODY_UNINITIALIZED | PHYSICS_BODY_TRANSFORMED)){
		physRigidBodyCentroidFromPosition(body);
	}

}

void physRigidBodyIntegrateConfiguration(physRigidBody *const restrict body, const float dt){

	// Integrate position.
	if(
		flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR) &&
		(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)
	){
		body->centroidGlobal.x += body->linearVelocity.x * dt;
		body->centroidGlobal.y += body->linearVelocity.y * dt;
		body->centroidGlobal.z += body->linearVelocity.z * dt;
		flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
	}else{
		flagsUnset(body->flags, PHYSICS_BODY_TRANSLATED);
	}

	// Integrate orientation.
	if(
		flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR) &&
		(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)
	){
		quatIntegrate(&body->configuration->orientation, &body->angularVelocity, dt);
		flagsSet(body->flags, PHYSICS_BODY_ROTATED);
	}else{
		flagsUnset(body->flags, PHYSICS_BODY_ROTATED);
	}

	// Update the position from the centroid.
	if(flagsAreSet(body->flags, PHYSICS_BODY_TRANSFORMED)){
		physRigidBodyPositionFromCentroid(body);
	}

}

return_t physRigidBodyAddConstraint(physRigidBody *const restrict body, physConstraint *const c){

	/*
	** Sort a new constraint into the body.
	*/

	///

}

void physRigidBodyDelete(physRigidBody *const restrict body){
	modulePhysicsColliderFreeArray(&body->hull);
}
