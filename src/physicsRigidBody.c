#include "modulePhysics.h"
#include "memoryManager.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PHYSICS_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Skeletons"FILE_PATH_DELIMITER_STRING"Physics"FILE_PATH_DELIMITER_STRING
#define PHYSICS_RESOURCE_DIRECTORY_LENGTH 28

/** Use Parallel Axis Theorem for loading colliders. **/

void physRigidBodyBaseInit(physRigidBodyBase *const restrict local){
	local->id = (physicsBodyIndex_t)-1;
	local->flags = PHYSICS_BODY_ASLEEP;
	local->hull = NULL;
	local->mass = 0.f;
	local->inverseMass = 0.f;
	local->linearDamping = 0.f;
	local->angularDamping = 0.f;
	vec3ZeroP(&local->centroid);
	mat3Identity(&local->inverseInertiaTensor);
}

__FORCE_INLINE__ static void physRigidBodyBaseAddCollider(physRigidBodyBase *const restrict local, physCollider *const c, const float **const vertexMassArray){

	/*
	** Adds a single collider to the body.
	*/

	float mass;
	float inverseMass;
	vec3 centroid;

	// Generate the new collider's mass properties.
	physColliderGenerateMass(&c->c, &mass, &inverseMass, &centroid, vertexMassArray);

	if(mass != 0.f){

		mat3 inverseInertiaTensor;

		const vec3 difference = vec3VSubV(local->centroid, centroid);
		const vec3 differenceWeighted = vec3VMultS(difference, mass);
		const vec3 differenceWeightedSquared = vec3VMultV(difference, differenceWeighted);

		// Generate and add the new collider's inverse moment
		// of inertia using the Parallel Axis Theorem.
		physColliderGenerateMoment(&c->c, &inverseInertiaTensor, &centroid, vertexMassArray);
		inverseInertiaTensor = mat3Invert(inverseInertiaTensor);

		// Translate the inertia tensor using the rigid body's centroid.
		inverseInertiaTensor.m[0][0] += differenceWeightedSquared.y + differenceWeightedSquared.z;
		inverseInertiaTensor.m[0][1] -= differenceWeighted.x * difference.y;
		inverseInertiaTensor.m[0][2] -= differenceWeighted.x * difference.z;
		inverseInertiaTensor.m[1][1] += differenceWeightedSquared.x + differenceWeightedSquared.z;
		inverseInertiaTensor.m[1][2] -= differenceWeighted.y * difference.z;
		inverseInertiaTensor.m[2][2] += differenceWeightedSquared.x + differenceWeightedSquared.y;

		// Add the collider's contribution to the body's inertia tensor.
		local->inverseInertiaTensor.m[0][0] += inverseInertiaTensor.m[0][0];
		local->inverseInertiaTensor.m[0][1] += inverseInertiaTensor.m[0][1];
		local->inverseInertiaTensor.m[1][0] += inverseInertiaTensor.m[0][1];
		local->inverseInertiaTensor.m[0][2] += inverseInertiaTensor.m[0][2];
		local->inverseInertiaTensor.m[2][0] += inverseInertiaTensor.m[0][2];
		local->inverseInertiaTensor.m[1][1] += inverseInertiaTensor.m[1][1];
		local->inverseInertiaTensor.m[1][2] += inverseInertiaTensor.m[1][2];
		local->inverseInertiaTensor.m[2][1] += inverseInertiaTensor.m[1][2];
		local->inverseInertiaTensor.m[2][2] += inverseInertiaTensor.m[2][2];

		// Calculate the new, weighted centroid.
		local->inverseMass = 1.f / (local->mass + mass);
		local->centroid = vec3VMultS(vec3VAddV(vec3VMultS(local->centroid, local->mass), vec3VMultS(centroid, mass)), local->inverseMass);
		local->mass += mass;

	}

}

__FORCE_INLINE__ static void physRigidBodyBaseGenerateProperties(physRigidBodyBase *const restrict local, const float **const vertexMassArray){

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

		local->inverseInertiaTensor = mat3Invert(local->inverseInertiaTensor);

	}else{
		local->inverseMass = 0.f;
		mat3ZeroP(&local->inverseInertiaTensor);
	}

	local->mass = tempMass;
	local->centroid = tempCentroid;

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
	boneInit(&body->configuration);
	vec3ZeroP(&body->linearVelocity);
	vec3ZeroP(&body->angularVelocity);
	vec3ZeroP(&body->netForce);
	vec3ZeroP(&body->netTorque);
}

