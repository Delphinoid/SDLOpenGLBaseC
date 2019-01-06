#include "modulePhysics.h"
#include "memoryManager.h"
#include "physicsCollider.h"
#include "helpersFileIO.h"
#include "inline.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PHYSICS_RESOURCE_DIRECTORY_STRING "Resources\\Skeletons\\Physics\\"
#define PHYSICS_RESOURCE_DIRECTORY_LENGTH 28

#define PHYSICS_RESTING_EPSILON 0.0001f

void physRigidBodyLocalInit(physRigidBodyLocal *const restrict local){
	local->id = (physicsBodyIndex_t)-1;
	local->flags = PHYSICS_BODY_ASLEEP;
	cInit(&local->hull, COLLIDER_TYPE_UNKNOWN);
	local->mass = 0.f;
	local->inverseMass = 0.f;
	local->coefficientOfRestitution = 1.f;
	vec3Zero(&local->centroid);
	mat3Identity(&local->inertiaTensor);
	local->constraints = NULL;
}

__FORCE_INLINE__ void physRigidBodyLocalGenerateMassProperties(physRigidBodyLocal *const restrict local, const float **const vertexMassArray){

	/*
	** Calculates the rigid body's total mass, inverse mass,
	** centroid and inertia tensor, as well as the mass
	** properties for each of its colliders.
	*/

	physColliderGenerateMass(local, vertexMassArray);
	physColliderGenerateMoment(local, vertexMassArray);

}

static return_t physColliderResizeToFit(collider *const restrict local){

	cMesh *const cHull = (cMesh *)&local->data;

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

	return 1;

}

