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
uniform mat4 vpMatrix;
uniform vec4 textureFragment;
uniform mat4 boneArray[SKL_MAX_BONE_NUM];

// Per-call data.
uniform sampler2D textureSampler0;

out vec2 UV;

void main(){
	
	// Transform the vertex.
	vec4 vertexPosition4 = vec4(vertexPosition, 1.0);
	vec4 vertex = vertexPosition4;
	
	// Apply each bone transformation (array indices must be const, so no for loops).
	if(vertexBoneIDs[0] >= 0 && vertexBoneIDs[0] < SKL_MAX_BONE_NUM && vertexBoneWeights[0] != 0.0){
		vertex = boneArray[vertexBoneIDs[0]] * vertexPosition4 * vertexBoneWeights[0];
	}
	if(vertexBoneIDs[1] >= 0 && vertexBoneIDs[1] < SKL_MAX_BONE_NUM && vertexBoneWeights[1] != 0.0){
		vertex = boneArray[vertexBoneIDs[1]] * vertexPosition4 * vertexBoneWeights[1] + vertex;
	}
	if(vertexBoneIDs[2] >= 0 && vertexBoneIDs[2] < SKL_MAX_BONE_NUM && vertexBoneWeights[2] != 0.0){
		vertex = boneArray[vertexBoneIDs[2]] * vertexPosition4 * vertexBoneWeights[2] + vertex;
	}
	if(vertexBoneIDs[3] >= 0 && vertexBoneIDs[3] < SKL_MAX_BONE_NUM && vertexBoneWeights[3] != 0.0){
		vertex = boneArray[vertexBoneIDs[3]] * vertexPosition4 * vertexBoneWeights[3] + vertex;
	}
	
	// Finally, apply it all to the vertex position.
	gl_Position = vpMatrix * vec4(vertex.xyz, 1.0);
	
	// Get the width and height of the texture.
	ivec2 textureDimensions = textureSize(textureSampler0, 0);
	
	// Calculate UVs for the current vertex based on the texture dimensions.
	if(textureFragment[0] != 0.0 || textureFragment[1] != 0.0 || textureFragment[2] != 1.0 || textureFragment[3] != 1.0){
		UV.x = vertexUV.x * textureFragment[2] + textureFragment[0];
		UV.y = vertexUV.y * textureFragment[3] + textureFragment[1];
	}else{
		// Mostly used for sprites, since the offset calculations are done on the CPU.
		UV.x = vertexUV.x;
		UV.y = vertexUV.y;
	}

}