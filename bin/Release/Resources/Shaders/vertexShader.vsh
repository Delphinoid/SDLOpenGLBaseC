#version 330 core

// Maximum number of bones allowed per skeleton
#define MAX_BONE_NUM 128

// Per-vertex data
layout(location = 0) in vec3  vertexPosition;
layout(location = 1) in vec2  vertexUV;
layout(location = 2) in vec3  vertexNormals;
layout(location = 3) in ivec4 vertexBoneIDs;
layout(location = 4) in vec4  vertexBoneWeights;

// Per-instance data (same as per-call for now since we're not using instancing)
uniform mat4 mvpMatrix;
uniform vec4 textureFragment;
uniform mat4 boneArray[MAX_BONE_NUM];

// Per-call data
uniform sampler2D textureSampler0;

out vec2 UV;

void main(){
	
	// Start with the MVP matrix
	mat4 transform = mvpMatrix;
	
	// Apply each bone transformation (array indices must be const, so no for loops)
	if(vertexBoneIDs[0] >= 0 && vertexBoneIDs[0] < MAX_BONE_NUM && vertexBoneWeights[0] != 0){
		transform *= boneArray[vertexBoneIDs[0]];// * vertexBoneWeights[0];
	}
	if(vertexBoneIDs[1] >= 0 && vertexBoneIDs[1] < MAX_BONE_NUM && vertexBoneWeights[1] != 0){
		transform *= boneArray[vertexBoneIDs[1]];// * vertexBoneWeights[1];
	}
	if(vertexBoneIDs[2] >= 0 && vertexBoneIDs[2] < MAX_BONE_NUM && vertexBoneWeights[2] != 0){
		transform *= boneArray[vertexBoneIDs[2]];// * vertexBoneWeights[2];
	}
	if(vertexBoneIDs[3] >= 0 && vertexBoneIDs[3] < MAX_BONE_NUM && vertexBoneWeights[3] != 0){
		transform *= boneArray[vertexBoneIDs[3]];// * vertexBoneWeights[3];
	}
	
	// Finally, apply it all to the vertex position
	gl_Position = transform * vec4(vertexPosition, 1.0);
	
	// Get the width and height of the texture
	ivec2 textureDimensions = textureSize(textureSampler0, 0);
	
	// Calculate UVs for the current vertex based on the texture dimensions
	if(textureFragment[0] != 0.0 || textureFragment[1] != 0.0 ||
	                                textureFragment[2] != textureDimensions.x ||
	                                textureFragment[3] != textureDimensions.y){

		// You can remove this if statement and just keep the following two
		// lines, but it may be slightly slower in very specific situations
		UV.x = ((vertexUV.x * textureFragment[2]) + textureFragment[0]) / textureDimensions.x;
		UV.y = ((vertexUV.y * textureFragment[3]) + textureFragment[1]) / textureDimensions.y;  // Flip the y dimension so the image isn't upside down

	}else{
		// Mostly used for sprites, since the offset calculations are done on the CPU
		UV.x = vertexUV.x;
		UV.y = vertexUV.y;
	}

}