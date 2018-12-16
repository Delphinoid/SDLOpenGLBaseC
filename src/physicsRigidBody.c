#include "modulePhysics.h"
#include "memoryManager.h"
#include "colliderConvexMesh.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define PHYSICS_RESOURCE_DIRECTORY_STRING "Resources\\Skeletons\\Physics\\"
#define PHYSICS_RESOURCE_DIRECTORY_LENGTH 28

#define PHYSICS_INTEGRATION_STEPS_EULER      2
#define PHYSICS_INTEGRATION_STEPS_RUNGEKUTTA 4

#define PHYSICS_RESTING_EPSILON 0.0001f
#define PHYSICS_BAUMGARTE_TERM  0.0001f

void physRigidBodyInit(physRigidBody *const restrict body){
	body->id = (physicsBodyIndex_t)-1;
	body->flags = PHYSICS_BODY_INACTIVE;
	body->colliders = NULL;
	body->mass = 0.f;
	body->inverseMass = 0.f;
	body->coefficientOfRestitution = 1.f;
	vec3SetS(&body->centroid, 0.f);
	mat3Identity(&body->inertiaTensor);
	body->constraints = NULL;
}

void physRigidBodyGenerateMassProperties(physRigidBody *const restrict body, float **const vertexMassArrays){

	/*
	** Calculates the rigid body's total mass, inverse mass,
	** centroid and inertia tensor, as well as the mass
	** properties for each of its colliders.
	*/

	physCollider *i;
	physColliderIndex_t j;
	float *colliderMassArray;
	float tempInertiaTensor[6];

	body->mass = 0.f;
	vec3SetS(&body->centroid, 0.f);
	body->inertiaTensor.m[0][0] = 0.f; body->inertiaTensor.m[0][1] = 0.f; body->inertiaTensor.m[0][2] = 0.f;
	body->inertiaTensor.m[1][0] = 0.f; body->inertiaTensor.m[1][1] = 0.f; body->inertiaTensor.m[1][2] = 0.f;
	body->inertiaTensor.m[2][0] = 0.f; body->inertiaTensor.m[2][1] = 0.f; body->inertiaTensor.m[2][2] = 0.f;

	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	j = 0;
	i = body->colliders;
	while(i != NULL){

		float colliderMass;
		if(vertexMassArrays != NULL){
			colliderMassArray = vertexMassArrays[j];
		}else{
			colliderMassArray = NULL;
		}
		colliderMass = physColliderGenerateMass(i, colliderMassArray);

		body->centroid.x += i->centroid.x * colliderMass;
		body->centroid.y += i->centroid.y * colliderMass;
		body->centroid.z += i->centroid.z * colliderMass;
		body->mass += colliderMass;

		i = modulePhysicsColliderNext(i);
		++j;

	}

	if(body->mass != 0.f){
		body->inverseMass = 1.f / body->mass;
		body->centroid.x *= body->inverseMass;
		body->centroid.y *= body->inverseMass;
		body->centroid.z *= body->inverseMass;
	}else{
		body->inverseMass = 0.f;
	}

	// Calculate the combined moment of inertia for the body
	// as the sum of its collider's moments.
	j = 0;
	i = body->colliders;
	while(i != NULL){

		if(vertexMassArrays[j] != NULL){
			colliderMassArray = vertexMassArrays[j];
		}else{
			colliderMassArray = NULL;
		}
		physColliderGenerateMoment(i, &body->centroid, colliderMassArray, tempInertiaTensor);

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		body->inertiaTensor.m[0][0] += tempInertiaTensor[0];// * avgVertexMass;
		body->inertiaTensor.m[1][1] += tempInertiaTensor[1];// * avgVertexMass;
		body->inertiaTensor.m[2][2] += tempInertiaTensor[2];// * avgVertexMass;
		body->inertiaTensor.m[0][1] += tempInertiaTensor[3];// * avgVertexMass;
		body->inertiaTensor.m[0][2] += tempInertiaTensor[4];// * avgVertexMass;
		body->inertiaTensor.m[1][2] += tempInertiaTensor[5];// * avgVertexMass;

		i = modulePhysicsColliderNext(i);
		++j;

	}

	// No point calculating the same numbers twice.
	body->inertiaTensor.m[1][0] = body->inertiaTensor.m[0][1];
	body->inertiaTensor.m[2][0] = body->inertiaTensor.m[0][2];
	body->inertiaTensor.m[2][1] = body->inertiaTensor.m[1][2];

	//mat3Invert(&body->localInverseInertiaTensor);

}