/** FIX CONSTRAINT LOADING. MOSTLY TEMPORARY. **/
return_t physRigidBodyLocalLoad(physRigidBodyLocal **const restrict bodies, const skeleton *const restrict skl, const char *const restrict prgPath, const char *const restrict filePath){

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
		float *vertexMassArray = NULL;  // Array of vertex masses for each collider.

		int isSkeleton = -1;         // Whether or not a skeleton is being described.
		int currentCommand = -1;     // The current multiline command type (-1 = none, 0 = rigid body, 1 = collider, 2 = constraint).
		fileLine_t currentLine = 0;  // Current file line being read.

		physRigidBodyLocal *currentBody = NULL;
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
							physRigidBodyLocalGenerateMassProperties(currentBody, &vertexMassArray);
							// Free the collider mass arrays.
							if(vertexMassArray != NULL){
								memFree(vertexMassArray);
								vertexMassArray = NULL;
							}
							if(!isSkeleton){
								break;
							}
						}else if(currentCommand == 1){
							if(physColliderResizeToFit(&currentBody->hull) < 0){
								/** Memory allocation failure. **/
								if(vertexMassArray != NULL){
									memFree(vertexMassArray);
								}
								modulePhysicsRigidBodyLocalFreeArray(bodies);
								fclose(rbInfo);
								return -1;
							}
						}
					}
					// Initialize a new rigid body.
					currentBody = modulePhysicsRigidBodyLocalAppend(bodies);
					if(currentBody == NULL){
						/** Memory allocation failure. **/
						if(vertexMassArray != NULL){
							memFree(vertexMassArray);
						}
						modulePhysicsRigidBodyLocalFreeArray(bodies);
						fclose(rbInfo);
						return -1;
					}
					physRigidBodyLocalInit(currentBody);
					currentBody->id = currentBodyID;
					currentBody->flags = PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE | PHYSICS_BODY_COLLIDE;
					currentCommand = 0;

				}


			// Collider
			}else if(lineLength >= 10 && strncmp(line, "collider ", 9) == 0){
				if(strchr(line+9, '{')){
					if(currentCommand >= 0){

						// Close any current commands.
						if(currentCommand > 0){
							printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
							       "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
							if(currentCommand == 1){
								if(physColliderResizeToFit(&currentBody->hull) < 0){
									/** Memory allocation failure. **/
									if(vertexMassArray != NULL){
										memFree(vertexMassArray);
									}
									modulePhysicsRigidBodyLocalFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						vertexCapacity = 0;
						normalCapacity = 0;
						edgeCapacity = 0;

						cInit(&currentBody->hull, COLLIDER_TYPE_MESH);
						cMeshInit((cMesh *)&currentBody->hull.data);

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

				cMesh *const cHull = (cMesh *)&currentBody->hull.data;
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
						if(vertexMassArray != NULL){
							memFree(vertexMassArray);
						}
						modulePhysicsRigidBodyLocalFreeArray(bodies);
						fclose(rbInfo);
						return -1;
					}
					tempBuffer2 = memReallocate(vertexMassArray, vertexCapacity*sizeof(float));
					if(tempBuffer2 == NULL){
						/** Memory allocation failure. **/
						if(vertexMassArray != NULL){
							memFree(vertexMassArray);
						}
						modulePhysicsRigidBodyLocalFreeArray(bodies);
						fclose(rbInfo);
						memFree(tempBuffer1);
						return -1;
					}
					cHull->vertices = tempBuffer1;
					vertexMassArray = tempBuffer2;
				}

				token = strtok(line+2, " ");
				cHull->vertices[cHull->vertexNum].x = strtod(token, NULL);
				token = strtok(NULL, " ");
				cHull->vertices[cHull->vertexNum].y = strtod(token, NULL);
				token = strtok(NULL, " ");
				cHull->vertices[cHull->vertexNum].z = strtod(token, NULL);
				token = strtok(NULL, " ");
				if(token != NULL){
					vertexMassArray[cHull->vertexNum] = strtod(token, NULL);
				}
				++cHull->vertexNum;


			// Collider face
			}else if(lineLength >= 7 && strncmp(line, "f ", 2) == 0){

				const char *token = strtok(line+2, " ");
				if(token != NULL){

					cMesh *const cHull = (cMesh *)&currentBody->hull.data;

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
								if(vertexMassArray != NULL){
									memFree(vertexMassArray);
								}
								modulePhysicsRigidBodyLocalFreeArray(bodies);
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
							if(vertexMassArray != NULL){
								memFree(vertexMassArray);
							}
							modulePhysicsRigidBodyLocalFreeArray(bodies);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer2 = memReallocate(cHull->faces, normalCapacity*sizeof(cMeshFace));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArray != NULL){
								memFree(vertexMassArray);
							}
							modulePhysicsRigidBodyLocalFreeArray(bodies);
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
								if(physColliderResizeToFit(&currentBody->hull) < 0){
									/** Memory allocation failure. **/
									if(vertexMassArray != NULL){
										memFree(vertexMassArray);
									}
									modulePhysicsRigidBodyLocalFreeArray(bodies);
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
									if(vertexMassArray != NULL){
										memFree(vertexMassArray);
									}
									modulePhysicsRigidBodyLocalFreeArray(bodies);
									fclose(rbInfo);
									return -1;
								}
								physConstraintInit(currentConstraint);
								#warning FIX THIS
								///currentConstraint->id = constrainedBodyID;
								///currentConstraint->ownerID = currentBody->id;
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
					/**const char *token = strtok(line+13, "/");
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
					currentConstraint->constraintOffsetMax.z = strtod(token, NULL);**/
				}else{
					printf("Error loading rigid bodies \"%s\": Constraint sub-command \"offsetBounds\" invoked on line %u "
					       "without specifying a constraint.\n", fullPath, currentLine);
				}


			// Constraint rotation bounds
			}else if(lineLength >= 26 && strncmp(line, "rotationBounds ", 15) == 0){
				if(currentCommand == 2){
					/**const char *token = strtok(line+15, "/");
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
					currentConstraint->constraintRotationMax.z = strtod(token, NULL);**/
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
						flagsUnset(currentConstraint->flags, PHYSICS_CONSTRAINT_NO_COLLISION);
					}else{
						flagsSet(currentConstraint->flags, PHYSICS_CONSTRAINT_NO_COLLISION);
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
						flagsSet(currentBody->flags, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE);
					}else{
						flagsUnset(currentBody->flags, PHYSICS_BODY_INITIALIZE | PHYSICS_BODY_SIMULATE);
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

					// Generate various mass properties for the rigid body.
					physRigidBodyLocalGenerateMassProperties(currentBody, &vertexMassArray);

					// Free the collider mass arrays.
					if(vertexMassArray != NULL){
						memFree(vertexMassArray);
						vertexMassArray = NULL;
					}

					success = 1;

					if(!isSkeleton){
						break;
					}

					currentCommand = -1;

				}else if(currentCommand == 1){

					const cMesh *const cHull = (cMesh *)&currentBody->hull.data;

					if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

						if(physColliderResizeToFit(&currentBody->hull) < 0){
							/** Memory allocation failure. **/
							if(vertexMassArray != NULL){
								memFree(vertexMassArray);
							}
							modulePhysicsRigidBodyLocalFreeArray(bodies);
							return -1;
						}

					}else{
						printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
						physColliderResizeToFit(&currentBody->hull);
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

			// Generate various mass properties for the rigid body.
			physRigidBodyLocalGenerateMassProperties(currentBody, &vertexMassArray);

			// Free the collider mass arrays.
			if(vertexMassArray != NULL){
				memFree(vertexMassArray);
				vertexMassArray = NULL;
			}

		}else if(currentCommand == 1){

			const cMesh *const cHull = (cMesh *)&currentBody->hull.data;

			if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

				if(physColliderResizeToFit(&currentBody->hull) < 0){
					/** Memory allocation failure. **/
					if(vertexMassArray != NULL){
						memFree(vertexMassArray);
					}
					modulePhysicsRigidBodyLocalFreeArray(bodies);
					return -1;
				}

			}else{
				printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
				physColliderResizeToFit(&currentBody->hull);
			}
			currentCommand = 0;

		}

	}else{
		printf("Error loading rigid bodies \"%s\": Could not open file.\n", fullPath);
		return 0;
	}

	return success;

}

