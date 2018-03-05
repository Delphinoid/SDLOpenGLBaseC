#version 330 core

// Maximum number of bones allowed per skeleton.
#define SKL_MAX_BONE_NUM 128

// Bone struct.
struct bone {
	vec3 position;
	vec4 orientation;
	vec3 scale;
};

// Per-vertex data.
layout(location = 0) in vec3  vertexPosition;
layout(location = 1) in vec2  vertexUV;
layout(location = 2) in vec3  vertexNormals;
layout(location = 3) in ivec4 vertexBoneIDs;
layout(location = 4) in vec4  vertexBoneWeights;

// Per-instance data (same as per-call for now since we're not using instancing).
uniform mat4 mvpMatrix;
uniform vec4 textureFragment;
uniform bone boneArray[SKL_MAX_BONE_NUM];

// Per-call data.
uniform sampler2D textureSampler0;

out vec2 UV;

void quatRotateVec3(in vec4 q, inout vec3 v){
	
	//                          //
	// Rotates v by negative q. //
	//                          //
	
	float dotQV = q.x * v.x + q.y * v.y + q.z * v.z;
	float dotQQ = q.x * q.x + q.y * q.y + q.z * q.z;
	float m = q.w*q.w - dotQQ;
	vec3 crossQV = vec3(q.y * v.z - q.z * v.y,
	                    q.z * v.x - q.x * v.z,
	                    q.x * v.y - q.y * v.x);

	v.x *= m;
	v.y *= m;
	v.z *= m;

	m = 2.0 * dotQV;
	v.x += m * q.x;
	v.y += m * q.y;
	v.z += m * q.z;

	m = 2.0 * -q.w;
	v.x += m * crossQV.x;
	v.y += m * crossQV.y;
	v.z += m * crossQV.z;
	
}

void main(){
	
	// Transform the vertex.
	vec3 vertex = vertexPosition;
	
	// Apply each bone transformation (array indices must be const, so no for loops).
	if(vertexBoneIDs[0] >= 0 && vertexBoneIDs[0] < SKL_MAX_BONE_NUM && vertexBoneWeights[0] != 0){
		// These should be weighted!
		quatRotateVec3(boneArray[vertexBoneIDs[0]].orientation, vertex);
		vertex += boneArray[vertexBoneIDs[0]].position;
		vertex *= boneArray[vertexBoneIDs[0]].scale;
	}
	if(vertexBoneIDs[1] >= 0 && vertexBoneIDs[1] < SKL_MAX_BONE_NUM && vertexBoneWeights[1] != 0){
		// These should be weighted!
		quatRotateVec3(boneArray[vertexBoneIDs[1]].orientation, vertex);
		vertex += boneArray[vertexBoneIDs[1]].position;
		vertex *= boneArray[vertexBoneIDs[1]].scale;
	}
	if(vertexBoneIDs[2] >= 0 && vertexBoneIDs[2] < SKL_MAX_BONE_NUM && vertexBoneWeights[2] != 0){
		// These should be weighted!
		quatRotateVec3(boneArray[vertexBoneIDs[2]].orientation, vertex);
		vertex += boneArray[vertexBoneIDs[2]].position;
		vertex *= boneArray[vertexBoneIDs[2]].scale;
	}
	if(vertexBoneIDs[3] >= 0 && vertexBoneIDs[3] < SKL_MAX_BONE_NUM && vertexBoneWeights[3] != 0){
		// These should be weighted!
		quatRotateVec3(boneArray[vertexBoneIDs[3]].orientation, vertex);
		vertex += boneArray[vertexBoneIDs[3]].position;
		vertex *= boneArray[vertexBoneIDs[3]].scale;
	}
	
	// Finally, apply it all to the vertex position.
	gl_Position = mvpMatrix * vec4(vertex, 1.0);
	
	// Get the width and height of the texture.
	ivec2 textureDimensions = textureSize(textureSampler0, 0);
	
	// Calculate UVs for the current vertex based on the texture dimensions.
	if(textureFragment[0] != 0.0 || textureFragment[1] != 0.0 ||
	                                textureFragment[2] != textureDimensions.x ||
	                                textureFragment[3] != textureDimensions.y){

		// You can remove this if statement and just keep the following two
		// lines, but it may be slightly slower in very specific situations.
		UV.x = ((vertexUV.x * textureFragment[2]) + textureFragment[0]) / textureDimensions.x;
		UV.y = ((vertexUV.y * textureFragment[3]) + textureFragment[1]) / textureDimensions.y;  // Flip the y dimension so the image isn't upside down.

	}else{
		// Mostly used for sprites, since the offset calculations are done on the CPU.
		UV.x = vertexUV.x;
		UV.y = vertexUV.y;
	}

}