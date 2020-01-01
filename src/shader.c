#include "shader.h"
#include "memoryManager.h"
#include "skeletonShared.h"
#include "helpersFileIO.h"
#include "helpersMisc.h"
#include "inline.h"
#include <string.h>
#include <stdio.h>

#define SHADER_RESOURCE_DIRECTORY_STRING "Resources"FILE_PATH_DELIMITER_STRING"Shaders"FILE_PATH_DELIMITER_STRING
#define SHADER_RESOURCE_DIRECTORY_LENGTH 18

void shdrDataInit(shaderData *const restrict shdrData){
	shdrData->lastTexID = 0;
	shdrData->identityMatrix = mat4Identity();
	shdrData->biasMIP = SHADER_DEFAULT_BIAS_MIP;
	shdrData->biasLOD = SHADER_DEFAULT_BIAS_LOD;
}

static __FORCE_INLINE__ return_t shdrLoad(GLuint *const restrict id, GLenum type, const char *const restrict prgPath, const char *const restrict filePath){

	char fullPath[FILE_MAX_PATH_LENGTH];
	const size_t fileLength = strlen(filePath);

	FILE *shdrInfo;
	long shdrSize;
	GLchar *shdrCode;

	GLint compiled = GL_FALSE;

	fileGenerateFullPath(fullPath, prgPath, strlen(prgPath), SHADER_RESOURCE_DIRECTORY_STRING, SHADER_RESOURCE_DIRECTORY_LENGTH, filePath, fileLength);

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

return_t shdrPrgLoad(GLuint *const restrict id, const char *const restrict prgPath, const char *const restrict vertexPath, const char *const restrict fragmentPath){

	// Generate shaders.
	return_t r;
	GLuint vertexShaderID, fragmentShaderID;
	if(
		(r = shdrLoad(&vertexShaderID, GL_VERTEX_SHADER, prgPath, vertexPath)) <= 0 ||
		(r = shdrLoad(&fragmentShaderID, GL_FRAGMENT_SHADER, prgPath, fragmentPath)) <= 0
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

return_t shdrPrgObjLink(shaderProgramObject *const restrict shdrPrg){

	GLenum glError;
	boneIndex_t i;

	glUseProgram(shdrPrg->id);

	// Link the uniform variables.
	shdrPrg->vpMatrixID = glGetUniformLocation(shdrPrg->id, "vpMatrix");
	shdrPrg->alphaID = glGetUniformLocation(shdrPrg->id, "alpha");
	shdrPrg->mipID = glGetUniformLocation(shdrPrg->id, "mip");

	glUniform1f(shdrPrg->mipID, 0);

	// Create references to each bone.
	for(i = 0; i < SKELETON_MAX_BONE_NUM; ++i){

		char num[LTOSTR_MAX_LENGTH];
		const size_t numLen = ltostr(i, &num[0]);
		char uniformString[11+LTOSTR_MAX_LENGTH];  // LTOSTR_MAX_LENGTH includes NULL terminator.

		memcpy(&uniformString[0], "boneArray[", 10*sizeof(char));
		memcpy(&uniformString[10], num, numLen*sizeof(char));
		uniformString[10+numLen] = ']';
		uniformString[10+numLen+1] = '\0';
		shdrPrg->boneArrayID[i] = glGetUniformLocation(shdrPrg->id, uniformString);

	}

	// Create references to each texture sampler.
	for(i = 0; i < SHADER_TEXTURE_SAMPLER_NUM; ++i){

		char num[LTOSTR_MAX_LENGTH];
		const size_t numLen = ltostr(i, &num[0]);
		char uniformString[17+LTOSTR_MAX_LENGTH];  // LTOSTR_MAX_LENGTH includes NULL terminator.

		memcpy(&uniformString[0], "textureFragment[", 16);
		memcpy(&uniformString[16], num, numLen);
		uniformString[16+numLen] = ']';
		uniformString[16+numLen+1] = '\0';
		shdrPrg->textureFragmentID[i] = glGetUniformLocation(shdrPrg->id, uniformString);

		memcpy(&uniformString[0], "textureSampler[", 15);
		memcpy(&uniformString[15], num, numLen);
		uniformString[15+numLen] = ']';
		uniformString[15+numLen+1] = '\0';
		shdrPrg->textureSamplerArrayID[i] = glGetUniformLocation(shdrPrg->id, uniformString);
		glUniform1i(shdrPrg->textureSamplerArrayID[i], 0);

	}

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading object shader: %u\n", glError);
		return 0;
	}

 	return 1;

}

return_t shdrPrgSprLink(shaderProgramSprite *const restrict shdrPrg){

	GLenum glError;

	glUseProgram(shdrPrg->id);

	// Link the uniform variables.
	shdrPrg->vpMatrixID = glGetUniformLocation(shdrPrg->id, "vpMatrix");
	shdrPrg->alphaID = glGetUniformLocation(shdrPrg->id, "alpha");
	shdrPrg->mipID = glGetUniformLocation(shdrPrg->id, "mip");
	shdrPrg->textureSamplerID = glGetUniformLocation(shdrPrg->id, "textureSampler");

	glUniform1f(shdrPrg->alphaID, 1.f);
	glUniform1f(shdrPrg->mipID, SHADER_DEFAULT_BIAS_MIP);
	glUniform1i(shdrPrg->textureSamplerID, 0);

	glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error loading sprite shader: %u\n", glError);
		return 0;
	}

 	return 1;

}

__FORCE_INLINE__ void shdrPrgDelete(const void *const restrict shdrPrg){
	glDeleteProgram(*((const GLuint *const restrict)shdrPrg));
}