void physRigidBodyLocalDelete(physRigidBodyLocal *const restrict local){
	cDelete(&local->hull);
	if(local->constraints != NULL){
		modulePhysicsConstraintFreeArray(&local->constraints);
	}
}

/*void physRigidBodyGenerateMassProperties(physRigidBody *body){

	vec3 *v0;
	vec3 *v1;
	float temp;
	float doubleVolume = 0.f;
	size_t i;

	vec3Zero(&body->localCentroid);
	mat3Zero(&body->localInverseInertiaTensor);

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

void physRigidBodyInit(physRigidBody *const restrict body){
	body->id = (physicsBodyIndex_t)-1;
	body->flags = PHYSICS_BODY_ASLEEP;
	body->local = NULL;
	cInit(&body->hull, COLLIDER_TYPE_UNKNOWN);
	body->configuration = NULL;
	vec3Zero(&body->linearVelocity);
	vec3Zero(&body->angularVelocity);
	vec3Zero(&body->netForce);
	vec3Zero(&body->netTorque);
	body->constraints = NULL;
	body->cache = NULL;
}

return_t physRigidBodyInstantiate(physRigidBody *const restrict body, physRigidBodyLocal *const restrict local, bone *const restrict configuration){

	physRigidBodyInit(body);

	// Copy the collider so we can transform it into global space.
	cInstantiate(&body->hull, &local->hull);

	// Copy each constraint.
	///

	body->local = local;
	body->configuration = configuration;
	body->flags = local->flags;

	return 1;

}

return_t physRigidBodyAddConstraint(physRigidBody *const restrict body, physConstraint *const c){

	/*
	** Sort a new constraint into the body.
	*/

	///

}

physSeparation *physRigidBodyFindSeparation(physRigidBody *const restrict body, const physicsBodyIndex_t id, physSeparation **const previous){

	/*
	** Find a separation from a previous failed narrowphase collision check.
	**
	** Separations are cached in increasing order of id, so once we find an
	** id greater than the supplied id we can perform an early exit.
	**
	** We also need to return the separation directly before it in the SLink
	** so we can perform an insertion or removal later on if we need to.
	*/

	physSeparation *i = body->cache;
	physSeparation *p = NULL;

	while(i != NULL && id >= i->id){
		if(id == i->id){
			*previous = p;
			return i;
		}
		p = i;
		i = (physSeparation *)memSLinkNext(i);
	}

	*previous = p;
	return NULL;

}