static return_t physColliderResizeToFit(physCollider *const restrict collider, float **const vertexMassArrays){

	cMesh *cHull = (cMesh *)&collider->c.hull;

	if(cHull->vertexNum != 0){
		float *tempBuffer2;
		vec3 *tempBuffer1 = memReallocate(cHull->vertices, cHull->vertexNum*sizeof(vec3));
		if(tempBuffer1 == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tempBuffer2 = memReallocate(*vertexMassArrays, cHull->vertexNum*sizeof(float));
		if(tempBuffer2 == NULL){
			/** Memory allocation failure. **/
			memFree(tempBuffer1);
			return -1;
		}
		cHull->vertices = tempBuffer1;
		*vertexMassArrays = tempBuffer2;
	}else{
		if(cHull->vertices != NULL){
			memFree(cHull->vertices);
			cHull->vertices = NULL;
		}
		if(*vertexMassArrays != NULL){
			memFree(*vertexMassArrays);
			*vertexMassArrays = NULL;
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
		cMeshEdge *tempBuffer = memReallocate(cHull->edges, cHull->edgeNum*sizeof(cMeshEdge));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		cHull->edges = tempBuffer;
	}else if(cHull->edges != NULL){
		memFree(cHull->edges);
		cHull->edges = NULL;
	}

	return 1;

}

/** FIX CONSTRAINT LOADING. MOSTLY TEMPORARY. **/
return_t physRigidBodyLoad(physRigidBody **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath){

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
		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = rigid body, 1 = collider, 2 = constraint).
		fileLine_t currentLine = 0;  // Current file line being read.

		physRigidBody *currentBody = NULL;
		physColliderIndex_t currentBodyColliderNum = 0;
		physCollider *currentCollider = NULL;
		physConstraint *currentConstraint = NULL;

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
							physRigidBodyGenerateMassProperties(currentBody, vertexMassArrays);
							// Free the collider mass arrays.
							if(vertexMassArrays != NULL){
								physColliderIndex_t i;
								for(i = 0; i < currentBodyColliderNum; ++i){
									if(vertexMassArrays[i] != NULL){
										memFree(vertexMassArrays[i]);
									}
								}
								memFree(vertexMassArrays);
								vertexMassArrays = NULL;
							}
							if(!isSkeleton){
								break;
							}
						}else if(currentCommand == 1){
							if(physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum-1]) == -1){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									physColliderIndex_t k;
									for(k = 0; k < currentBodyColliderNum; ++k){
										if(vertexMassArrays[k] != NULL){
											memFree(vertexMassArrays[k]);
										}
									}
									memFree(vertexMassArrays);
								}
								modulePhysicsRigidBodyFreeArray(bodies);
								fclose(rbInfo);
								return -1;
							}
						}
					}
					// Initialize a new rigid body.
					currentBody = modulePhysicsRigidBodyAppend(bodies);
					if(currentBody == NULL){
						/** Memory allocation failure. **/
						if(vertexMassArrays != NULL){
							physColliderIndex_t k;
							for(k = 0; k < currentBodyColliderNum; ++k){
								if(vertexMassArrays[k] != NULL){
									memFree(vertexMassArrays[k]);
								}
							}
							memFree(vertexMassArrays);
						}
						modulePhysicsRigidBodyFreeArray(bodies);
						fclose(rbInfo);
						return -1;
					}
					physRigidBodyInit(currentBody);
					currentBody->id = currentBodyID;
					currentBodyColliderNum = 0;
					currentBody->flags = PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_COLLIDE;
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
								if(physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum-1]) == -1){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										physColliderIndex_t k;
										for(k = 0; k < currentBodyColliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												memFree(vertexMassArrays[k]);
											}
										}
										memFree(vertexMassArrays);
									}
									modulePhysicsRigidBodyFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						// Allocate memory for the new collider.
						currentCollider = modulePhysicsColliderAppend(&currentBody->colliders);
						if(currentCollider == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								physColliderIndex_t k;
								for(k = 0; k < currentBodyColliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										memFree(vertexMassArrays[k]);
									}
								}
								memFree(vertexMassArrays);
							}
							modulePhysicsRigidBodyFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer = memReallocate(vertexMassArrays, (currentBodyColliderNum+1)*sizeof(float *));
						if(tempBuffer == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								physColliderIndex_t k;
								for(k = 0; k < currentBodyColliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										memFree(vertexMassArrays[k]);
									}
								}
								memFree(vertexMassArrays);
							}
							modulePhysicsRigidBodyFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}

						vertexMassArrays = tempBuffer;
						vertexMassArrays[currentBodyColliderNum] = NULL;
						vertexCapacity = 0;
						normalCapacity = 0;
						edgeCapacity = 0;

						cInit(&currentCollider->c, COLLIDER_TYPE_MESH);
						cMeshInit((cMesh *)&currentCollider->c.hull);
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

					cMesh *const cHull = (cMesh *)&currentCollider->c.hull;
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
								physColliderIndex_t k;
								for(k = 0; k < currentBodyColliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										memFree(vertexMassArrays[k]);
									}
								}
								memFree(vertexMassArrays);
							}
							modulePhysicsRigidBodyFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer2 = memReallocate(vertexMassArrays[currentBodyColliderNum-1], vertexCapacity*sizeof(float));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								physColliderIndex_t k;
								for(k = 0; k < currentBodyColliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										memFree(vertexMassArrays[k]);
									}
								}
								memFree(vertexMassArrays);
							}
							modulePhysicsRigidBodyFreeArray(bodies);
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

						cMesh *const cHull = (cMesh *)&currentCollider->c.hull;

						cEdgeIndex_t i = 0;

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
										physColliderIndex_t k;
										for(k = 0; k < currentBodyColliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												memFree(vertexMassArrays[k]);
											}
										}
										memFree(vertexMassArrays);
									}
									modulePhysicsRigidBodyFreeArray(bodies);
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
									physColliderIndex_t k;
									for(k = 0; k < currentBodyColliderNum; ++k){
										if(vertexMassArrays[k] != NULL){
											memFree(vertexMassArrays[k]);
										}
									}
									memFree(vertexMassArrays);
								}
								modulePhysicsRigidBodyFreeArray(bodies);
								fclose(rbInfo);
								return -1;
							}
							tempBuffer2 = memReallocate(cHull->faces, normalCapacity*sizeof(cMeshFace));
							if(tempBuffer2 == NULL){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									physColliderIndex_t k;
									for(k = 0; k < currentBodyColliderNum; ++k){
										if(vertexMassArrays[k] != NULL){
											memFree(vertexMassArrays[k]);
										}
									}
									memFree(vertexMassArrays);
								}
								modulePhysicsRigidBodyFreeArray(bodies);
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
						vec3Cross(&BsA, &CsA, &cHull->normals[cHull->faceNum]);
						vec3NormalizeFastAccurate(&cHull->normals[cHull->faceNum]);

						//cHull->faces[cHull->faceNum].edgeNum = addNum;
						if(addNum > cHull->edgeMax){
							// Update the maximum edge num.
							cHull->edgeMax = addNum;
						}
						cHull->faces[cHull->faceNum] = first;

						++cHull->faceNum;

					}else{
						printf("Error loading rigid bodies \"%s\": Collider face at line %u "
						       "must have at least three vertices.\n", fullPath, currentLine);
					}

				}


			// Constraint
			}else if(lineLength >= 12 && strncmp(line, "constraint ", 11) == 0){

				const char *end;
				while(line[10] == ' ' || line[10] == '\t'){
					++line;
					--lineLength;
				}

				// Make sure a brace exists.
				end = strrchr(line+10, '{');
				if(end){
					if(currentCommand >= 0){

						// Close any current commands.
						if(currentCommand > 0){
							printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
							       "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
							if(currentCommand == 1){
								if(physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum-1]) == -1){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										physColliderIndex_t k;
										for(k = 0; k < currentBodyColliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												memFree(vertexMassArrays[k]);
											}
										}
										memFree(vertexMassArrays);
									}
									modulePhysicsRigidBodyFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						// Check if the rigid body has a name.
						if(end != &line[10] && skl != NULL && isSkeleton){

							physicsBodyIndex_t constrainedBodyID;

							// Parse the name.
							while(*end == ' ' || *end == '\t'){
								--end;
							}
							lineLength = end-line;
							if(line[10] == '"'){
								while(*end != '"'){
									--end;
								}
								if(end != &line[10]){
									++line;
									lineLength = end-line;
								}
							}

							// Loop through the skeleton to find a bone with the given name.
							for(constrainedBodyID = 0; constrainedBodyID < skl->boneNum; ++constrainedBodyID){
								if(strncmp(line, skl->bones[constrainedBodyID].name, lineLength) == 0){
									break;
								}
							}
							// Add the new constraint if possible.
							if(constrainedBodyID < skl->boneNum){
								currentConstraint = modulePhysicsConstraintAppend(&currentBody->constraints);
								if(currentConstraint == NULL){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										physColliderIndex_t k;
										for(k = 0; k < currentBodyColliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												memFree(vertexMassArrays[k]);
											}
										}
										memFree(vertexMassArrays);
									}
									modulePhysicsRigidBodyFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
								physConstraintInit(currentConstraint);
								currentConstraint->ownerID = currentBody->id;
								currentConstraint->constraintID = constrainedBodyID;
								currentCommand = 2;
								success = 1;
							}

						}else{
							printf("Error loading rigid bodies \"%s\": The \"constraint\" command at like %u may only be used by. "
							       "physics skeletons. Ignoring.\n", fullPath, currentLine);
						}

					}else{
						printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"constraint\" invoked on line %u "
						       "without specifying a rigid body.\n", fullPath, currentLine);
					}
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"constraint\" at line %u "
					       "does not contain a brace.\n", fullPath, currentLine);
				}


			// Constraint offset bounds
			}else if(lineLength >= 24 && strncmp(line, "offsetBounds ", 13) == 0){
				if(currentCommand == 2){
					const char *token = strtok(line+13, "/");
					currentConstraint->constraintOffsetMin.x = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintOffsetMin.y = strtod(token, NULL);
					token = strtok(NULL, " ");
					currentConstraint->constraintOffsetMin.z = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintOffsetMax.x = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintOffsetMax.y = strtod(token, NULL);
					token = strtok(NULL, " ");
					currentConstraint->constraintOffsetMax.z = strtod(token, NULL);
				}else{
					printf("Error loading rigid bodies \"%s\": Constraint sub-command \"offsetBounds\" invoked on line %u "
					       "without specifying a constraint.\n", fullPath, currentLine);
				}


			// Constraint rotation bounds
			}else if(lineLength >= 26 && strncmp(line, "rotationBounds ", 15) == 0){
				if(currentCommand == 2){
					const char *token = strtok(line+15, "/");
					currentConstraint->constraintRotationMin.x = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintRotationMin.y = strtod(token, NULL);
					token = strtok(NULL, " ");
					currentConstraint->constraintRotationMin.z = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintRotationMax.x = strtod(token, NULL);
					token = strtok(NULL, "/");
					currentConstraint->constraintRotationMax.y = strtod(token, NULL);
					token = strtok(NULL, " ");
					currentConstraint->constraintRotationMax.z = strtod(token, NULL);
				}else{
					printf("Error loading rigid bodies \"%s\": Constraint sub-command \"rotationBounds\" invoked on line %u "
					       "without specifying a constraint.\n", fullPath, currentLine);
				}


			// Coefficient of restitution
			}else if(lineLength >= 13 && strncmp(line, "energyratio ", 12) == 0){
				if(currentCommand == 0){
					currentBody->coefficientOfRestitution = strtof(line+12, NULL);
					if(currentBody->coefficientOfRestitution > 1.f){
						currentBody->coefficientOfRestitution = 1.f;
					}else if(currentBody->coefficientOfRestitution < 0.f){
						currentBody->coefficientOfRestitution = 0.f;
					}
				}else if(currentCommand > 0){
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"energyratio\" at line %u does not belong "
					       "in any other multiline command.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"energyratio\" invoked on line %u without "
					       "specifying a rigid body.\n", fullPath, currentLine);
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
					if(strtoul(line+10, NULL, 0)){
						flagsSet(currentConstraint->flags, PHYSICS_CONSTRAINT_COLLIDE);
					}else{
						flagsUnset(currentConstraint->flags, PHYSICS_CONSTRAINT_COLLIDE);
					}
				}else if(currentCommand > 1){
					printf("Error loading rigid bodies \"%s\": Rigid body and constraint sub-command \"collision\" at line %u does not belong "
					       "in any other multiline commands.\n", fullPath, currentLine);
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body and constraint sub-command \"collision\" invoked on line %u without "
					       "specifying a rigid body or constraint.\n", fullPath, currentLine);
				}


			// Active flag
			}else if(lineLength >= 8 && strncmp(line, "active ", 7) == 0){
				if(currentCommand == 0){
					if(strtoul(line+7, NULL, 0)){
						flagsSet(currentBody->flags, PHYSICS_BODY_INITIALIZE);
					}else{
						flagsUnset(currentBody->flags, PHYSICS_BODY_INITIALIZE);
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
						physRigidBodyGenerateMassProperties(currentBody, vertexMassArrays);

						// Free the collider mass arrays.
						if(vertexMassArrays != NULL){
							physColliderIndex_t i;
							for(i = 0; i < currentBodyColliderNum; ++i){
								if(vertexMassArrays[i] != NULL){
									memFree(vertexMassArrays[i]);
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

					const cMesh *const cHull = (cMesh *)&currentCollider->c.hull;

					if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

						if(physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum-1]) == -1){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								physColliderIndex_t k;
								for(k = 0; k < currentBodyColliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										memFree(vertexMassArrays[k]);
									}
								}
								memFree(vertexMassArrays);
							}
							modulePhysicsRigidBodyFreeArray(bodies);
							return -1;
						}

					}else{
						printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
						--currentBodyColliderNum;
						physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum]);
					}
					currentCommand = 0;

				}else if(currentCommand == 2){
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
				physRigidBodyGenerateMassProperties(currentBody, vertexMassArrays);

				// Free the collider mass arrays.
				if(vertexMassArrays != NULL){
					physColliderIndex_t i;
					for(i = 0; i < currentBodyColliderNum; ++i){
						if(vertexMassArrays[i] != NULL){
							memFree(vertexMassArrays[i]);
						}
					}
					memFree(vertexMassArrays);
					vertexMassArrays = NULL;
				}

			}

		}else if(currentCommand == 1){

			const cMesh *const cHull = (cMesh *)&currentCollider->c.hull;

			if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

				if(physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum-1]) == -1){
					/** Memory allocation failure. **/
					if(vertexMassArrays != NULL){
						physColliderIndex_t k;
						for(k = 0; k < currentBodyColliderNum; ++k){
							if(vertexMassArrays[k] != NULL){
								memFree(vertexMassArrays[k]);
							}
						}
						memFree(vertexMassArrays);
					}
					modulePhysicsRigidBodyFreeArray(bodies);
					return -1;
				}

			}else{
				printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
				--currentBodyColliderNum;
				physColliderResizeToFit(currentCollider, &vertexMassArrays[currentBodyColliderNum]);
			}
			currentCommand = 0;

		}

	}else{
		printf("Error loading rigid bodies \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	return success;

}

void physRigidBodyDelete(physRigidBody *const restrict body){
	if(body->colliders != NULL){
		modulePhysicsColliderFreeArray(&body->colliders);
	}
	if(body->constraints != NULL){
		modulePhysicsConstraintFreeArray(&body->constraints);
	}
}

/*void physRigidBodyGenerateMassProperties(physRigidBody *body){

	vec3 *v0;
	vec3 *v1;
	float temp;
	float doubleVolume = 0.f;
	size_t i;

	vec3SetS(&body->localCentroid, 0.f);
	body->localInverseInertiaTensor.m[0][0] = 0.f; body->localInverseInertiaTensor.m[0][1] = 0.f; body->localInverseInertiaTensor.m[0][2] = 0.f;
	body->localInverseInertiaTensor.m[1][0] = 0.f; body->localInverseInertiaTensor.m[1][1] = 0.f; body->localInverseInertiaTensor.m[1][2] = 0.f;
	body->localInverseInertiaTensor.m[2][0] = 0.f; body->localInverseInertiaTensor.m[2][1] = 0.f; body->localInverseInertiaTensor.m[2][2] = 0.f;

	if(body->hull.vertexNum > 0){

		const float avgVertexMass = body->mass * body->hull.vertexNum;

		// Recursively calculate the center of mass.
		for(i = 0; i < body->hull.vertexNum; ++i){

			v0 = &body->hull.vertices[i-1];
			v1 = &body->hull.vertices[i];
			temp = v0->x * v1->y - v0->y * v1->x;
			body->localCentroid.x += (v0->x + v1->x) * temp;
			body->localCentroid.y += (v0->y + v1->y) * temp;
			body->localCentroid.z += (v0->z + v1->z) * temp;
			doubleVolume += temp;
		}

		// Final iteration with the last and first vertices.
		v0 = &body->hull.vertices[body->hull.vertexNum-1];
		v1 = &body->hull.vertices[0];
		temp = v0->x * v1->y - v0->y * v1->x;
		body->localCentroid.x += (v0->x + v1->x) * temp;
		body->localCentroid.y += (v0->y + v1->y) * temp;
		body->localCentroid.z += (v0->z + v1->z) * temp;
		doubleVolume += temp;

		// Calculate the mesh's final center of mass.
		temp = 1.f / (3.f * doubleVolume);
		body->localCentroid.x *= temp;
		body->localCentroid.y *= temp;
		body->localCentroid.z *= temp;

		// Calculate the moment of inertia tensor.
		for(i = 0; i < body->hull.vertexNum; ++i){

			const float x = body->hull.vertices[i].x - body->localCentroid.x;  // Is this correct?
			const float y = body->hull.vertices[i].y - body->localCentroid.y;
			const float z = body->hull.vertices[i].z - body->localCentroid.z;
			const float sqrX = x*x;
			const float sqrY = y*y;
			const float sqrZ = z*z;
			// xx
			body->localInertiaTensor.m[0][0] += (sqrY + sqrZ);// * avgVertexMass;
			// yy
			body->localInertiaTensor.m[1][1] += (sqrX + sqrZ);// * avgVertexMass;
			// zz
			body->localInertiaTensor.m[2][2] += (sqrX + sqrY);// * avgVertexMass;
			// xy yx
			body->localInertiaTensor.m[0][1] -= x * y;// * avgVertexMass;
			// xz zx
			body->localInertiaTensor.m[0][2] -= x * z;// * avgVertexMass;
			// yz zy
			body->localInertiaTensor.m[1][2] -= y * z;// * avgVertexMass;
		}

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		body->localInertiaTensor.m[0][0] *= avgVertexMass;
		body->localInertiaTensor.m[1][1] *= avgVertexMass;
		body->localInertiaTensor.m[2][2] *= avgVertexMass;
		body->localInertiaTensor.m[0][1] *= avgVertexMass;
		body->localInertiaTensor.m[0][2] *= avgVertexMass;
		body->localInertiaTensor.m[1][2] *= avgVertexMass;

		// No point calculating the same numbers twice.
		body->localInertiaTensor.m[1][0] = body->localInertiaTensor.m[0][1];
		body->localInertiaTensor.m[2][0] = body->localInertiaTensor.m[0][2];
		body->localInertiaTensor.m[2][1] = body->localInertiaTensor.m[1][2];

		//mat3Invert(&body->localInverseInertiaTensor);

	}

}*/

void physRBIInit(physRBInstance *const restrict prbi){
	prbi->id = (physicsBodyIndex_t)-1;
	prbi->flags = PHYSICS_BODY_INACTIVE;
	prbi->local = NULL;
	prbi->colliders = NULL;
	prbi->configuration = NULL;
	vec3SetS(&prbi->linearVelocity, 0.f);
	vec3SetS(&prbi->angularVelocity, 0.f);
	vec3SetS(&prbi->netForce, 0.f);
	vec3SetS(&prbi->netTorque, 0.f);
	prbi->constraints = NULL;
	prbi->cache = NULL;
}

return_t physRBIInstantiate(physRBInstance *const restrict prbi, physRigidBody *const restrict body, bone *const restrict configuration){

	cVertexIndex_t vertexArraySize;
	cFaceIndex_t normalArraySize;
	cMesh *cHull;
	cMesh *cTemp;

	physCollider *colliderBase;
	physCollider *colliderInstance;

	physRBIInit(prbi);

	// Copy each collider so we can transform it into global space.
	colliderBase = body->colliders;
	while(colliderBase != NULL){

		colliderInstance = modulePhysicsColliderAppend(&prbi->colliders);

		if(colliderInstance != NULL){

			cHull = (cMesh *)&colliderBase->c.hull;
			cTemp = (cMesh *)&colliderInstance->c.hull;

			vertexArraySize = cHull->vertexNum * sizeof(vec3);
			cTemp->vertices = memAllocate(vertexArraySize);
			if(cTemp->vertices == NULL){
				/** Memory allocation failure. **/
				return -1;
			}
			normalArraySize = cHull->faceNum * sizeof(vec3);
			cTemp->normals = memAllocate(normalArraySize);
			if(cTemp->normals == NULL){
				/** Memory allocation failure. **/
				memFree(cTemp->vertices);
				return -1;
			}

			cTemp->vertexNum = cHull->vertexNum;
			cTemp->edgeMax   = cHull->edgeMax;
			cTemp->faceNum   = cHull->faceNum;
			cTemp->edgeNum   = cHull->edgeNum;

			memcpy(cTemp->vertices, cHull->vertices, vertexArraySize);
			memcpy(cTemp->normals,  cHull->normals,  normalArraySize);
			// Re-use the faces and edges arrays. Vertices and
			// normals, however, are modified each update when the
			// collider's configuration changes.
			cTemp->faces = cHull->faces;
			cTemp->edges = cHull->edges;

			colliderInstance->aabb     = colliderBase->aabb;
			colliderInstance->c.type   = colliderBase->c.type;
			colliderInstance->centroid = colliderBase->centroid;

			colliderBase = modulePhysicsColliderNext(colliderBase);

		}else{

			/** Memory allocation failure. **/
			modulePhysicsColliderFreeArray(&prbi->colliders);
			return -1;

		}

	}

	// Copy each constraint.
	///

	prbi->local = body;
	prbi->configuration = configuration;
	prbi->flags = body->flags;

	return 1;

}

return_t physRBIAddConstraint(physRBInstance *const restrict prbi, physConstraint *const c){

	/*
	** Sort a new constraint into the body.
	*/

	///

}

return_t physRBICacheSeparation(physRBInstance *const restrict prbi, physCollisionInfo *const c){

	/*
	** Cache a separation after a failed narrowphase collision check.
	*/

	///

}

return_t physRBIAddCollision(physRBInstance *const restrict prbi, physCollisionInfo *const c){

	/*
	** Caches the collision and creates an
	** inequality constraint representing it.
	*/

	///

}

static void physRBICentroidFromPosition(physRBInstance *const restrict prbi){
	prbi->centroid = prbi->local->centroid;
	quatGetRotatedVec3(&prbi->configuration->orientation, &prbi->centroid);
	vec3AddVToV(&prbi->centroid, &prbi->configuration->position);
}

/*static void physRBIPositionFromCentroid(physRBInstance *prbi){
	prbi->configuration->position.x = -prbi->local->centroid.x;
	prbi->configuration->position.y = -prbi->local->centroid.y;
	prbi->configuration->position.z = -prbi->local->centroid.z;
	quatGetRotatedVec3(&prbi->configuration->orientation, &prbi->configuration->position);
	vec3AddVToV(&prbi->configuration->position, &prbi->centroid);
}*/

static void physRBIGenerateGlobalInertia(physRBInstance *const restrict prbi){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &prbi->configuration->orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &prbi->local->inertiaTensor, &prbi->inverseInertiaTensor);
	mat3MultMByM1(&prbi->inverseInertiaTensor, &inverseOrientationMatrix);

}

