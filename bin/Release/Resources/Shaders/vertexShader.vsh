#version 330 core

// Maximum number of bones allowed per skeleton
#define MAX_BONE_NUM 128

// Struct definitions
struct quat {
	float w;
	vec3 v;
};
struct bone {
	vec3 position;
	quat orientation;
};

// Per-vertex data
layout(location = 0) in vec3  vertexPosition;
layout(location = 1) in vec2  vertexUV;
layout(location = 2) in vec3  vertexNormals;
layout(location = 3) in ivec4 vertexBoneIDs;
layout(location = 4) in vec4  vertexBoneWeights;

// Per-instance data (same as per-call for now since we're not using instancing)
uniform mat4 mvpMatrix;
uniform vec4 textureFragment;
uniform bone boneArray[MAX_BONE_NUM];

// Per-call data
uniform sampler2D textureSampler0;

out vec2 UV;

void main(){

	gl_Position =  mvpMatrix * vec4(vertexPosition, 1.0);
	ivec2 textureDimensions = textureSize(textureSampler0, 0);

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