return_t physRigidBodyInstantiate(physRigidBody *const restrict body, physRigidBodyBase *const restrict local){

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
	body->centroidLocal = local->centroid;
	body->inverseInertiaTensorLocal = local->inverseInertiaTensor;

	body->base = local;
	body->flags = local->flags;

	return 1;

}

__FORCE_INLINE__ void physRigidBodySetUninitialized(physRigidBody *const restrict body){
	flagsSet(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetInitialized(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_UNINITIALIZED);
	flagsSet(body->flags, PHYSICS_BODY_INITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetInitializedFull(physRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSICS_BODY_UNINITIALIZED);
}
__FORCE_INLINE__ void physRigidBodySetAsleep(physRigidBody *const restrict body){
	body->flags &= PHYSICS_BODY_ASLEEP;
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

__FORCE_INLINE__ return_t physRigidBodyWasInitialized(const physRigidBody *const restrict body){
	return flagsAreSet(body->flags, PHYSICS_BODY_INITIALIZED);
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

__HINT_INLINE__ void physRigidBodyApplyLinearForce(physRigidBody *const restrict body, const vec3 F){
	/*
	** Apply a linear force.
	*/
	body->netForce = vec3VAddV(body->netForce, F);
}

__HINT_INLINE__ void physRigidBodyApplyAngularForce(physRigidBody *const restrict body, const vec3 F, const vec3 r){
	/*
	** Apply an angular force in global space.
	*/
	// T = r x F
	body->netTorque = vec3VAddV(body->netTorque, vec3Cross(vec3VSubV(r, body->centroidGlobal), F));
}

__HINT_INLINE__ void physRigidBodyApplyForce(physRigidBody *const restrict body, const vec3 F, const vec3 r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in global space.
	*/

	// Accumulate torque.
	physRigidBodyApplyAngularForce(body, F, r);

	// Accumulate force.
	physRigidBodyApplyLinearForce(body, F);

}

__HINT_INLINE__ void physRigidBodyApplyVelocityImpulse(physRigidBody *const restrict body, const vec3 x, const vec3 J){

	/*
	** Applies an impulse J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VAddV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VAddV(body->angularVelocity, mat3MMultVBra(body->inverseInertiaTensorGlobal, vec3Cross(x, J)));

	//}

}
__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseInverse(physRigidBody *const restrict body, const vec3 x, const vec3 J){

	/*
	** Applies an impulse -J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VSubV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VSubV(body->angularVelocity, mat3MMultVBra(body->inverseInertiaTensorGlobal, vec3Cross(x, J)));

	//}

}

__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseAngular(physRigidBody *const restrict body, const vec3 x, const vec3 J, const vec3 a){

	/*
	** Applies an impulse J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VAddV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VAddV(body->angularVelocity, mat3MMultVBra(body->inverseInertiaTensorGlobal, vec3VAddV(vec3Cross(x, J), a)));

	//}

}
__HINT_INLINE__ void physRigidBodyApplyVelocityImpulseAngularInverse(physRigidBody *const restrict body, const vec3 x, const vec3 J, const vec3 a){

	/*
	** Applies an impulse -J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		body->linearVelocity = vec3VSubV(body->linearVelocity, vec3VMultS(J, body->inverseMass));
		// Angular impulse.
		body->angularVelocity = vec3VSubV(body->angularVelocity, mat3MMultVBra(body->inverseInertiaTensorGlobal, vec3VAddV(vec3Cross(x, J), a)));

	//}

}

__HINT_INLINE__ void physRigidBodyApplyConfigurationImpulse(physRigidBody *const restrict body, const vec3 x, const vec3 J){

	/*
	** Applies an impulse J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)/* &&
			(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)*/
		){
			body->centroidGlobal = vec3VAddV(body->centroidGlobal, vec3VMultS(J, body->inverseMass));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}
		// Angular impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)/* &&
			(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)*/
		){
			body->configuration.orientation = quatNormalizeFastAccurate(
				quatQAddQ(
					body->configuration.orientation,
					quatDifferentiate(
						body->configuration.orientation,
						mat3MMultVBra(
							body->inverseInertiaTensorGlobal,
							vec3Cross(x, J)
						)
					)
				)
			);
			physRigidBodyGenerateGlobalInertia(body);
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}

	//}

}
__HINT_INLINE__ void physRigidBodyApplyConfigurationImpulseInverse(physRigidBody *const restrict body, const vec3 x, const vec3 J){

	/*
	** Applies an impulse -J at point x in global space.
	*/

	//if(body->inverseMass > 0.f){

		// Linear impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)/* &&
			(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)*/
		){
			body->centroidGlobal = vec3VSubV(body->centroidGlobal, vec3VMultS(J, body->inverseMass));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}
		// Angular impulse.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)/* &&
			(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)*/
		){
			body->configuration.orientation = quatNormalizeFastAccurate(
				quatQSubQ(
					body->configuration.orientation,
					quatDifferentiate(
						body->configuration.orientation,
						mat3MMultVBra(
							body->inverseInertiaTensorGlobal,
							vec3Cross(x, J)
						)
					)
				)
			);
			physRigidBodyGenerateGlobalInertia(body);
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}

	//}

}