physSeparation *physRigidBodyCacheSeparation(physRigidBody *const restrict body, physSeparation *const restrict previous){

	/*
	** Cache a separation after a failed narrowphase collision check.
	*/

	return modulePhysicsSeparationInsertAfter(&body->cache, previous);

}

void physRigidBodyRemoveSeparation(physRigidBody *const restrict body, physSeparation *const restrict separation, const physSeparation *const restrict previous){

	/*
	** Cache a separation after a failed narrowphase collision check.
	*/

	modulePhysicsSeparationFree(&body->cache, separation, previous);

}

static __FORCE_INLINE__ void physRigidBodyCentroidFromPosition(physRigidBody *const restrict body){
	body->centroid = body->local->centroid;
	quatGetRotatedVec3(&body->configuration->orientation, &body->centroid);
	vec3AddVToV(&body->centroid, &body->configuration->position);
}

static __FORCE_INLINE__ void physRigidBodyPositionFromCentroid(physRigidBody *const restrict body){
	body->configuration->position.x = -body->local->centroid.x;
	body->configuration->position.y = -body->local->centroid.y;
	body->configuration->position.z = -body->local->centroid.z;
	quatGetRotatedVec3(&body->configuration->orientation, &body->configuration->position);
	vec3AddVToV(&body->configuration->position, &body->centroid);
}

static __FORCE_INLINE__ void physRigidBodyGenerateGlobalInertia(physRigidBody *const restrict body){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &body->configuration->orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &body->local->inertiaTensor, &body->inverseInertiaTensor);
	mat3MultMByM1(&body->inverseInertiaTensor, &inverseOrientationMatrix);

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
	vec3SubVFromVR(r, &body->centroid, &rsR);
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

/*void physRigidBodyApplyLinearImpulse(physRigidBody *const restrict body, const vec3 *const restrict j){
	* Apply a linear impulse. *
	body->linearVelocity.x += j->x * body->local->inverseMass;
	body->linearVelocity.y += j->y * body->local->inverseMass;
	body->linearVelocity.z += j->z * body->local->inverseMass;
}

void physRigidBodyApplyAngularImpulse(physRigidBody *const restrict body, const vec3 *const restrict T){
	* Apply an angular impulse. *
	vec3 newTorque = *T;
	mat3MultMByVRow(&body->inverseInertiaTensor, &newTorque);
	body->angularVelocity.x += newTorque.x;
	body->angularVelocity.y += newTorque.y;
	body->angularVelocity.z += newTorque.z;
}

void physRigidBodyApplyImpulseAtGlobalPoint(physRigidBody *const restrict body, const vec3 *const restrict F, const vec3 *const restrict r){
	vec3 T;
	vec3CrossR(r, F, &T);
	physRigidBodyApplyLinearImpulse(body, F->x, F->y, F->z);
	physRigidBodyApplyAngularImpulse(body, &T);
}*/

static __FORCE_INLINE__ void physRigidBodyResetForceAccumulator(physRigidBody *const restrict body){
	vec3Zero(&body->netForce);
}

static __FORCE_INLINE__ void physRigidBodyResetTorqueAccumulator(physRigidBody *const restrict body){
	vec3Zero(&body->netTorque);
}

void physRigidBodyIntegrateVelocity(physRigidBody *const restrict body, const float dt){

	/*
	** Integrate one full timestep.
	*/

	// Update moment of inertia.
	physRigidBodyGenerateGlobalInertia(body);

	if(body->local->inverseMass > 0.f){

		const float modifier = body->local->inverseMass * dt;

		// Integrate linear velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)){
			body->linearVelocity.x += body->netForce.x * modifier;
			body->linearVelocity.y += body->netForce.y * modifier;
			body->linearVelocity.z += body->netForce.z * modifier;
		}

		// Integrate angular velocity.
		if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)){
			vec3 momentum;
			mat3MultMByVRowR(&body->inverseInertiaTensor, &body->netTorque, &momentum);
			body->angularVelocity.x += momentum.x * dt;
			body->angularVelocity.y += momentum.y * dt;
			body->angularVelocity.z += momentum.z * dt;
		}

	}

	// Reset force and torque accumulators.
	physRigidBodyResetForceAccumulator(body);
	physRigidBodyResetTorqueAccumulator(body);

}

