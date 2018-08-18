#include "physicsRigidBody.h"
#include "helpersMisc.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define PHYS_INTEGRATION_STEPS_EULER      2
#define PHYS_INTEGRATION_STEPS_RUNGEKUTTA 4

#define PHYS_RESTING_EPSILON 0.0001f
#define PHYS_BAUMGARTE_TERM  0.0001f

void physConstraintInit(physConstraint *constraint){
	constraint->flags = PHYS_CONSTRAINT_TYPE_1;
	constraint->constraintID = (size_t)-1;
	vec3SetS(&constraint->constraintOffsetMin, 0.f);
	vec3SetS(&constraint->constraintOffsetMax, 0.f);
	vec3SetS(&constraint->constraintRotationMin, 0.f);
	vec3SetS(&constraint->constraintRotationMax, 0.f);
}

void physRigidBodyInit(physRigidBody *body){
	body->colliderNum = 0;
	body->colliders = NULL;
	body->mass = 0.f;
	body->inverseMass = 0.f;
	body->coefficientOfRestitution = 1.f;
	vec3SetS(&body->centroid, 0.f);
	mat3Identity(&body->inertiaTensor);
}

void physRigidBodyGenerateMassProperties(physRigidBody *body, float **vertexMassArrays){

	/*
	** Calculates the rigid body's total mass, inverse mass,
	** centroid and inertia tensor, as well as the mass
	** properties for each of its colliders.
	*/

	colliderIndex_t i;
	float *colliderMassArray;
	float tempInertiaTensor[6];

	body->mass = 0.f;
	vec3SetS(&body->centroid, 0.f);
	body->inertiaTensor.m[0][0] = 0.f; body->inertiaTensor.m[0][1] = 0.f; body->inertiaTensor.m[0][2] = 0.f;
	body->inertiaTensor.m[1][0] = 0.f; body->inertiaTensor.m[1][1] = 0.f; body->inertiaTensor.m[1][2] = 0.f;
	body->inertiaTensor.m[2][0] = 0.f; body->inertiaTensor.m[2][1] = 0.f; body->inertiaTensor.m[2][2] = 0.f;

	// Generate the mass properites of each collider, as
	// well as the total, weighted centroid of the body.
	for(i = 0; i < body->colliderNum; ++i){

		float colliderMass;
		if(vertexMassArrays != NULL){
			colliderMassArray = vertexMassArrays[i];
		}else{
			colliderMassArray = NULL;
		}
		colliderMass = physColliderGenerateMass(&body->colliders[i], colliderMassArray);

		body->centroid.x += body->colliders[i].centroid.x * colliderMass;
		body->centroid.y += body->colliders[i].centroid.y * colliderMass;
		body->centroid.z += body->colliders[i].centroid.z * colliderMass;
		body->mass += colliderMass;

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
	for(i = 0; i < body->colliderNum; ++i){

		if(vertexMassArrays[i] != NULL){
			colliderMassArray = vertexMassArrays[i];
		}else{
			colliderMassArray = NULL;
		}
		physColliderGenerateMoment(&body->colliders[i], &body->centroid, colliderMassArray, tempInertiaTensor);

		// Since every vertex has the same mass, we can simplify our calculations by moving the multiplications out of the loop.
		body->inertiaTensor.m[0][0] += tempInertiaTensor[0];// * avgVertexMass;
		body->inertiaTensor.m[1][1] += tempInertiaTensor[1];// * avgVertexMass;
		body->inertiaTensor.m[2][2] += tempInertiaTensor[2];// * avgVertexMass;
		body->inertiaTensor.m[0][1] += tempInertiaTensor[3];// * avgVertexMass;
		body->inertiaTensor.m[0][2] += tempInertiaTensor[4];// * avgVertexMass;
		body->inertiaTensor.m[1][2] += tempInertiaTensor[5];// * avgVertexMass;

	}

	// No point calculating the same numbers twice.
	body->inertiaTensor.m[1][0] = body->inertiaTensor.m[0][1];
	body->inertiaTensor.m[2][0] = body->inertiaTensor.m[0][2];
	body->inertiaTensor.m[2][1] = body->inertiaTensor.m[1][2];

	//mat3Invert(&body->localInverseInertiaTensor);

}

static signed char physColliderResizeToFit(physCollider *collider, float **vertexMassArrays){

	hbMesh *cHull = (hbMesh *)&collider->hb.hull;

	if(cHull->vertexNum != 0){
		float *tempBuffer2;
		vec3 *tempBuffer1 = realloc(cHull->vertices, cHull->vertexNum*sizeof(vec3));
		if(tempBuffer1 == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tempBuffer2 = realloc(*vertexMassArrays, cHull->vertexNum*sizeof(float));
		if(tempBuffer2 == NULL){
			/** Memory allocation failure. **/
			free(tempBuffer1);
			return -1;
		}
		cHull->vertices = tempBuffer1;
		*vertexMassArrays = tempBuffer2;
	}else{
		if(cHull->vertices != NULL){
			free(cHull->vertices);
			cHull->vertices = NULL;
		}
		if(*vertexMassArrays != NULL){
			free(*vertexMassArrays);
			*vertexMassArrays = NULL;
		}
	}

	if(cHull->faceNum != 0){
		hbMeshFace *tempBuffer2;
		vec3 *tempBuffer1 = realloc(cHull->normals, cHull->faceNum*sizeof(vec3));
		if(tempBuffer1 == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		tempBuffer2 = realloc(cHull->faces, cHull->faceNum*sizeof(hbMeshFace));
		if(tempBuffer2 == NULL){
			/** Memory allocation failure. **/
			free(tempBuffer1);
			return -1;
		}
		cHull->normals = tempBuffer1;
		cHull->faces = tempBuffer2;
	}else{
		if(cHull->normals != NULL){
			free(cHull->normals);
			cHull->normals = NULL;
		}
		if(cHull->faces != NULL){
			free(cHull->faces);
			cHull->faces = NULL;
		}
	}

	if(cHull->edgeNum != 0){
		hbMeshEdge *tempBuffer = realloc(cHull->edges, cHull->edgeNum*sizeof(hbMeshEdge));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
		cHull->edges = tempBuffer;
	}else if(cHull->edges != NULL){
		free(cHull->edges);
		cHull->edges = NULL;
	}

	return 1;

}

signed char physRigidBodyLoad(physRigidBody *bodies, flags_t *flags, constraintIndex_t *constraintNum, physConstraint **constraints,
                              const skeleton *skl, const char *prgPath, const char *filePath){

	/*
	** Loads a series of rigid bodies.
	**
	** If skeleton is not NULL, it constrains them using
	** the specified bone names.
	*/

	boneIndex_t j;
	boneIndex_t bodyNum = skl == NULL ? 1 : skl->boneNum;
	signed char success = 0;

	FILE *rbInfo;
	const size_t pathLen = strlen(prgPath);
	const size_t fileLen = strlen(filePath);
	char *fullPath = malloc((pathLen+fileLen+1) * sizeof(char));
	if(fullPath == NULL){
		/** Memory allocation failure. **/
		return -1;
	}
	memcpy(fullPath, prgPath, pathLen);
	memcpy(fullPath+pathLen, filePath, fileLen);
	fullPath[pathLen+fileLen] = '\0';
	rbInfo = fopen(fullPath, "r");

	for(j = 0; j < bodyNum; ++j){
		physRigidBodyInit(&bodies[j]);
	}

	if(rbInfo != NULL){

		char lineFeed[1024];
		char *line;
		size_t lineLength;

		hbVertexIndex_t vertexCapacity = 0;
		hbFaceIndex_t normalCapacity = 0;
		hbEdgeIndex_t edgeCapacity = 0;
		float **vertexMassArrays = NULL;  // Array of vertex masses for each collider.

		signed char isSkeleton = -1;  // Whether or not a skeleton is being described.
		boneIndex_t currentBone = 0;
		signed char currentCommand = -1;  // The current multiline command type (-1 = none, 0 = rigid body, 1 = collider, 2 = constraint).
		unsigned int currentLine = 0;     // Current file line being read.

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

					boneIndex_t nextBone = 0;

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
							for(j = 0; j < skl->boneNum; ++j){
								if(strncmp(line, skl->bones[j].name, lineLength) == 0){
									nextBone = j;
									isSkeleton = 1;
									break;
								}
							}

						}else{
							printf("Error loading rigid bodies \"%s\": Rigid body at line %u appears to be attached to a bone when "
							       "a previous body was not. Ignoring.\n", fullPath, currentLine);
						}

					}else{
						isSkeleton = 0;
					}

					if(isSkeleton >= 0){
						// Close any current commands.
						if(currentCommand != -1){
							printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
								   "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
							if(currentCommand == 0){
								// Generate various mass properties for the rigid body.
								physRigidBodyGenerateMassProperties(&bodies[currentBone], vertexMassArrays);
								// Free the collider mass arrays.
								if(vertexMassArrays != NULL){
									colliderIndex_t i;
									for(i = 0; i < bodies[currentBone].colliderNum; ++i){
										if(vertexMassArrays[i] != NULL){
											free(vertexMassArrays[i]);
										}
									}
									free(vertexMassArrays);
									vertexMassArrays = NULL;
								}
								if(!isSkeleton){
									break;
								}
							}else if(currentCommand == 1){
								if(physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1],
									                       &vertexMassArrays[bodies[currentBone].colliderNum-1]) == -1){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										colliderIndex_t k;
										for(k = 0; k < bodies[currentBone].colliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												free(vertexMassArrays[k]);
											}
										}
										free(vertexMassArrays);
									}
									for(j = 0; j < bodyNum; ++j){
										physRigidBodyDelete(&bodies[currentBone]);
										if(constraints[j] != NULL){
											free(constraints[j]);
										}
									}
									free(fullPath);
									fclose(rbInfo);
									return -1;
								}
							}
						}
						currentBone = nextBone;
						flags[currentBone] = PHYS_BODY_INITIALIZE | PHYS_BODY_COLLIDE;
						physRigidBodyInit(&bodies[currentBone]);
						constraintNum[currentBone] = 0;
						constraints[currentBone] = NULL;
						currentCommand = 0;
					}

				}


			// Collider
			}else if(lineLength >= 10 && strncmp(line, "collider ", 9) == 0){
				if(strchr(line+9, '{')){
					if(currentCommand >= 0){

						physCollider *tempBuffer1;
						float **tempBuffer2;

						// Close any current commands.
						if(currentCommand > 0){
							printf("Error loading rigid bodies \"%s\": Trying to start a multiline command at line %u "
							       "while another is already in progress. Closing the current command.\n", fullPath, currentLine);
							if(currentCommand == 1){
								if(physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1],
									                       &vertexMassArrays[bodies[currentBone].colliderNum-1]) == -1){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										colliderIndex_t k;
										for(k = 0; k < bodies[currentBone].colliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												free(vertexMassArrays[k]);
											}
										}
										free(vertexMassArrays);
									}
									for(j = 0; j < bodyNum; ++j){
										physRigidBodyDelete(&bodies[currentBone]);
										if(constraints[j] != NULL){
											free(constraints[j]);
										}
									}
									free(fullPath);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						// Allocate memory for the new collider.
						tempBuffer1 = realloc(bodies[currentBone].colliders, (bodies[currentBone].colliderNum+1)*sizeof(physCollider));
						if(tempBuffer1 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								colliderIndex_t k;
								for(k = 0; k < bodies[currentBone].colliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										free(vertexMassArrays[k]);
									}
								}
								free(vertexMassArrays);
							}
							for(j = 0; j < bodyNum; ++j){
								physRigidBodyDelete(&bodies[currentBone]);
								if(constraints[j] != NULL){
									free(constraints[j]);
								}
							}
							free(fullPath);
							fclose(rbInfo);
							return -1;
						}
						bodies[currentBone].colliders = tempBuffer1;
						tempBuffer2 = realloc(vertexMassArrays, (bodies[currentBone].colliderNum+1)*sizeof(float *));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								colliderIndex_t k;
								for(k = 0; k < bodies[currentBone].colliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										free(vertexMassArrays[k]);
									}
								}
								free(vertexMassArrays);
							}
							for(j = 0; j < bodyNum; ++j){
								physRigidBodyDelete(&bodies[currentBone]);
								if(constraints[j] != NULL){
									free(constraints[j]);
								}
							}
							free(fullPath);
							fclose(rbInfo);
							return -1;
						}
						vertexMassArrays = tempBuffer2;
						vertexMassArrays[bodies[currentBone].colliderNum] = NULL;
						vertexCapacity = 0;
						normalCapacity = 0;
						edgeCapacity = 0;
						hbInit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum].hb, HB_TYPE_MESH);
						hbMeshInit((hbMesh *)&bodies[currentBone].colliders[bodies[currentBone].colliderNum].hb.hull);
						++bodies[currentBone].colliderNum;
						currentCommand = 1;

					}else{
						printf("Error loading texture wrapper \"%s\": Rigid body sub-command \"constraint\" invoked on line %u "
						       "without specifying a rigid body.\n", fullPath, currentLine);
					}

				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"collider\" at line %u "
					       "does not contain a brace.\n", fullPath, currentLine);
				}


			// Collider vertex
			}else if(lineLength >= 7 && strncmp(line, "v ", 2) == 0){

				if(bodies[currentBone].colliderNum > 0){

					hbMesh *cHull = (hbMesh *)&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1].hb.hull;
					char *token;

					// Reallocate vertex array if necessary.
					if(cHull->vertexNum == vertexCapacity){
						if(vertexCapacity == 0){
							vertexCapacity = 1;
						}else{
							vertexCapacity *= 2;
						}
						float *tempBuffer2;
						vec3 *tempBuffer1 = realloc(cHull->vertices, vertexCapacity*sizeof(vec3));
						if(tempBuffer1 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								colliderIndex_t k;
								for(k = 0; k < bodies[currentBone].colliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										free(vertexMassArrays[k]);
									}
								}
								free(vertexMassArrays);
							}
							for(j = 0; j < bodyNum; ++j){
								physRigidBodyDelete(&bodies[currentBone]);
								if(constraints[j] != NULL){
									free(constraints[j]);
								}
							}
							free(fullPath);
							fclose(rbInfo);
							return -1;
						}
						tempBuffer2 = realloc(vertexMassArrays[bodies[currentBone].colliderNum-1], vertexCapacity*sizeof(float));
						if(tempBuffer2 == NULL){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								colliderIndex_t k;
								for(k = 0; k < bodies[currentBone].colliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										free(vertexMassArrays[k]);
									}
								}
								free(vertexMassArrays);
							}
							for(j = 0; j < bodyNum; ++j){
								physRigidBodyDelete(&bodies[currentBone]);
								if(constraints[j] != NULL){
									free(constraints[j]);
								}
							}
							free(fullPath);
							fclose(rbInfo);
							free(tempBuffer1);
							return -1;
						}
						cHull->vertices = tempBuffer1;
						vertexMassArrays[bodies[currentBone].colliderNum-1] = tempBuffer2;
					}

					token = strtok(line+2, " ");
					cHull->vertices[cHull->vertexNum].x = strtod(token, NULL);
					token = strtok(NULL, " ");
					cHull->vertices[cHull->vertexNum].y = strtod(token, NULL);
					token = strtok(NULL, " ");
					cHull->vertices[cHull->vertexNum].z = strtod(token, NULL);
					token = strtok(NULL, " ");
					if(token != NULL){
						vertexMassArrays[bodies[currentBone].colliderNum-1][cHull->vertexNum] = strtod(token, NULL);
					}
					++cHull->vertexNum;

				}


			// Collider face
			}else if(lineLength >= 7 && strncmp(line, "f ", 2) == 0){

				if(bodies[currentBone].colliderNum > 0){

					char *token = strtok(line+2, " ");
					if(token != NULL){

						hbMesh *cHull = (hbMesh *)&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1].hb.hull;

						hbEdgeIndex_t i = 0;

						hbEdgeIndex_t start;
						hbEdgeIndex_t end = strtoul(token, NULL, 0)-1;

						hbEdgeIndex_t oldNum = cHull->edgeNum;
						hbEdgeIndex_t addNum = 0;

						hbEdgeIndex_t first = (hbEdgeIndex_t)-1;
						hbEdgeIndex_t last = oldNum;

						byte_t firstTwin = 0;
						byte_t lastTwin = 0;

						byte_t exit = 0;

						vec3 BsA;
						vec3 CsA;

						// Recursively add the face's edges.
						while(1){

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
								hbMeshEdge *tempBuffer = realloc(cHull->edges, edgeCapacity*sizeof(hbMeshEdge));
								if(tempBuffer == NULL){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										colliderIndex_t k;
										for(k = 0; k < bodies[currentBone].colliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												free(vertexMassArrays[k]);
											}
										}
										free(vertexMassArrays);
									}
									for(j = 0; j < bodyNum; ++j){
										physRigidBodyDelete(&bodies[currentBone]);
										if(constraints[j] != NULL){
											free(constraints[j]);
										}
									}
									free(fullPath);
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
									if(first == (hbEdgeIndex_t)-1){
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
								if(first == (hbEdgeIndex_t)-1){
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
							if(normalCapacity == 0){
								normalCapacity = 1;
							}else{
								normalCapacity *= 2;
							}
							vec3 *tempBuffer1 = realloc(cHull->normals, normalCapacity*sizeof(vec3));
							if(tempBuffer1 == NULL){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									colliderIndex_t k;
									for(k = 0; k < bodies[currentBone].colliderNum; ++k){
										if(vertexMassArrays[k] != NULL){
											free(vertexMassArrays[k]);
										}
									}
									free(vertexMassArrays);
								}
								for(j = 0; j < bodyNum; ++j){
									physRigidBodyDelete(&bodies[currentBone]);
									if(constraints[j] != NULL){
										free(constraints[j]);
									}
								}
								free(fullPath);
								fclose(rbInfo);
								return -1;
							}
							hbMeshFace *tempBuffer2 = realloc(cHull->faces, normalCapacity*sizeof(hbMeshFace));
							if(tempBuffer2 == NULL){
								/** Memory allocation failure. **/
								if(vertexMassArrays != NULL){
									colliderIndex_t k;
									for(k = 0; k < bodies[currentBone].colliderNum; ++k){
										if(vertexMassArrays[k] != NULL){
											free(vertexMassArrays[k]);
										}
									}
									free(vertexMassArrays);
								}
								for(j = 0; j < bodyNum; ++j){
									physRigidBodyDelete(&bodies[currentBone]);
									if(constraints[j] != NULL){
										free(constraints[j]);
									}
								}
								free(fullPath);
								fclose(rbInfo);
								free(tempBuffer1);
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

						cHull->faces[cHull->faceNum].edgeNum = addNum;
						cHull->faces[cHull->faceNum].edge = first;

						++cHull->faceNum;

					}else{
						printf("Error loading rigid bodies \"%s\": Collider face at line %u "
						       "must have at least three vertices.\n", fullPath, currentLine);
					}

				}


			// Constraint
			}else if(lineLength >= 12 && strncmp(line, "constraint ", 11) == 0){

				char *end;
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
								if(physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1],
									                       &vertexMassArrays[bodies[currentBone].colliderNum-1]) == -1){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										colliderIndex_t k;
										for(k = 0; k < bodies[currentBone].colliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												free(vertexMassArrays[k]);
											}
										}
										free(vertexMassArrays);
									}
									for(j = 0; j < bodyNum; ++j){
										physRigidBodyDelete(&bodies[currentBone]);
										if(constraints[j] != NULL){
											free(constraints[j]);
										}
									}
									free(fullPath);
									fclose(rbInfo);
									return -1;
								}
							}
						}

						// Check if the rigid body has a name.
						if(end != &line[10] && skl != NULL && isSkeleton){

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
							for(j = 0; j < skl->boneNum; ++j){
								if(strncmp(line, skl->bones[j].name, lineLength) == 0){
									break;
								}
							}
							// Add the new constraint if possible.
							if(j < skl->boneNum){
								constraints[currentBone] = realloc(constraints[currentBone], (constraintNum[currentBone]+1)*sizeof(physConstraint *));
								if(constraints[currentBone] == NULL){
									/** Memory allocation failure. **/
									if(vertexMassArrays != NULL){
										colliderIndex_t k;
										for(k = 0; k < bodies[currentBone].colliderNum; ++k){
											if(vertexMassArrays[k] != NULL){
												free(vertexMassArrays[k]);
											}
										}
										free(vertexMassArrays);
									}
									for(j = 0; j < bodyNum; ++j){
										physRigidBodyDelete(&bodies[currentBone]);
										if(constraints[j] != NULL){
											free(constraints[j]);
										}
									}
									free(fullPath);
									fclose(rbInfo);
									return -1;
								}
								physConstraintInit(&constraints[currentBone][constraintNum[currentBone]]);
								constraints[currentBone][constraintNum[currentBone]].constraintID = j;
								++constraintNum[currentBone];
								currentCommand = 2;
								success = 1;
							}

						}else{
							printf("Error loading rigid bodies \"%s\": The \"constraint\" command at like %u may only be used by. "
							       "physics skeletons. Ignoring.\n", fullPath, currentLine);
						}

					}else{
						printf("Error loading texture wrapper \"%s\": Rigid body sub-command \"constraint\" invoked on line %u "
						       "without specifying a rigid body.\n", fullPath, currentLine);
					}
				}else{
					printf("Error loading rigid bodies \"%s\": Rigid body sub-command \"constraint\" at line %u "
					       "does not contain a brace.\n", fullPath, currentLine);
				}


			// Constraint offset bounds
			}else if(lineLength >= 24 && strncmp(line, "offsetBounds ", 13) == 0){
				char *token = strtok(line+13, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMin.x = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMin.y = strtod(token, NULL);
				token = strtok(NULL, " ");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMin.z = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMax.x = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMax.y = strtod(token, NULL);
				token = strtok(NULL, " ");
				constraints[currentBone][constraintNum[currentBone]-1].constraintOffsetMax.z = strtod(token, NULL);


			// Constraint rotation bounds
			}else if(lineLength >= 26 && strncmp(line, "rotationBounds ", 15) == 0){
				char *token = strtok(line+15, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMin.x = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMin.y = strtod(token, NULL);
				token = strtok(NULL, " ");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMin.z = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMax.x = strtod(token, NULL);
				token = strtok(NULL, "/");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMax.y = strtod(token, NULL);
				token = strtok(NULL, " ");
				constraints[currentBone][constraintNum[currentBone]-1].constraintRotationMax.z = strtod(token, NULL);


			// Coefficient of restitution
			}else if(lineLength >= 13 && strncmp(line, "energyratio ", 12) == 0){
				if(currentCommand == 0){
					bodies[currentBone].coefficientOfRestitution = strtof(line+12, NULL);
					if(bodies[currentBone].coefficientOfRestitution > 1.f){
						bodies[currentBone].coefficientOfRestitution = 1.f;
					}else if(bodies[currentBone].coefficientOfRestitution < 0.f){
						bodies[currentBone].coefficientOfRestitution = 0.f;
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
						flags[currentBone] |= PHYS_BODY_COLLIDE;
					}else{
						flags[currentBone] &= ~PHYS_BODY_COLLIDE;
					}
				}else if(currentCommand == 1){
					if(strtoul(line+10, NULL, 0)){
						constraints[currentBone][constraintNum[currentBone]-1].flags |= PHYS_CONSTRAINT_COLLIDE;
					}else{
						constraints[currentBone][constraintNum[currentBone]-1].flags &= ~PHYS_CONSTRAINT_COLLIDE;
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
						flags[currentBone] |= PHYS_BODY_INITIALIZE;
					}else{
						flags[currentBone] &= ~PHYS_BODY_INITIALIZE;
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

					if(bodies[currentBone].colliderNum > 0 || constraintNum[currentBone] > 0){

						// Generate various mass properties for the rigid body.
						physRigidBodyGenerateMassProperties(&bodies[currentBone], vertexMassArrays);

						// Free the collider mass arrays.
						if(vertexMassArrays != NULL){
							colliderIndex_t i;
							for(i = 0; i < bodies[currentBone].colliderNum; ++i){
								if(vertexMassArrays[i] != NULL){
									free(vertexMassArrays[i]);
								}
							}
							free(vertexMassArrays);
							vertexMassArrays = NULL;
						}

						success = 1;

						if(!isSkeleton){
							break;
						}

					}

					currentCommand = -1;

				}else if(currentCommand == 1){

					hbMesh *cHull = (hbMesh *)&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1].hb.hull;

					if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

						if(physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1],
						                           &vertexMassArrays[bodies[currentBone].colliderNum-1]) == -1){
							/** Memory allocation failure. **/
							if(vertexMassArrays != NULL){
								colliderIndex_t k;
								for(k = 0; k < bodies[currentBone].colliderNum; ++k){
									if(vertexMassArrays[k] != NULL){
										free(vertexMassArrays[k]);
									}
								}
								free(vertexMassArrays);
							}
							for(j = 0; j < bodyNum; ++j){
								physRigidBodyDelete(&bodies[currentBone]);
								if(constraints[j] != NULL){
									free(constraints[j]);
								}
							}
							return -1;
						}

					}else{
						printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
						--bodies[currentBone].colliderNum;
						physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum],
						                        &vertexMassArrays[bodies[currentBone].colliderNum]);
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
		free(fullPath);

		if(currentCommand == 0){

			if(bodies[currentBone].colliderNum > 0 || constraintNum[currentBone] > 0){

				// Generate various mass properties for the rigid body.
				physRigidBodyGenerateMassProperties(&bodies[currentBone], vertexMassArrays);

				// Free the collider mass arrays.
				if(vertexMassArrays != NULL){
					colliderIndex_t i;
					for(i = 0; i < bodies[currentBone].colliderNum; ++i){
						if(vertexMassArrays[i] != NULL){
							free(vertexMassArrays[i]);
						}
					}
					free(vertexMassArrays);
					vertexMassArrays = NULL;
				}

			}

		}else if(currentCommand == 1){

			hbMesh *cHull = (hbMesh *)&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1].hb.hull;

			if(cHull->vertexNum > 0 && cHull->faceNum > 0 && cHull->edgeNum > 0){

				if(physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum-1],
				                           &vertexMassArrays[bodies[currentBone].colliderNum-1]) == -1){
					/** Memory allocation failure. **/
					if(vertexMassArrays != NULL){
						colliderIndex_t k;
						for(k = 0; k < bodies[currentBone].colliderNum; ++k){
							if(vertexMassArrays[k] != NULL){
								free(vertexMassArrays[k]);
							}
						}
						free(vertexMassArrays);
					}
					for(j = 0; j < bodyNum; ++j){
						physRigidBodyDelete(&bodies[currentBone]);
						if(constraints[j] != NULL){
							free(constraints[j]);
						}
					}
					return -1;
				}

			}else{
				printf("Error loading rigid bodies \"%s\": Collider has no vertices or faces.\n", fullPath);
				--bodies[currentBone].colliderNum;
				physColliderResizeToFit(&bodies[currentBone].colliders[bodies[currentBone].colliderNum],
				                        &vertexMassArrays[bodies[currentBone].colliderNum]);
			}
			currentCommand = 0;

		}

	}else{
		printf("Error loading rigid bodies \"%s\": Could not open file.\n", fullPath);
		free(fullPath);
		return 0;
	}

	return success;

}