__FORCE_INLINE__ void physRigidBodyCentroidFromPosition(physRigidBody *const restrict body){
	body->centroidGlobal = vec3VAddV(
		vec3VMultV(
			quatRotateVec3(
				body->configuration.orientation,
				body->centroidLocal
			),
			body->configuration.scale
		),
		body->configuration.position
	);
}

__FORCE_INLINE__ void physRigidBodyPositionFromCentroid(physRigidBody *const restrict body){
	body->configuration.position = vec3VAddV(
		vec3VMultV(
			quatRotateVec3(
				body->configuration.orientation,
				vec3Negate(body->centroidLocal)
			),
			body->configuration.scale
		),
		body->centroidGlobal
	);
}

#ifdef PHYSICS_SCALE_INERTIA_TENSORS
__FORCE_INLINE__ static mat3 physRigidBodyScaleInertia(mat3 I, const vec3 scale){

	if(scale.x != 1.f || scale.y != 1.f || scale.z != 1.f){

		if(mat3InvertR(I, &I)){

			const float xy = scale.x * scale.y;
			const float xz = scale.x * scale.z;
			const float yz = scale.y * scale.z;

			float sqrZ = (I.m[0][0] + I.m[1][1] - I.m[2][2]) * 0.5f;
			const float sqrY = (I.m[0][0] - sqrZ) * scale.y * scale.y;
			const float sqrX = (I.m[1][1] - sqrZ) * scale.x * scale.x;
			sqrZ *= scale.z * scale.z;

			/*
			** I[0][0] = J[0] = y^2 + z^2
			** I[1][1] = J[1] = x^2 + z^2
			** I[2][2] = J[2] = x^2 + y^2
			**
			** Therefore:
			** x^2 = (J[1] - J[0] + J[2])/2
			** y^2 = (J[0] - J[1] + J[2])/2
			** z^2 = (J[0] - J[2] + J[1])/2
			**
			** To scale, multiply by the scale coefficient squared.
			*/
			I.m[0][0] = sqrY + sqrZ;
			I.m[1][1] = sqrX + sqrZ;
			I.m[2][2] = sqrX + sqrY;

			/*
			** I[0][1] = I[1][0] = J[3] -= x * y;
			** I[0][2] = I[2][0] = J[4] -= x * z;
			** I[1][2] = I[2][1] = J[5] -= y * z;
			**
			** To scale, just multiply each element by the
			** product of the two scale coefficients.
			*/
			I.m[0][1] *= xy;
			I.m[0][2] *= xz;
			I.m[1][2] *= yz;
			I.m[1][0] *= xy;
			I.m[2][0] *= xz;
			I.m[2][1] *= yz;

			I = mat3Invert(I);

		}

	}

	return I;

}
#endif

