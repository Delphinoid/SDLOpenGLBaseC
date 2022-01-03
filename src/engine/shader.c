#include "shader.h"
#include "memoryManager.h"
#include "skeletonShared.h"
#include "helpersFileIO.h"
#include "helpersMisc.h"
#include <string.h>
#include <stdio.h>

#define SHADER_RESOURCE_DIRECTORY_STRING FILE_PATH_RESOURCE_DIRECTORY_SHARED"Resources"FILE_PATH_DELIMITER_STRING"Shaders"FILE_PATH_DELIMITER_STRING
#define SHADER_RESOURCE_DIRECTORY_LENGTH 20

void shdrDataInit(shaderData *const __RESTRICT__ shdrData){
	shdrData->lastTexID = 0;
	shdrData->identityMatrix = g_mat4Identity;
	shdrData->biasMIP = SHADER_DEFAULT_BIAS_MIP;
	shdrData->biasLOD = SHADER_DEFAULT_BIAS_LOD;
}

return_t shdrLoad(GLuint *const __RESTRICT__ id, GLenum type, const char *const __RESTRICT__ filePath, const size_t filePathLength){

	char fullPath[FILE_MAX_PATH_LENGTH];

	FILE *shdrInfo;
	long shdrSize;
	GLchar *shdrCode;

	GLint compiled = GL_FALSE;

	fileGenerateFullPath(fullPath, SHADER_RESOURCE_DIRECTORY_STRING, SHADER_RESOURCE_DIRECTORY_LENGTH, filePath, filePathLength);

	// Load the shader.
	shdrInfo = fopen(fullPath, "rb");
	fseek(shdrInfo, 0, SEEK_END);
	shdrSize = ftell(shdrInfo);
	rewind(shdrInfo);
	shdrCode = memAllocate((shdrSize+1)*sizeof(char));
	if(shdrCode == NULL){
		/** Memory allocation failure. **/
		fclose(shdrInfo);
		return -1;
	}
	if(fread(shdrCode, sizeof(char), shdrSize, shdrInfo) != shdrSize){
		memFree(shdrCode);
		fclose(shdrInfo);
		return 0;
	}
	shdrCode[shdrSize] = '\0';
	fclose(shdrInfo);

	// Compile the shader.
	*id = glCreateShader(type);
	glShaderSource(*id, 1, (const GLchar *const *)&shdrCode, NULL);
	glCompileShader(*id);
	memFree(shdrCode);

	// Validate the shader.
	glGetShaderiv(*id, GL_COMPILE_STATUS, &compiled);
 	if(compiled == GL_FALSE){
		char *error;
		int infoLogLength;
		glGetShaderiv(*id, GL_INFO_LOG_LENGTH, &infoLogLength);
		error = memAllocate((infoLogLength+1)*sizeof(char));
		if(error == NULL){
			/** Memory allocation failure. **/
			return -1;
		}
 		glGetShaderInfoLog(*id, infoLogLength, NULL, error);
 		printf("Error validating shader \"%s\": %s", fullPath, error);
 		memFree(error);
 		return 0;
 	}

 	return 1;

}

return_t shdrPrgLoad(GLuint *const __RESTRICT__ id, const char *const __RESTRICT__ vertexPath, const size_t vertexPathLength, const char *const __RESTRICT__ fragmentPath, const size_t fragmentPathLength){

	// Generate shaders.
	return_t r;
	GLuint vertexShaderID, fragmentShaderID;
	if(
		(r = shdrLoad(&vertexShaderID, GL_VERTEX_SHADER, vertexPath, vertexPathLength)) <= 0 ||
		(r = shdrLoad(&fragmentShaderID, GL_FRAGMENT_SHADER, fragmentPath, fragmentPathLength)) <= 0
	){
		return r;
	}

	// Link the program.
	*id = glCreateProgram();
 	glAttachShader(*id, vertexShaderID);
 	glAttachShader(*id, fragmentShaderID);
 	glLinkProgram(*id);

	// Detach and free shaders.
 	glDetachShader(*id, vertexShaderID);
 	glDetachShader(*id, fragmentShaderID);
 	glDeleteShader(vertexShaderID);
 	glDeleteShader(fragmentShaderID);

	return 1;

}

return_t shdrPrgObjLink(shaderProgramObject *const __RESTRICT__ shdrPrg){

	GLenum glError;
	glUseProgram(shdrPrg->id);

	// Link the uniform variables.
	shdrPrg->vpMatrixID = glGetUniformLocation(shdrPrg->id, "vpMatrix");
	shdrPrg->alphaID = glGetUniformLocation(shdrPrg->id, "alpha");
	shdrPrg->mipID = glGetUniformLocation(shdrPrg->id, "mip");
	shdrPrg->boneArrayID = glGetUniformLocation(shdrPrg->id, "boneArray");
	shdrPrg->textureFragmentArrayID = glGetUniformLocation(shdrPrg->id, "textureFragmentArray");
	shdrPrg->textureSamplerArrayID = glGetUniformLocation(shdrPrg->id, "textureSamplerArray");

	// Initialize the uniform variables.
	glUniform1f(shdrPrg->mipID, 0);
	glUniform1i(shdrPrg->textureSamplerArrayID, 0);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading object shader: %u\n", glError);
		return 0;
	}

 	return 1;

}

return_t shdrPrgSprLink(shaderProgramSprite *const __RESTRICT__ shdrPrg){

	GLenum glError;

	glUseProgram(shdrPrg->id);

	// Link the uniform variables.
	shdrPrg->vpMatrixID = glGetUniformLocation(shdrPrg->id, "vpMatrix");
	shdrPrg->alphaID = glGetUniformLocation(shdrPrg->id, "alpha");
	shdrPrg->mipID = glGetUniformLocation(shdrPrg->id, "mip");
	shdrPrg->sdfTypeID = glGetUniformLocation(shdrPrg->id, "sdfType");
	shdrPrg->sdfColourID = glGetUniformLocation(shdrPrg->id, "sdfColour");
	shdrPrg->sdfBackgroundID = glGetUniformLocation(shdrPrg->id, "sdfBackground");
	shdrPrg->textureSamplerID = glGetUniformLocation(shdrPrg->id, "textureSampler");

	// Initialize the uniform variables.
	glUniform1f(shdrPrg->alphaID, 1.f);
	glUniform1f(shdrPrg->mipID, SHADER_DEFAULT_BIAS_MIP);
	glUniform1ui(shdrPrg->sdfTypeID, SHADER_SDF_MODE_DISABLED);
	glUniform4f(shdrPrg->sdfColourID, 0.f, 0.f, 0.f, 1.f);
	glUniform4f(shdrPrg->sdfBackgroundID, 0.f, 0.f, 0.f, 0.f);
	glUniform1i(shdrPrg->textureSamplerID, 0);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading sprite shader: %u\n", glError);
		return 0;
	}

 	return 1;

}

__FORCE_INLINE__ void shdrPrgDelete(const void *const __RESTRICT__ shdrPrg){
	glDeleteProgram(*((const GLuint *const __RESTRICT__)shdrPrg));
}
