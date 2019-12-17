#include "graphicsManager.h"
#include "gui.h"
#include "vertex.h"
#include "texture.h"
#include "skeleton.h"
#include "model.h"
#include "constantsMath.h"
#include "helpersMath.h"
#include "inline.h"

/**
// Default panel model.
static model mdlPanelDefault = {
	.skl = NULL,
	.lodNum = 0,
	.lods = NULL,
	.vertexNum = 0,
	.indexNum = 0,
	.vaoID = 0,
	.vboID = 0,
	.iboID = 0,
	.name = "panel"
};

return_t guiPanelInit(guiElement *const restrict element, const rectangle areas[4]){

	// The areas array contains the bounds for the
	// four corners of the panel on the texture.

	vertex vertices[16];
	vertexIndex_t indices[54];

	// Top left.
	vertices[0].position = vec3New(-0.5f, 0.5f, -1.f);
	vertices[0].u = areas[0].x; vertices[0].v = areas[0].y;
	vertices[0].normal = vec3New(0.f, 1.f, 0.f);
	vertices[0].bIDs[0] = 0; vertices[0].bIDs[1] = -1; vertices[0].bIDs[2] = -1; vertices[0].bIDs[3] = -1;
	vertices[0].bWeights[0] = 1.f; vertices[0].bWeights[1] = 0.f; vertices[0].bWeights[2] = 0.f; vertices[0].bWeights[3] = 0.f;
	vertices[1].position = vec3New(-1.f/6.f, 0.5f, -1.f);
	vertices[1].u = areas[0].x + areas[0].w; vertices[1].v = areas[0].y;
	vertices[1].normal = vec3New(0.f, 1.f, 0.f);
	vertices[1].bIDs[0] = 0; vertices[1].bIDs[1] = -1; vertices[1].bIDs[2] = -1; vertices[1].bIDs[3] = -1;
	vertices[1].bWeights[0] = 1.f; vertices[1].bWeights[1] = 0.f; vertices[1].bWeights[2] = 0.f; vertices[1].bWeights[3] = 0.f;
	vertices[2].position = vec3New(-0.5f, 1.f/6.f, -1.f);
	vertices[2].u = areas[0].x; vertices[2].v = areas[0].y + areas[0].h;
	vertices[2].normal = vec3New(0.f, 1.f, 0.f);
	vertices[2].bIDs[0] = 0; vertices[2].bIDs[1] = -1; vertices[2].bIDs[2] = -1; vertices[2].bIDs[3] = -1;
	vertices[2].bWeights[0] = 1.f; vertices[2].bWeights[1] = 0.f; vertices[2].bWeights[2] = 0.f; vertices[2].bWeights[3] = 0.f;
	vertices[3].position = vec3New(-1.f/6.f, 1.f/6.f, -1.f);
	vertices[3].u = areas[0].x + areas[0].w; vertices[3].v = areas[0].y + areas[0].h;
	vertices[3].normal = vec3New(0.f, 1.f, 0.f);
	vertices[3].bIDs[0] = 0; vertices[3].bIDs[1] = -1; vertices[3].bIDs[2] = -1; vertices[3].bIDs[3] = -1;
	vertices[3].bWeights[0] = 1.f; vertices[3].bWeights[1] = 0.f; vertices[3].bWeights[2] = 0.f; vertices[3].bWeights[3] = 0.f;
	// Top right.
	vertices[4].position = vec3New(1.f/6.f, 0.5f, -1.f);
	vertices[4].u = areas[1].x; vertices[4].v = areas[1].y;
	vertices[4].normal = vec3New(0.f, 1.f, 0.f);
	vertices[4].bIDs[0] = 1; vertices[4].bIDs[1] = -1; vertices[4].bIDs[2] = -1; vertices[4].bIDs[3] = -1;
	vertices[4].bWeights[0] = 1.f; vertices[4].bWeights[1] = 0.f; vertices[4].bWeights[2] = 0.f; vertices[4].bWeights[3] = 0.f;
	vertices[5].position = vec3New(0.5f, 0.5f, -1.f);
	vertices[5].u = areas[1].x + areas[1].w; vertices[5].v = areas[1].y;
	vertices[5].normal = vec3New(0.f, 1.f, 0.f);
	vertices[5].bIDs[0] = 1; vertices[5].bIDs[1] = -1; vertices[5].bIDs[2] = -1; vertices[5].bIDs[3] = -1;
	vertices[5].bWeights[0] = 1.f; vertices[5].bWeights[1] = 0.f; vertices[5].bWeights[2] = 0.f; vertices[5].bWeights[3] = 0.f;
	vertices[6].position = vec3New(1.f/6.f, 1.f/6.f, -1.f);
	vertices[6].u = areas[1].x; vertices[6].v = areas[1].y + areas[1].h;
	vertices[6].normal = vec3New(0.f, 1.f, 0.f);
	vertices[6].bIDs[0] = 1; vertices[6].bIDs[1] = -1; vertices[6].bIDs[2] = -1; vertices[6].bIDs[3] = -1;
	vertices[6].bWeights[0] = 1.f; vertices[6].bWeights[1] = 0.f; vertices[6].bWeights[2] = 0.f; vertices[6].bWeights[3] = 0.f;
	vertices[7].position = vec3New(0.5f, 1.f/6.f, -1.f);
	vertices[7].u = areas[1].x + areas[1].w; vertices[7].v = areas[1].y + areas[1].h;
	vertices[7].normal = vec3New(0.f, 1.f, 0.f);
	vertices[7].bIDs[0] = 1; vertices[7].bIDs[1] = -1; vertices[7].bIDs[2] = -1; vertices[7].bIDs[3] = -1;
	vertices[7].bWeights[0] = 1.f; vertices[7].bWeights[1] = 0.f; vertices[7].bWeights[2] = 0.f; vertices[7].bWeights[3] = 0.f;
	// Bottom left.
	vertices[8].position = vec3New(-0.5f, -1.f/6.f, -1.f);
	vertices[8].u = areas[2].x; vertices[8].v = areas[2].y;
	vertices[8].normal = vec3New(0.f, 1.f, 0.f);
	vertices[8].bIDs[0] = 2; vertices[8].bIDs[1] = -1; vertices[8].bIDs[2] = -1; vertices[8].bIDs[3] = -1;
	vertices[8].bWeights[0] = 1.f; vertices[8].bWeights[1] = 0.f; vertices[8].bWeights[2] = 0.f; vertices[8].bWeights[3] = 0.f;
	vertices[9].position = vec3New(-1.f/6.f, -1.f/6.f, -1.f);
	vertices[9].u = areas[2].x + areas[2].w; vertices[9].v = areas[2].y;
	vertices[9].normal = vec3New(0.f, 1.f, 0.f);
	vertices[9].bIDs[0] = 2; vertices[9].bIDs[1] = -1; vertices[9].bIDs[2] = -1; vertices[9].bIDs[3] = -1;
	vertices[9].bWeights[0] = 1.f; vertices[9].bWeights[1] = 0.f; vertices[9].bWeights[2] = 0.f; vertices[9].bWeights[3] = 0.f;
	vertices[10].position = vec3New(-0.5f, -0.5f, -1.f);
	vertices[10].u = areas[2].x; vertices[10].v = areas[2].y + areas[2].h;
	vertices[10].normal = vec3New(0.f, 1.f, 0.f);
	vertices[10].bIDs[0] = 2; vertices[10].bIDs[1] = -1; vertices[10].bIDs[2] = -1; vertices[10].bIDs[3] = -1;
	vertices[10].bWeights[0] = 1.f; vertices[10].bWeights[1] = 0.f; vertices[10].bWeights[2] = 0.f; vertices[10].bWeights[3] = 0.f;
	vertices[11].position = vec3New(-1.f/6.f, -0.5f, -1.f);
	vertices[11].u = areas[2].x + areas[2].w; vertices[11].v = areas[2].y + areas[2].h;
	vertices[11].normal = vec3New(0.f, 1.f, 0.f);
	vertices[11].bIDs[0] = 2; vertices[11].bIDs[1] = -1; vertices[11].bIDs[2] = -1; vertices[11].bIDs[3] = -1;
	vertices[11].bWeights[0] = 1.f; vertices[11].bWeights[1] = 0.f; vertices[11].bWeights[2] = 0.f; vertices[11].bWeights[3] = 0.f;
	// Bottom right.
	vertices[12].position = vec3New(1.f/6.f, -1.f/6.f, -1.f);
	vertices[12].u = areas[3].x; vertices[12].v = areas[3].y;
	vertices[12].normal = vec3New(0.f, 1.f, 0.f);
	vertices[12].bIDs[0] = 3; vertices[12].bIDs[1] = -1; vertices[12].bIDs[2] = -1; vertices[12].bIDs[3] = -1;
	vertices[12].bWeights[0] = 1.f; vertices[12].bWeights[1] = 0.f; vertices[12].bWeights[2] = 0.f; vertices[12].bWeights[3] = 0.f;
	vertices[13].position = vec3New(0.5f, -1.f/6.f, -1.f);
	vertices[13].u = areas[3].x + areas[3].w; vertices[13].v = areas[3].y;
	vertices[13].normal = vec3New(0.f, 1.f, 0.f);
	vertices[13].bIDs[0] = 3; vertices[13].bIDs[1] = -1; vertices[13].bIDs[2] = -1; vertices[13].bIDs[3] = -1;
	vertices[13].bWeights[0] = 1.f; vertices[13].bWeights[1] = 0.f; vertices[13].bWeights[2] = 0.f; vertices[13].bWeights[3] = 0.f;
	vertices[14].position = vec3New(1.f/6.f, -0.5f, -1.f);
	vertices[14].u = areas[3].x; vertices[14].v = areas[3].y + areas[3].h;
	vertices[14].normal = vec3New(0.f, 1.f, 0.f);
	vertices[14].bIDs[0] = 3; vertices[14].bIDs[1] = -1; vertices[14].bIDs[2] = -1; vertices[14].bIDs[3] = -1;
	vertices[14].bWeights[0] = 1.f; vertices[14].bWeights[1] = 0.f; vertices[14].bWeights[2] = 0.f; vertices[14].bWeights[3] = 0.f;
	vertices[15].position = vec3New(0.5f, -0.5f, -1.f);
	vertices[15].u = areas[3].x + areas[3].w; vertices[15].v = areas[3].y + areas[3].h;
	vertices[15].normal = vec3New(0.f, 1.f, 0.f);
	vertices[15].bIDs[0] = 3; vertices[15].bIDs[1] = -1; vertices[15].bIDs[2] = -1; vertices[15].bIDs[3] = -1;
	vertices[15].bWeights[0] = 1.f; vertices[15].bWeights[1] = 0.f; vertices[15].bWeights[2] = 0.f; vertices[15].bWeights[3] = 0.f;

	// Top left.
	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;
	// Top right.
	indices[6] = 6;
	indices[7] = 5;
	indices[8] = 4;
	indices[9] = 6;
	indices[10] = 7;
	indices[11] = 5;
	// Bottom left.
	indices[12] = 10;
	indices[13] = 9;
	indices[14] = 8;
	indices[15] = 10;
	indices[16] = 11;
	indices[17] = 9;
	// Bottom right.
	indices[18] = 14;
	indices[19] = 13;
	indices[20] = 12;
	indices[21] = 14;
	indices[22] = 15;
	indices[23] = 13;
	// Center.
	indices[24] = 9;
	indices[25] = 6;
	indices[26] = 3;
	indices[27] = 9;
	indices[28] = 12;
	indices[29] = 6;
	// Up.
	indices[30] = 3;
	indices[31] = 4;
	indices[32] = 1;
	indices[33] = 3;
	indices[34] = 6;
	indices[35] = 4;
	// Left.
	indices[36] = 8;
	indices[37] = 3;
	indices[38] = 2;
	indices[39] = 8;
	indices[40] = 9;
	indices[41] = 3;
	// Right.
	indices[42] = 12;
	indices[43] = 7;
	indices[44] = 6;
	indices[45] = 12;
	indices[46] = 13;
	indices[47] = 7;
	// Bottom.
	indices[48] = 11;
	indices[49] = 12;
	indices[50] = 9;
	indices[51] = 11;
	indices[52] = 14;
	indices[53] = 12;

	if(mdlGenerateBuffers(16, vertices, 54, indices, &mdlPanelDefault.vaoID, &mdlPanelDefault.vboID, &mdlPanelDefault.iboID, NULL) <= 0){
		return 0;
	}

	mdlPanelDefault.vertexNum = 16;
	mdlPanelDefault.indexNum = 54;

	element->data.panel.rndr.mdl = &mdlPanelDefault;

	return 1;

}
**/