void physRBIUpdateCollisionMesh(physRBInstance *const restrict prbi){

	/*
	** Transform the vertices of each body into global space.
	*/

	if(prbi->local != NULL){  /** Remove? **/

		physCollider *i = prbi->colliders;
		const physCollider *j = prbi->local->colliders;
		while(i != NULL){

			// Update the collider.
			physColliderUpdate(i, j, prbi->configuration);

			// Update body minima and maxima.
			if(i == 0){
				// Initialize them to the first collider's bounding box.
				prbi->aabb.left = i->aabb.left;
				prbi->aabb.right = i->aabb.right;
				prbi->aabb.top = i->aabb.top;
				prbi->aabb.bottom = i->aabb.bottom;
				prbi->aabb.front = i->aabb.front;
				prbi->aabb.back = i->aabb.back;
			}else{
				// Update aabb.left and aabb.right.
				if(i->aabb.left <= prbi->aabb.left){
					prbi->aabb.left = i->aabb.left;
				}else if(i->aabb.right > prbi->aabb.right){
					prbi->aabb.right = i->aabb.right;
				}
				// Update aabb.top and aabb.bottom.
				if(i->aabb.top >= prbi->aabb.top){
					prbi->aabb.top = i->aabb.top;
				}else if(i->aabb.bottom < prbi->aabb.bottom){
					prbi->aabb.bottom = i->aabb.bottom;
				}
				// Update aabb.front and aabb.back.
				if(i->aabb.front >= prbi->aabb.front){
					prbi->aabb.front = i->aabb.front;
				}else if(i->aabb.back < prbi->aabb.back){
					prbi->aabb.back = i->aabb.back;
				}
			}

			i = modulePhysicsColliderNext(i);
			j = modulePhysicsColliderNext(j);

		}

	}

}