__FORCE_INLINE__ void physRigidBodyGenerateGlobalInertia(physRigidBody *const restrict body){

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	const mat3 orientationMatrix = mat3Quaternion(body->configuration.orientation);
	const mat3 inverseOrientationMatrix = mat3Transpose(orientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	body->inverseInertiaTensorGlobal = mat3MMultM(
		mat3MMultM(
			orientationMatrix,
			#ifdef PHYSICS_SCALE_INERTIA_TENSORS
			physRigidBodyScaleInertia(body->inverseInertiaTensorLocal, body->configuration.scale)
			#else
			body->inverseInertiaTensorLocal
			#endif
		),
		inverseOrientationMatrix
	);

}

__FORCE_INLINE__ void physRigidBodyUpdateConfiguration(physRigidBody *const restrict body){
	if(flagsAreSet(body->flags, PHYSICS_BODY_TRANSFORMED)){
		physRigidBodyPositionFromCentroid(body);
	}
}

__FORCE_INLINE__ void physRigidBodyResetAccumulators(physRigidBody *const restrict body){
	// Reset force and torque accumulators.
	vec3ZeroP(&body->netForce);
	vec3ZeroP(&body->netTorque);
}

void physRigidBodyIntegrateVelocity(physRigidBody *const restrict body, const float dt){

	//if(body->inverseMass > 0.f){

		const float modifier = body->inverseMass * dt;

		// Integrate linear velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)){
			// Apply damping.
			body->linearVelocity = vec3VMultS(vec3VAddV(body->linearVelocity, vec3VMultS(body->netForce, modifier)),  1.f / (1.f + dt * body->linearDamping));
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
						mat3MMultVBra(
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

	/*}else{

		mat3ZeroP(&body->inverseInertiaTensorGlobal);

	}*/

}

void physRigidBodyIntegrateConfiguration(physRigidBody *const restrict body, const float dt){

	//if(body->inverseMass > 0.f){

		// Integrate position.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)/* &&
			(body->linearVelocity.y != 0.f || body->linearVelocity.x != 0.f || body->linearVelocity.z != 0.f)*/
		){
			body->centroidGlobal = vec3VAddV(body->centroidGlobal, vec3VMultS(body->linearVelocity, dt));
			flagsSet(body->flags, PHYSICS_BODY_TRANSLATED);
		}else{
			flagsUnset(body->flags, PHYSICS_BODY_TRANSLATED);
		}

		// Integrate orientation.
		if(
			flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)/* &&
			(body->angularVelocity.y != 0.f || body->angularVelocity.z != 0.f || body->angularVelocity.x != 0.f)*/
		){
			body->configuration.orientation = quatNormalizeFastAccurate(quatIntegrate(body->configuration.orientation, body->angularVelocity, dt));
			flagsSet(body->flags, PHYSICS_BODY_ROTATED);
		}else{
			flagsUnset(body->flags, PHYSICS_BODY_ROTATED);
		}

	//}

}

__FORCE_INLINE__ void physRigidBodyIntegrateSymplecticEuler(physRigidBody *const restrict body, const float dt){

	// Integrate the body's velocities.
	physRigidBodyIntegrateVelocity(body, dt);
	physRigidBodyResetAccumulators(body);

	// Integrate the body's configuration.
	physRigidBodyIntegrateConfiguration(body, dt);

}

__FORCE_INLINE__ void physRigidBodyIntegrateLeapfrog(physRigidBody *const restrict body, const float dt){

	// "Kick".
	physRigidBodyIntegrateVelocity(body, dt*0.5f);

	// "Drift".
	physRigidBodyIntegrateConfiguration(body, dt);

	// "Kick".
	physRigidBodyIntegrateVelocity(body, dt*0.5f);
	physRigidBodyResetAccumulators(body);

}

