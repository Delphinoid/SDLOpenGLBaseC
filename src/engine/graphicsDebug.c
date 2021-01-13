#include "graphicsDebug.h"
#include "shader.h"
#include "memoryManager.h"

typedef struct {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	size_t indexNum;
	GLuint drawMode;
} gfxDebugMesh;

typedef struct {
	GLuint id;
	GLuint vpMatrixID;
	GLuint colourID;
} shaderProgramDebug;

shaderProgramDebug shdrPrgDbg;

return_t gfxDebugLoadShaderProgram(){
	// Load the debug shader program.
	return_t r;
	if((r = shdrPrgLoad(&shdrPrgDbg.id, "s_vertex_debug.gls", 18, "s_fragment_debug.gls", 20)) > 0){
		// Link the uniform variables.
		glUseProgram(shdrPrgDbg.id);
		shdrPrgDbg.vpMatrixID = glGetUniformLocation(shdrPrgDbg.id, "vpMatrix");
		shdrPrgDbg.colourID = glGetUniformLocation(shdrPrgDbg.id, "meshColour");
	}
	return r;
}
__FORCE_INLINE__ void gfxDebugDeleteShaderProgram(){
	shdrPrgDelete((void *)&shdrPrgDbg);
}

static void gfxDebugMeshGenerateBuffers(
	gfxDebugMesh *const __RESTRICT__ dbgMesh,
	const vec3 *const __RESTRICT__ vertices, const size_t vertexNum,
	const size_t *const __RESTRICT__ indices, const size_t indexNum
){

	// Generate a vertex array object for our mesh and bind it!
	glGenVertexArrays(1, &dbgMesh->vertexArrayID);
	glBindVertexArray(dbgMesh->vertexArrayID);

	// Generate a buffer object for our vertex data and bind it!
	glGenBuffers(1, &dbgMesh->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, dbgMesh->vertexBufferID);
	// Now add all our data to it!
	glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * vertexNum, vertices, GL_STREAM_DRAW);

	// Set up the vertex array object attributes that require this buffer!
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), 0);

	// Generate a buffer object for our indices and bind it!
	glGenBuffers(1, &dbgMesh->indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dbgMesh->indexBufferID);
	// Now add all our data to it!
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * indexNum, indices, GL_STREAM_DRAW);

}

static void gfxDebugMeshDrawBuffers(
	const gfxDebugMesh *const __RESTRICT__ dbgMesh, const gfxDebugInfo *const __RESTRICT__ info, const mat4 *const __RESTRICT__ vpMatrix
){
	// Draw a debug mesh. This assumes the vertex array is bound.
	glPolygonMode(GL_FRONT_AND_BACK, info->fillMode);
	glUniform3fv(shdrPrgDbg.colourID, 1, (GLfloat *)&info->colour);
	glUniformMatrix4fv(shdrPrgDbg.vpMatrixID, 1, GL_FALSE, (GLfloat *)vpMatrix);
	glDrawElements(dbgMesh->drawMode, dbgMesh->indexNum, GL_UNSIGNED_INT, NULL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void gfxDebugMeshDelete(const gfxDebugMesh *const __RESTRICT__ dbgMesh){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our mesh structure to have no padding.
	glDeleteBuffers(2, &dbgMesh->vertexBufferID);
	glDeleteVertexArrays(1, &dbgMesh->vertexArrayID);
}

gfxDebugInfo gfxDebugInfoInit(const unsigned int fillMode, const vec3 colour){
	const gfxDebugInfo info = {
		.fillMode = fillMode,
		.colour = colour
	};
	return info;
}

void gfxDebugDrawSkeleton(
	const vec3 *const __RESTRICT__ positions, const boneIndex_t *const __RESTRICT__ parents, const boneIndex_t boneNum,
	const gfxDebugInfo info, const mat4 *const __RESTRICT__ vpMatrix
){

	gfxDebugMesh dbgMesh;
	size_t *indices; boneIndex_t i;
	GLint prevArrayObject, prevShader;

	// Set up the index array.
	dbgMesh.drawMode = GL_LINES;
	dbgMesh.indexNum = (boneNum - 1) * 2;
	indices = memAllocate(dbgMesh.indexNum*sizeof(size_t));
	i = 1;
	for(i = 1; i < boneNum; ++i){
		indices[(i<<1)-2] = i;
		indices[(i<<1)-1] = parents[i];
	}

	// Make sure we keep the current global state so we can restore it after drawing.
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevArrayObject);
	glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);

	// Bind the debug shader for when we draw the mesh.
	glUseProgram(shdrPrgDbg.id);
	// Generate temporary buffers for the mesh data.
	gfxDebugMeshGenerateBuffers(&dbgMesh, positions, boneNum, indices, dbgMesh.indexNum);
	// Draw the mesh from the buffers.
	gfxDebugMeshDrawBuffers(&dbgMesh, &info, vpMatrix);
	// Now that we've drawn the mesh, we can destroy the buffers.
	gfxDebugMeshDelete(&dbgMesh);

	// Restore the global state.
	glUseProgram(prevShader);
	glBindVertexArray(prevArrayObject);

	memFree(indices);

}