void physRBIApplyLinearForce(physRBInstance *const restrict prbi, const vec3 *const restrict F){
	/* Apply a linear force. */
	prbi->netForce.x += F->x;
	prbi->netForce.y += F->y;
	prbi->netForce.z += F->z;
}

void physRBIApplyAngularForceGlobal(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r){
	/* Apply an angular force. */
	// T = r x F
	vec3 rsR, rxF;
	vec3SubVFromVR(r, &prbi->centroid, &rsR);
	vec3Cross(&rsR, F, &rxF);
	vec3AddVToV(&prbi->netTorque, &rxF);
}

void physRBIApplyForceGlobal(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in world space.
	*/

	/* Accumulate torque. */
	physRBIApplyAngularForceGlobal(prbi, F, r);

	/* Accumulate force. */
	physRBIApplyLinearForce(prbi, F);

}

/*void physRBIApplyLinearImpulse(physRBInstance *const restrict prbi, const vec3 *const restrict j){
	* Apply a linear impulse. *
	prbi->linearVelocity.x += j->x * prbi->local->inverseMass;
	prbi->linearVelocity.y += j->y * prbi->local->inverseMass;
	prbi->linearVelocity.z += j->z * prbi->local->inverseMass;
}

void physRBIApplyAngularImpulse(physRBInstance *const restrict prbi, const vec3 *const restrict T){
	* Apply an angular impulse. *
	vec3 newTorque = *T;
	mat3MultMByVRow(&prbi->inverseInertiaTensor, &newTorque);
	prbi->angularVelocity.x += newTorque.x;
	prbi->angularVelocity.y += newTorque.y;
	prbi->angularVelocity.z += newTorque.z;
}

void physRBIApplyImpulseAtGlobalPoint(physRBInstance *const restrict prbi, const vec3 *const restrict F, const vec3 *const restrict r){
	vec3 T;
	vec3Cross(r, F, &T);
	physRBIApplyLinearImpulse(prbi, F->x, F->y, F->z);
	physRBIApplyAngularImpulse(prbi, &T);
}*/