void physRigidBodyIntegrateConfiguration(physRigidBody *const restrict body, const float dt){

	// Integrate position.
	if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_LINEAR)){
		body->centroid.x += body->linearVelocity.x * dt;
		body->centroid.y += body->linearVelocity.y * dt;
		body->centroid.z += body->linearVelocity.z * dt;
	}

	// Integrate orientation.
	if(flagsAreSet(body->flags, PHYSICS_BODY_SIMULATE_ANGULAR)){
		quatIntegrate(&body->configuration->orientation, &body->angularVelocity, dt);
	}

	// Update the position from the centroid.
	physRigidBodyPositionFromCentroid(body);

}

__FORCE_INLINE__ void physRigidBodyUpdateCollisionMesh(physRigidBody *const restrict body){

	/*
	** Transform the vertices of each body into global space.
	** Calculates the body's center of mass from its configuration.
	*/

	physRigidBodyCentroidFromPosition(body);
	cTransform(&body->hull, &body->centroid, &body->local->hull, &body->local->centroid, &body->aabb, body->configuration);

}

static __FORCE_INLINE__ void physRigidBodyResolveCollisionImpulse(physRigidBody *const restrict body1, physRigidBody *const restrict body2, const cContact *const restrict cm){

	/**vec3 localContactPointA, localContactPointB;
	vec3 contactVelocityA, contactVelocityB;
	vec3 relativeVelocity;
	float normalVelocity;

	// Convert contact point A from global space to local space.
	vec3SubVFromVR(&cd->contacts[0].pointA, &body1->centroid, &localContactPointA);
	// Find the velocity of contact point A.
	// The velocity of a point is V + cross(w, r), where V is the linear velocity,
	// w is the angular velocity and r is the local contact point.
	vec3CrossR(&body1->angularVelocity, &localContactPointA, &contactVelocityA);
	vec3AddVToV(&contactVelocityA, &body1->linearVelocity);

	// Convert contact point B from global space to local space.
	vec3SubVFromVR(&cd->contacts[0].pointB, &body2->centroid, &localContactPointB);
	// Find the velocity of contact point B.
	vec3CrossR(&body2->angularVelocity, &localContactPointB, &contactVelocityB);
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
		vec3CrossR(&localContactPointA, &cd->normal, &angularDeltaA);
		// Calculate contact point A's new angular velocity.
		mat3MultMByVRow(&body1->inverseInertiaTensor, &angularDeltaA);
		// Calculate contact point A's change in linear velocity due to its rotation.
		vec3CrossR(&angularDeltaA, &localContactPointA, &angularDeltaLinear);

		// Calculate contact point B's new torque.
		vec3CrossR(&localContactPointB, &cd->normal, &angularDeltaB);
		// Calculate contact point B's new angular velocity.
		mat3MultMByVRow(&body2->inverseInertiaTensor, &angularDeltaB);
		// Calculate contact point B's change in linear velocity due to its rotation.
		vec3CrossR(&angularDeltaB, &localContactPointB, &angularDeltaLinearB);
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

void physRigidBodyResolveCollisionGS(physRigidBody *const restrict body1, physRigidBody *const restrict body2, const cContact *const restrict cm){

	/*
	** Uses the Gauss-Seidel method to solve the
	** impulse magnitude equation as a system of
	** linear equations with multiple points of
	** contact.
	*/

	//

}

void physRigidBodyUpdate(physRigidBody *const restrict body, const float dt){

	//

}

void physRigidBodyDelete(physRigidBody *const restrict body){
	physColliderDelete(&body->hull);
	if(body->constraints != NULL){
		modulePhysicsConstraintFreeArray(&body->constraints);
	}
	if(body->cache != NULL){
		modulePhysicsSeparationFreeArray(&body->cache);
	}
}