void physRigidBodyDelete(physRigidBody *body){
	if(body->colliders != NULL){
		colliderIndex_t i;
		for(i = 0; i < body->colliderNum; ++i){
			physColliderDelete(&body->colliders[i]);
		}
		free(body->colliders);
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

void physRBIInit(physRBInstance *prbi){
	prbi->id = (physicsBodyIndex_t)-1;
	prbi->flags = PHYS_BODY_SIMULATE | PHYS_BODY_COLLIDE;
	prbi->local = NULL;
	prbi->colliders = NULL;
	prbi->configuration = NULL;
	//prbi->configuration[1] = NULL;
	vec3SetS(&prbi->linearVelocity, 0.f);
	vec3SetS(&prbi->angularVelocity, 0.f);
	vec3SetS(&prbi->netForce, 0.f);
	vec3SetS(&prbi->netTorque, 0.f);
	prbi->constraintNum = 0;
	prbi->constraintCapacity = 0;
	prbi->constraints = NULL;
	prbi->separationNum = 0;
	prbi->separationCapacity = 0;
	prbi->cache = NULL;
}

signed char physRBIInstantiate(physRBInstance *prbi, physRigidBody *body, bone *configuration){

	hbMesh *cHull;
	hbMesh *cTemp;

	colliderIndex_t i;

	// Allocate memory for each collider.
	physCollider *tempBuffer = malloc(body->colliderNum * sizeof(physCollider));
	if(tempBuffer == NULL){
		/** Memory allocation failure. **/
		return -1;
	}

	// Copy each collider so we can transform it into global space.
	for(i = 0; i < body->colliderNum; ++i){

		hbVertexIndex_t vertexArraySize;
		hbFaceIndex_t normalArraySize;

		cHull = (hbMesh *)&body->colliders[i].hb.hull;
		cTemp = (hbMesh *)&tempBuffer[i].hb.hull;

		vertexArraySize = cHull->vertexNum * sizeof(vec3);
		cTemp->vertices = malloc(vertexArraySize);
		if(cTemp->vertices == NULL){
			/** Memory allocation failure. **/
			break;
		}
		normalArraySize = cHull->faceNum * sizeof(vec3);
		cTemp->normals = malloc(normalArraySize);
		if(cTemp->normals == NULL){
			/** Memory allocation failure. **/
			free(cTemp->vertices);
			break;
		}

		cTemp->vertexNum = cHull->vertexNum;
		cTemp->faceNum = cHull->faceNum;
		cTemp->edgeNum = cHull->edgeNum;

		memcpy(cTemp->vertices, cHull->vertices, vertexArraySize);
		memcpy(cTemp->normals, cHull->normals, normalArraySize);
		// Re-use the faces and edges arrays. Vertices and
		// normals, however, are modified each update when the
		// collider's configuration changes.
		cTemp->faces = cHull->faces;
		cTemp->edges = cHull->edges;

		tempBuffer[i].aabb = body->colliders[i].aabb;
		tempBuffer[i].hb.type = body->colliders[i].hb.type;
		tempBuffer[i].centroid = body->colliders[i].centroid;

	}

	// Make sure every collider copied successfully.
	if(i < body->colliderNum){
		/** Memory allocation failure. **/
		while(i > 0){
			--i;
			cTemp = (hbMesh *)&tempBuffer[i].hb.hull;
			free(cTemp->vertices);
			free(cTemp->normals);
		}
		free(tempBuffer);
		return -1;
	}

	physRBIInit(prbi);
	prbi->local = body;
	prbi->colliders = tempBuffer;
	prbi->configuration = configuration;
	return 1;

}

signed char physRBIStateCopy(physRBInstance *o, physRBInstance *c){
	/** Finish this. **/
	return 1;
}

signed char physRBIAddConstraint(physRBInstance *prbi, physConstraint *c){

	/*
	** Sort a new constraint into the body.
	*/

	constraintIndex_t i;
	if(prbi->constraintNum >= prbi->constraintCapacity){

		// Allocate room for the new constraint.
		const constraintIndex_t tempCapacity = prbi->constraintNum+1;
		physConstraint *tempBuffer = malloc(tempCapacity*sizeof(physConstraint));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}

		// Sort the constraints back into the array.
		for(i = 0; i < prbi->constraintNum; ++i){
			if(c->constraintID < prbi->constraints[i].constraintID){
				tempBuffer[i] = *c;
				c = &prbi->constraints[i];
			}else{
				tempBuffer[i] = prbi->constraints[i];
			}
		}

		tempBuffer[i] = *c;
		free(prbi->constraints);
		prbi->constraints = tempBuffer;
		prbi->constraintCapacity = tempCapacity;

	}else{

		// Sort the new constraint into the array.
		i = prbi->constraintNum;
		while(i > 0){
			--i;
			if(c->constraintID < prbi->constraints[i].constraintID){
				prbi->constraints[i+1] = prbi->constraints[i];
			}else{
				prbi->constraints[i+1] = *c;
				break;
			}
		}

	}

	++prbi->constraintNum;
	return 1;

}

signed char physRBICacheSeparation(physRBInstance *prbi, physCollisionInfo *c){

	/*
	** Cache a separation after a failed narrowphase collision check.
	*/

	cacheIndex_t i;
	if(prbi->separationNum >= prbi->separationCapacity){

		// Allocate room for the new constraint.
		const cacheIndex_t tempCapacity = prbi->separationNum+1;
		physCollisionInfo *tempBuffer = malloc(tempCapacity*sizeof(physCollisionInfo));
		if(tempBuffer == NULL){
			/** Memory allocation failure. **/
			return -1;
		}

		// Sort the separations back into the array.
		for(i = 0; i < prbi->separationNum; ++i){
			if(c->collisionID < prbi->cache[i].collisionID){
				tempBuffer[i] = *c;
				c = &prbi->cache[i];
			}else{
				tempBuffer[i] = prbi->cache[i];
			}
		}

		tempBuffer[i] = *c;
		free(prbi->cache);
		prbi->cache = tempBuffer;
		prbi->separationCapacity = tempCapacity;

	}else{

		// Sort the new separation into the array.
		i = prbi->separationNum;
		while(i > 0){
			--i;
			if(c->collisionID < prbi->cache[i].collisionID){
				prbi->cache[i+1] = prbi->cache[i];
			}else{
				prbi->cache[i+1] = *c;
				break;
			}
		}

	}

	++prbi->separationNum;
	return 1;

}

signed char physRBIAddCollision(physRBInstance *prbi, physCollisionInfo *c){

	/*
	** Caches the collision and creates an
	** inequality constraint representing it.
	*/



}

static void physRBICentroidFromPosition(physRBInstance *prbi){
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

static void physRBIGenerateGlobalInertia(physRBInstance *prbi){

	mat3 orientationMatrix, inverseOrientationMatrix;

	// Generate 3x3 matrices for the orientation and the inverse orientation.
	mat3Quat(&orientationMatrix, &prbi->configuration->orientation);
	mat3TransposeR(&orientationMatrix, &inverseOrientationMatrix);

	// Multiply them against the local inertia tensor to get the global inverse moment of inertia.
	mat3MultMByMR(&orientationMatrix, &prbi->local->inertiaTensor, &prbi->inverseInertiaTensor);
	mat3MultMByM1(&prbi->inverseInertiaTensor, &inverseOrientationMatrix);

}

void physRBIUpdateCollisionMesh(physRBInstance *prbi){

	/*
	** Transform the vertices of each body into global space.
	*/

	if(prbi->local != NULL){  /** Remove? **/

		colliderIndex_t i;
		for(i = 0; i < prbi->local->colliderNum; ++i){

			// Update the collider.
			physColliderUpdate(&prbi->colliders[i], &prbi->local->colliders[i], &prbi->configuration[0]);

			// Update body minima and maxima.
			if(i == 0){
				// Initialize them to the first collider's bounding box.
				prbi->aabb.left = prbi->colliders[i].aabb.left;
				prbi->aabb.right = prbi->colliders[i].aabb.right;
				prbi->aabb.top = prbi->colliders[i].aabb.top;
				prbi->aabb.bottom = prbi->colliders[i].aabb.bottom;
				prbi->aabb.front = prbi->colliders[i].aabb.front;
				prbi->aabb.back = prbi->colliders[i].aabb.back;
			}else{
				// Update aabb.left and aabb.right.
				if(prbi->colliders[i].aabb.left <= prbi->aabb.left){
					prbi->aabb.left = prbi->colliders[i].aabb.left;
				}else if(prbi->colliders[i].aabb.right > prbi->aabb.right){
					prbi->aabb.right = prbi->colliders[i].aabb.right;
				}
				// Update aabb.top and aabb.bottom.
				if(prbi->colliders[i].aabb.top >= prbi->aabb.top){
					prbi->aabb.top = prbi->colliders[i].aabb.top;
				}else if(prbi->colliders[i].aabb.bottom < prbi->aabb.bottom){
					prbi->aabb.bottom = prbi->colliders[i].aabb.bottom;
				}
				// Update aabb.front and aabb.back.
				if(prbi->colliders[i].aabb.front >= prbi->aabb.front){
					prbi->aabb.front = prbi->colliders[i].aabb.front;
				}else if(prbi->colliders[i].aabb.back < prbi->aabb.back){
					prbi->aabb.back = prbi->colliders[i].aabb.back;
				}
			}

		}

	}

}

void physRBIApplyLinearForce(physRBInstance *prbi, const vec3 *F){
	/* Apply a linear force. */
	prbi->netForce.x += F->x;
	prbi->netForce.y += F->y;
	prbi->netForce.z += F->z;
}

void physRBIApplyAngularForceGlobal(physRBInstance *prbi, const vec3 *F, const vec3 *r){
	/* Apply an angular force. */
	// T = r x F
	vec3 rsR, rxF;
	vec3SubVFromVR(r, &prbi->centroid, &rsR);
	vec3Cross(&rsR, F, &rxF);
	vec3AddVToV(&prbi->netTorque, &rxF);
}

void physRBIApplyForceGlobal(physRBInstance *prbi, const vec3 *F, const vec3 *r){

	/*
	** Accumulate the net force and torque.
	** r is where the force F is applied, in world space.
	*/

	/* Accumulate torque. */
	physRBIApplyAngularForceGlobal(prbi, F, r);

	/* Accumulate force. */
	physRBIApplyLinearForce(prbi, F);

}

/*void physRBIApplyLinearImpulse(physRBInstance *prbi, const vec3 *j){
	* Apply a linear impulse. *
	prbi->linearVelocity.x += j->x * prbi->local->inverseMass;
	prbi->linearVelocity.y += j->y * prbi->local->inverseMass;
	prbi->linearVelocity.z += j->z * prbi->local->inverseMass;
}

void physRBIApplyAngularImpulse(physRBInstance *prbi, const vec3 *T){
	* Apply an angular impulse. *
	vec3 newTorque = *T;
	mat3MultMByVRow(&prbi->inverseInertiaTensor, &newTorque);
	prbi->angularVelocity.x += newTorque.x;
	prbi->angularVelocity.y += newTorque.y;
	prbi->angularVelocity.z += newTorque.z;
}

void physRBIApplyImpulseAtGlobalPoint(physRBInstance *prbi, const float j, const vec3 *r){
	vec3 T;
	vec3Cross(r, F, &T);
	physRBIApplyLinearImpulse(prbi, F->x, F->y, F->z);
	physRBIApplyAngularImpulse(prbi, &T);
}*/

static void physRBIResetForceAccumulator(physRBInstance *prbi){
	prbi->netForce.x = 0.f;
	prbi->netForce.y = 0.f;
	prbi->netForce.z = 0.f;
}

static void physRBIResetTorqueAccumulator(physRBInstance *prbi){
	prbi->netTorque.x = 0.f;
	prbi->netTorque.y = 0.f;
	prbi->netTorque.z = 0.f;
}

void physRBIBeginSimulation(physRBInstance *prbi){
	physRBIGenerateGlobalInertia(prbi);
	physRBICentroidFromPosition(prbi);
}

void physRBIIntegrateEuler(physRBInstance *prbi, const float dt){

	/* Euler integration scheme. */
	if(prbi->local != NULL){  /** Remove? **/

		/* Update moment of inertia. */
		physRBIGenerateGlobalInertia(prbi);

		if(prbi->local->inverseMass > 0.f){

			const float dtStep = dt/PHYS_INTEGRATION_STEPS_EULER;
			vec3 tempVec3;
			float tempFloat;
			quat tempQuat;
			unsigned int i;

			for(i = 0; i < PHYS_INTEGRATION_STEPS_EULER; ++i){

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

void physRBIIntegrateLeapfrog(physRBInstance *prbi, const float dt){

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

void physRBIIntegrateLeapfrogVelocity(physRBInstance *prbi, const float dt){

	/* Leapfrog integration scheme. */
	if(prbi->local != NULL && prbi->local->inverseMass > 0.f){  /** Remove? **/

		/* Integrate linear velocity half-step. */
		const float tempFloat = prbi->local->inverseMass * dt * 0.5f;
		prbi->linearVelocity.x += prbi->netForce.x * tempFloat;
		prbi->linearVelocity.y += prbi->netForce.y * tempFloat;
		prbi->linearVelocity.z += prbi->netForce.z * tempFloat;

	}

}

void physRBIIntegrateLeapfrogConstraints(physRBInstance *prbi, const float dt){

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

void physRBIIntegrateRungeKutta(physRBInstance *prbi, const float dt){

	/* RK4 integration scheme. */
	//

}

static inline void physRBIPenetrationSlop(physRBInstance *body1, physRBInstance *body2, const hbCollisionContactManifold *cm){

	// Baumgarte stabilization

}

static inline void physRBIResolveCollisionImpulse(physRBInstance *body1, physRBInstance *body2, const hbCollisionContactManifold *cm){

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
	if(normalVelocity > PHYS_RESTING_EPSILON){

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

void physRBIResolveCollisionGS(physRBInstance *body1, physRBInstance *body2, const hbCollisionContactManifold *cm){

	/*
	** Uses the Gauss-Seidel method to solve the
	** impulse magnitude equation as a system of
	** linear equations with multiple points of
	** contact.
	*/

	//

}

void physRBIUpdate(physRBInstance *prbi, const float dt){

	//

}

void physRBIDelete(physRBInstance *prbi){
	colliderIndex_t i;
	if(prbi->colliders != NULL){
		hbMesh *cHull;
		for(i = 0; i < prbi->local->colliderNum; ++i){
			// Only free vertices and normals, as we re-use edges from the local collider's hull.
			cHull = (hbMesh *)&prbi->colliders[i].hb.hull;
			free(cHull->vertices);
			free(cHull->normals);
		}
		free(prbi->colliders);
	}
	if(prbi->constraints != NULL){
		free(prbi->constraints);
	}
}