static void physRBIResetForceAccumulator(physRBInstance *const restrict prbi){
	prbi->netForce.x = 0.f;
	prbi->netForce.y = 0.f;
	prbi->netForce.z = 0.f;
}

static void physRBIResetTorqueAccumulator(physRBInstance *const restrict prbi){
	prbi->netTorque.x = 0.f;
	prbi->netTorque.y = 0.f;
	prbi->netTorque.z = 0.f;
}

void physRBIBeginSimulation(physRBInstance *const restrict prbi){
	physRBIGenerateGlobalInertia(prbi);
	physRBICentroidFromPosition(prbi);
}

void physRBIIntegrateEuler(physRBInstance *const restrict prbi, const float dt){

	/* Euler integration scheme. */
	if(prbi->local != NULL){  /** Remove? **/

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		if(prbi->local->inverseMass > 0.f){

			const float dtStep = dt/PHYSICS_INTEGRATION_STEPS_EULER;
			vec3 tempVec3;
			float tempFloat;
			quat tempQuat;
			int i;

			for(i = 0; i < PHYSICS_INTEGRATION_STEPS_EULER; ++i){

				/* Calculate linear velocity. */
				// a = F/m
				// dv = a * dt
				prbi->linearVelocity.x += prbi->netForce.x * prbi->local->inverseMass * dtStep;
				prbi->linearVelocity.y += prbi->netForce.y * prbi->local->inverseMass * dtStep;
				prbi->linearVelocity.z += prbi->netForce.z * prbi->local->inverseMass * dtStep;

				/* Update position. */
				prbi->configuration->position.x += prbi->linearVelocity.x * dtStep;
				prbi->configuration->position.y += prbi->linearVelocity.y * dtStep;
				prbi->configuration->position.z += prbi->linearVelocity.z * dtStep;

				/* Calculate angular velocity. */
				tempVec3.x = prbi->netTorque.x * dtStep;
				tempVec3.y = prbi->netTorque.y * dtStep;
				tempVec3.z = prbi->netTorque.z * dtStep;
				mat3MultMByVRow(&prbi->inverseInertiaTensor, &tempVec3);
				prbi->angularVelocity.x += tempVec3.x;
				prbi->angularVelocity.y += tempVec3.y;
				prbi->angularVelocity.z += tempVec3.z;

				/* Update orientation. */
				// Angle
				tempFloat = vec3Magnitude(&prbi->angularVelocity) * dtStep;
				// Axis
				tempVec3 = prbi->angularVelocity;
				vec3NormalizeFast(&tempVec3);
				// Convert axis-angle rotation to a quaternion.
				quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
				quatMultQByQ2(&tempQuat, &prbi->configuration->orientation);
				// Normalize the orientation.
				quatNormalizeFast(&prbi->configuration->orientation);

			}

		}

		/* Update global centroid. */
		physRBICentroidFromPosition(prbi);

		/* Reset force and torque accumulators. */
		physRBIResetForceAccumulator(prbi);
		physRBIResetTorqueAccumulator(prbi);

	}

}