void guiElementTick(guiElement *const restrict element, const float elapsedTime){
	if(flagsAreSet(element->flags, GUI_ELEMENT_TYPE_WINDOW)){
		twiTick(&element->data.window.body, elapsedTime);
		twiTick(&element->data.window.border, elapsedTime);
	}
}

guiElement *guiElementAddChild(guiElement *const restrict element){
	//
}

static void guiElementRenderWindow(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){

	/**
	// Get texture information for rendering and feed it to the shader.
	const twFrame *const restrict frame = twiState(&element->data.panel.rndr.twi, interpT);
	const float width = frame->image->width * frame->subframe.w;
	const float height = frame->image->height * frame->subframe.h;
	// Generate bone states for the panel corners.
	const bone tl = {
		.position = {.x = element->data.panel.configuration[0].x, .y = element->data.panel.configuration[0].y, .z = 0.f},
		.orientation = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f},
		.scale = {.x = width, .y = height, .z = 1.f}
	};
	const bone tr = {
		.position = {.x = element->data.panel.configuration[1].x, .y = element->data.panel.configuration[1].y, .z = 0.f},
		.orientation = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f},
		.scale = {.x = width, .y = height, .z = 1.f}
	};
	const bone bl = {
		.position = {.x = element->data.panel.configuration[2].x, .y = element->data.panel.configuration[2].y, .z = 0.f},
		.orientation = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f},
		.scale = {.x = width, .y = height, .z = 1.f}
	};
	const bone br = {
		.position = {.x = element->data.panel.configuration[3].x, .y = element->data.panel.configuration[3].y, .z = 0.f},
		.orientation = {.w = 1.f, .v.x = 0.f, .v.y = 0.f, .v.z = 0.f},
		.scale = {.x = width, .y = height, .z = 1.f}
	};
	const mat4 transform[4] = {
		boneMatrix(boneTransformAppend(element->root, tl)),
		boneMatrix(boneTransformAppend(element->root, tr)),
		boneMatrix(boneTransformAppend(element->root, bl)),
		boneMatrix(boneTransformAppend(element->root, br))
	};

	// Bind the texture (if needed).
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frame->image->diffuseID);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&frame->subframe);

	// Feed the translucency multiplier to the shader
	glUniform1f(gfxMngr->alphaID, rndrAlpha(&element->data.panel.rndr, interpT));

	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform[0].m[0][0]);
	glUniformMatrix4fv(gfxMngr->boneArrayID[1], 1, GL_FALSE, &transform[1].m[0][0]);
	glUniformMatrix4fv(gfxMngr->boneArrayID[2], 1, GL_FALSE, &transform[2].m[0][0]);
	glUniformMatrix4fv(gfxMngr->boneArrayID[3], 1, GL_FALSE, &transform[3].m[0][0]);

	// Render the model.
	glBindVertexArray(element->data.panel.rndr.mdl->vaoID);
	glDrawElements(GL_TRIANGLES, element->data.panel.rndr.mdl->indexNum, GL_UNSIGNED_INT, 0);
	**/

	const twFrame *const restrict frameBody = twiState(&element->data.window.body, interpT);
	const twFrame *const restrict frameBorder = twiState(&element->data.window.border, interpT);

	// Generate bone states for the window corners.
	const rectangle *offsets = &element->data.window.offsets[0];
	const bone root = {
		.position = {
			// Move the origin to the top left corner.
			.x = element->root.position.x + element->root.scale.x*0.5f + offsets->w * frameBorder->image->width,
			.y = element->root.position.y - element->root.scale.y*0.5f - offsets->h * frameBorder->image->height,
			.z = element->root.position.z
		},
		.orientation = element->root.orientation,
		.scale = element->root.scale
	};

	const float inverseWidth = 1.f/frameBorder->image->width;
	const float inverseHeight = 1.f/frameBody->image->height;

	vec2 size;
	rectangle subframe;
	mat4 transform;

	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBorder->image->diffuseID);
	// Feed the translucency multiplier to the shader
	glUniform1f(gfxMngr->alphaID, 1.f);

	// Draw the bottom right corner.
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = frameBorder->subframe.w * offsets->w;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix.
	transform.m[0][0] = size.x; transform.m[1][0] = 0.f;    transform.m[2][0] = 0.f; transform.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	transform.m[0][1] = 0.f;    transform.m[1][1] = size.y; transform.m[2][1] = 0.f; transform.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.y);
	transform.m[0][2] = 0.f;    transform.m[1][2] = 0.f;    transform.m[2][2] = 1.f; transform.m[3][2] = root.position.z;
	transform.m[0][3] = 0.f;    transform.m[1][3] = 0.f;    transform.m[2][3] = 0.f; transform.m[3][3] = 1.f;
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the top right corner.
	++offsets;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = frameBorder->subframe.w * offsets->w;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = size.x;
	transform.m[1][1] = size.y;
	transform.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	transform.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.y);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the top left corner.
	++offsets;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = frameBorder->subframe.w * offsets->w;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = size.x;
	transform.m[1][1] = size.y;
	transform.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	transform.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.y);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the bottom left corner.
	++offsets;
	// Border corner size, in pixels.
	size.x = offsets->w * frameBorder->image->width;
	size.y = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = frameBorder->subframe.w * offsets->w;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = size.x;
	transform.m[1][1] = size.y;
	transform.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	transform.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.y);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the right side.
	++offsets;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*root.scale.y*inverseWidth;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = 0.f;
	transform.m[0][1] = -root.scale.y;
	transform.m[1][0] = size.x;
	transform.m[1][1] = 0.f;
	transform.m[3][0] = root.position.x + 0.5f * (root.scale.x + size.x);
	transform.m[3][1] = root.position.y;
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the top side.
	++offsets;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = frameBorder->subframe.x + offsets->x;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : -2.f*root.scale.x*inverseWidth;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = root.scale.x;
	transform.m[0][1] = 0.f;
	transform.m[1][0] = 0.f;
	transform.m[1][1] = size.x;
	transform.m[3][0] = root.position.x;
	transform.m[3][1] = root.position.y + 0.5f * (root.scale.y + size.x);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the left side.
	++offsets;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*root.scale.y*inverseWidth;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*root.scale.y*inverseWidth;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = 0.f;
	transform.m[0][1] = root.scale.y;
	transform.m[1][0] = -size.x;
	transform.m[1][1] = 0.f;
	transform.m[3][0] = root.position.x - 0.5f * (root.scale.x + size.x);
	transform.m[3][1] = root.position.y;
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the bottom side.
	++offsets;
	// Border side thickness, in pixels.
	size.x = offsets->h * frameBorder->image->height;
	// UV subframe.
	subframe.x = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.x + offsets->x : frameBorder->subframe.x + offsets->x + 2.f*root.scale.x*inverseWidth;
	subframe.y = frameBorder->subframe.y + offsets->y;
	subframe.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBorder->subframe.w * offsets->w : 2.f*root.scale.x*inverseWidth;
	subframe.h = frameBorder->subframe.h * offsets->h;
	// Transformation matrix. We need only overwrite what's different from the previous setup.
	transform.m[0][0] = -root.scale.x;
	transform.m[0][1] = 0.f;
	transform.m[1][0] = 0.f;
	transform.m[1][1] = -size.x;
	transform.m[3][0] = root.position.x;
	transform.m[3][1] = root.position.y - 0.5f * (root.scale.y + size.x);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

	// Draw the body.
	// UV subframe.
	subframe.x = frameBody->subframe.x;
	subframe.y = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.y : frameBody->subframe.y + root.scale.y*inverseHeight;
	subframe.w = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.w : root.scale.x/frameBody->image->width;
	subframe.h = flagsAreSet(element->data.window.flags, GUI_WINDOW_STRETCH_BODY) ? frameBody->subframe.h : root.scale.y*inverseHeight;
	// Transformation matrix.
	transform = boneMatrix(root);
	// Bind the texture.
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frameBody->image->diffuseID);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->textureFragmentID[0], 1, (const GLfloat *)&subframe);
	// Feed the bone configuration to the shader.
	glUniformMatrix4fv(gfxMngr->boneArrayID[0], 1, GL_FALSE, &transform.m[0][0]);
	// Render the model.
	glBindVertexArray(mdlSprite.vaoID);
	glDrawElements(GL_TRIANGLES, mdlSprite.indexNum, GL_UNSIGNED_INT, 0);

}

/** The lines below should eventually be removed. **/
#define guiElementRenderRenderable NULL
#define guiElementRenderText       NULL
#define guiElementRenderObject     NULL

void (* const guiElementRenderJumpTable[4])(
	const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT
) = {
	guiElementRenderRenderable,
	guiElementRenderWindow,
	guiElementRenderText,
	guiElementRenderObject
};
__FORCE_INLINE__ void guiElementRender(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){
	guiElementRenderJumpTable[element->flags & GUI_ELEMENT_TYPE_MASK](element, gfxMngr, cam, distance, interpT);
}

void guiElementDelete(guiElement *element){
	//
}