__FORCE_INLINE__ void physRigidBodyIntegrateLeapfrogTest(physRigidBody *const restrict body, const float dt){

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

	/*
	** Check if two rigid bodies may collide.
	*/

	// Check if they share any joints that forbid collision.
	// "Ownership" of the joint is delegated to the body with
	// the greater address; while the joint will appear in
	// both arrays, it will be closer to the front in the
	// owner's array.

	if(body1 > body2){

		// Body 1's joints.
		const physJoint *i = body1->joints;
		while(i != NULL && body2 >= (physRigidBody *)i->bodyB){
			// Check if the child body is the same.
			if(body2 == (physRigidBody *)i->bodyB){
				if(flagsAreUnset(i->flags, PHYSICS_JOINT_COLLIDE)){
					return 0;
				}
			}
			i = (physJoint *)memQLinkNextA(i);
		}

		return 1;

	}else if(body2 > body1){

		// Body 2's joints.
		const physJoint *i = body2->joints;
		while(i != NULL && body1 >= (physRigidBody *)i->bodyB){
			// Check if the child body is the same.
			if(body1 == (physRigidBody *)i->bodyB){
				if(flagsAreUnset(i->flags, PHYSICS_JOINT_COLLIDE)){
					return 0;
				}
			}
			i = (physJoint *)memQLinkNextA(i);
		}

		return 1;

	}

	return 0;

}

void physRigidBodyAddCollider(physRigidBody *const restrict body, physCollider *const c, const float **const vertexMassArray){

	/*
	** Adds a single collider to the body.
	*/

	float mass;
	float inverseMass;
	vec3 centroid;

	// Generate the new collider's mass properties.
	physColliderGenerateMass(&c->c, &mass, &inverseMass, &centroid, vertexMassArray);

	if(mass != 0.f){

		mat3 inverseInertiaTensor;

		const vec3 difference = vec3VSubV(body->centroidLocal, centroid);
		const vec3 differenceWeighted = vec3VMultS(difference, mass);
		const vec3 differenceWeightedSquared = vec3VMultV(difference, differenceWeighted);

		// Generate and add the new collider's inverse moment
		// of inertia using the Parallel Axis Theorem.
		physColliderGenerateMoment(&c->c, &inverseInertiaTensor, &centroid, vertexMassArray);
		inverseInertiaTensor = mat3Invert(inverseInertiaTensor);

		// Translate the inertia tensor using the rigid body's centroid.
		inverseInertiaTensor.m[0][0] += differenceWeightedSquared.y + differenceWeightedSquared.z;
		inverseInertiaTensor.m[0][1] -= differenceWeighted.x * difference.y;
		inverseInertiaTensor.m[0][2] -= differenceWeighted.x * difference.z;
		inverseInertiaTensor.m[1][1] += differenceWeightedSquared.x + differenceWeightedSquared.z;
		inverseInertiaTensor.m[1][2] -= differenceWeighted.y * difference.z;
		inverseInertiaTensor.m[2][2] += differenceWeightedSquared.x + differenceWeightedSquared.y;

		// Add the collider's contribution to the body's inertia tensor.
		body->inverseInertiaTensorLocal.m[0][0] += inverseInertiaTensor.m[0][0];
		body->inverseInertiaTensorLocal.m[0][1] += inverseInertiaTensor.m[0][1];
		body->inverseInertiaTensorLocal.m[1][0] += inverseInertiaTensor.m[0][1];
		body->inverseInertiaTensorLocal.m[0][2] += inverseInertiaTensor.m[0][2];
		body->inverseInertiaTensorLocal.m[2][0] += inverseInertiaTensor.m[0][2];
		body->inverseInertiaTensorLocal.m[1][1] += inverseInertiaTensor.m[1][1];
		body->inverseInertiaTensorLocal.m[1][2] += inverseInertiaTensor.m[1][2];
		body->inverseInertiaTensorLocal.m[2][1] += inverseInertiaTensor.m[1][2];
		body->inverseInertiaTensorLocal.m[2][2] += inverseInertiaTensor.m[2][2];

		// Calculate the new, weighted centroid.
		body->inverseMass = 1.f / (body->mass + mass);
		body->centroidLocal = vec3VMultS(vec3VAddV(vec3VMultS(body->centroidLocal, body->mass), vec3VMultS(centroid, mass)), body->inverseMass);
		body->mass += mass;

	}

}

return_t physRigidBodyAddJoint(physRigidBody *const restrict body, physJoint *const joint){

	/*
	** Sort a new joint into the body.
	*/

	///

}

void physRigidBodyDelete(physRigidBody *const restrict body){
	modulePhysicsColliderFreeArray(&body->hull);
}