void physRBIIntegrateLeapfrog(physRBInstance *const restrict prbi, const float dt){

	/* Leapfrog integration scheme. */
	if(prbi->local != NULL){  /** Remove? **/

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		if(prbi->local->inverseMass > 0.f){

			vec3 tempVec3;
			float tempFloat;
			quat tempQuat;

			/* Integrate position and linear velocity. */
			tempFloat = prbi->local->inverseMass * dt * 0.5f;
			tempVec3.x = prbi->netForce.x * tempFloat;
			tempVec3.y = prbi->netForce.y * tempFloat;
			tempVec3.z = prbi->netForce.z * tempFloat;
			prbi->linearVelocity.x += tempVec3.x;
			prbi->linearVelocity.y += tempVec3.y;
			prbi->linearVelocity.z += tempVec3.z;
			prbi->configuration->position.x += prbi->linearVelocity.x * dt;
			prbi->configuration->position.y += prbi->linearVelocity.y * dt;
			prbi->configuration->position.z += prbi->linearVelocity.z * dt;
			/**polygonResetForce(polygon);**/
			prbi->linearVelocity.x += tempVec3.x;
			prbi->linearVelocity.y += tempVec3.y;
			prbi->linearVelocity.z += tempVec3.z;

			/* Calculate angular velocity. */
			tempVec3.x = prbi->netTorque.x * dt;
			tempVec3.y = prbi->netTorque.y * dt;
			tempVec3.z = prbi->netTorque.z * dt;
			mat3MultMByVRow(&prbi->inverseInertiaTensor, &tempVec3);
			prbi->angularVelocity.x += tempVec3.x;
			prbi->angularVelocity.y += tempVec3.y;
			prbi->angularVelocity.z += tempVec3.z;

			/* Update orientation. */
			// Axis
			tempVec3 = prbi->angularVelocity;
			vec3NormalizeFast(&tempVec3);
			// Angle
			tempFloat = vec3Magnitude(&prbi->angularVelocity) * dt;
			// Convert axis-angle rotation to a quaternion.
			quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
			quatMultQByQ2(&tempQuat, &prbi->configuration->orientation);
			// Normalize the orientation.
			quatNormalizeFast(&prbi->configuration->orientation);

		}

		/* Update centroid. */
		physRBICentroidFromPosition(prbi);

		/* Reset force and torque accumulators. */
		physRBIResetForceAccumulator(prbi);
		physRBIResetTorqueAccumulator(prbi);

	}

}

void physRBIIntegrateLeapfrogVelocity(physRBInstance *const restrict prbi, const float dt){

	/* Leapfrog integration scheme. */
	if(prbi->local != NULL && prbi->local->inverseMass > 0.f){  /** Remove? **/

		/* Integrate linear velocity half-step. */
		const float tempFloat = prbi->local->inverseMass * dt * 0.5f;
		prbi->linearVelocity.x += prbi->netForce.x * tempFloat;
		prbi->linearVelocity.y += prbi->netForce.y * tempFloat;
		prbi->linearVelocity.z += prbi->netForce.z * tempFloat;

	}

}

void physRBIIntegrateLeapfrogConstraints(physRBInstance *const restrict prbi, const float dt){

	/* Leapfrog integration scheme. */
	if(prbi->local != NULL){  /** Remove? **/

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		if(prbi->local->inverseMass > 0.f){

			vec3 tempVec3;
			float tempFloat;
			quat tempQuat;

			/* Integrate position and final half of linear velocity. */
			prbi->configuration->position.x += prbi->linearVelocity.x * dt;
			prbi->configuration->position.y += prbi->linearVelocity.y * dt;
			prbi->configuration->position.z += prbi->linearVelocity.z * dt;
			/**polygonResetForce(polygon);**/
			tempFloat = prbi->local->inverseMass * dt * 0.5f;
			prbi->linearVelocity.x += prbi->netForce.x * tempFloat;
			prbi->linearVelocity.y += prbi->netForce.y * tempFloat;
			prbi->linearVelocity.z += prbi->netForce.z * tempFloat;

			/* Calculate angular velocity. */
			tempVec3.x = prbi->netTorque.x * dt;
			tempVec3.y = prbi->netTorque.y * dt;
			tempVec3.z = prbi->netTorque.z * dt;
			mat3MultMByVRow(&prbi->inverseInertiaTensor, &tempVec3);
			prbi->angularVelocity.x += tempVec3.x;
			prbi->angularVelocity.y += tempVec3.y;
			prbi->angularVelocity.z += tempVec3.z;

			/* Update orientation. */
			// Axis
			tempVec3 = prbi->angularVelocity;
			vec3NormalizeFast(&tempVec3);
			// Angle
			tempFloat = vec3Magnitude(&prbi->angularVelocity) * dt;
			// Convert axis-angle rotation to a quaternion.
			quatSetAxisAngle(&tempQuat, tempFloat, tempVec3.x, tempVec3.y, tempVec3.z);
			quatMultQByQ2(&tempQuat, &prbi->configuration->orientation);
			// Normalize the orientation.
			quatNormalizeFast(&prbi->configuration->orientation);

		}

		/* Update centroid. */
		physRBICentroidFromPosition(prbi);

		/* Reset force and torque accumulators. */
		physRBIResetForceAccumulator(prbi);
		physRBIResetTorqueAccumulator(prbi);

	}

}

void physRBIIntegrateRungeKutta(physRBInstance *const restrict prbi, const float dt){

	/* RK4 integration scheme. */
	//

}

static __FORCE_INLINE__ void physRBIPenetrationSlop(physRBInstance *const restrict body1, physRBInstance *const restrict body2, const cCollisionContactManifold *const restrict cm){

	// Baumgarte stabilization.

}

static __FORCE_INLINE__ void physRBIResolveCollisionImpulse(physRBInstance *const restrict body1, physRBInstance *const restrict body2, const cCollisionContactManifold *const restrict cm){

	/**vec3 localContactPointA, localContactPointB;
	vec3 contactVelocityA, contactVelocityB;
	vec3 relativeVelocity;
	float normalVelocity;

	// Convert contact point A from global space to local space.
	vec3SubVFromVR(&cd->contacts[0].pointA, &body1->centroid, &localContactPointA);
	// Find the velocity of contact point A.
	// The velocity of a point is V + cross(w, r), where V is the linear velocity,
	// w is the angular velocity and r is the local contact point.
	vec3Cross(&body1->angularVelocity, &localContactPointA, &contactVelocityA);
	vec3AddVToV(&contactVelocityA, &body1->linearVelocity);

	// Convert contact point B from global space to local space.
	vec3SubVFromVR(&cd->contacts[0].pointB, &body2->centroid, &localContactPointB);
	// Find the velocity of contact point B.
	vec3Cross(&body2->angularVelocity, &localContactPointB, &contactVelocityB);
	vec3AddVToV(&contactVelocityB, &body2->linearVelocity);

	// Find the velocity of point A relative to the velocity of point B.
	vec3SubVFromVR(&contactVelocityA, &contactVelocityB, &relativeVelocity);

	// Find how much of relative velocity is in the direction of the contact normal.
	normalVelocity = vec3Dot(&cd->normal, &relativeVelocity);

	// If the velocity in the direction of the contact normal does not pass the
	// threshold, assume a resting collision.
	if(normalVelocity > PHYSICS_RESTING_EPSILON){

		vec3 angularDeltaA, angularDeltaB;
		vec3 angularDeltaLinear, angularDeltaLinearB;

		float impulseMagnitude;
		float impulseMagnitudeOverMass;

		// Calculate contact point A's new torque.
		vec3Cross(&localContactPointA, &cd->normal, &angularDeltaA);
		// Calculate contact point A's new angular velocity.
		mat3MultMByVRow(&body1->inverseInertiaTensor, &angularDeltaA);
		// Calculate contact point A's change in linear velocity due to its rotation.
		vec3Cross(&angularDeltaA, &localContactPointA, &angularDeltaLinear);

		// Calculate contact point B's new torque.
		vec3Cross(&localContactPointB, &cd->normal, &angularDeltaB);
		// Calculate contact point B's new angular velocity.
		mat3MultMByVRow(&body2->inverseInertiaTensor, &angularDeltaB);
		// Calculate contact point B's change in linear velocity due to its rotation.
		vec3Cross(&angularDeltaB, &localContactPointB, &angularDeltaLinearB);
		vec3AddVToV(&angularDeltaLinear, &angularDeltaLinearB);

		// Calculate the impulse magnitude.
		impulseMagnitude = (-1.f - body1->local->coefficientOfRestitution * body2->local->coefficientOfRestitution) *
		                   normalVelocity /
		                   (body1->local->inverseMass + body2->local->inverseMass +
		                   vec3Dot(&angularDeltaLinear, &cd->normal));

		// Calculate body 1's new linear velocity.
		impulseMagnitudeOverMass = impulseMagnitude * body1->local->inverseMass;
		body1->linearVelocity.x += cd->normal.x * impulseMagnitudeOverMass;
		body1->linearVelocity.y += cd->normal.y * impulseMagnitudeOverMass;
		body1->linearVelocity.z += cd->normal.z * impulseMagnitudeOverMass;
		// Calculate body 1's new angular velocity.
		body1->angularVelocity.x += angularDeltaA.x * impulseMagnitude;
		body1->angularVelocity.y += angularDeltaA.y * impulseMagnitude;
		body1->angularVelocity.z += angularDeltaA.z * impulseMagnitude;

		// Calculate body 2's new linear velocity.
		impulseMagnitudeOverMass = impulseMagnitude * body2->local->inverseMass;
		body2->linearVelocity.x -= cd->normal.x * impulseMagnitudeOverMass;
		body2->linearVelocity.y -= cd->normal.y * impulseMagnitudeOverMass;
		body2->linearVelocity.z -= cd->normal.z * impulseMagnitudeOverMass;
		// Calculate body 2's new angular velocity.
		body2->angularVelocity.x -= angularDeltaB.x * impulseMagnitude;
		body2->angularVelocity.y -= angularDeltaB.y * impulseMagnitude;
		body2->angularVelocity.z -= angularDeltaB.z * impulseMagnitude;

	}**/

}

void physRBIResolveCollisionGS(physRBInstance *const restrict body1, physRBInstance *const restrict body2, const cCollisionContactManifold *const restrict cm){

	/*
	** Uses the Gauss-Seidel method to solve the
	** impulse magnitude equation as a system of
	** linear equations with multiple points of
	** contact.
	*/

	//

}

void physRBIUpdate(physRBInstance *const restrict prbi, const float dt){

	//

}

void physRBIDelete(physRBInstance *const restrict prbi){
	if(prbi->colliders != NULL){
		modulePhysicsColliderRBIFreeArray(&prbi->colliders);
	}
	if(prbi->constraints != NULL){
		modulePhysicsConstraintFreeArray(&prbi->constraints);
	}
}
