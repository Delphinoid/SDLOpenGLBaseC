#include "graphicsManager.h"
#include "renderable.h"
#include "texture.h"
#include "model.h"
#include "skeleton.h"
#include "inline.h"

void rndrBaseInit(renderableBase *const restrict rndr){
	rndr->mdl = NULL;
	rndr->tw = NULL;
}

void rndrInit(renderable *const restrict rndr){
	rndr->mdl = NULL;
	twiInit(&rndr->twi, NULL);
	rndrStateInit(&rndr->state);
	billboardInit(&rndr->billboardData);
}

void rndrInstantiate(renderable *const restrict rndr, const renderableBase *const base){
	rndr->mdl = base->mdl;
	twiInit(&rndr->twi, base->tw);
	rndrStateInit(&rndr->state);
	billboardInit(&rndr->billboardData);
}

__FORCE_INLINE__ void rndrTick(renderable *const restrict rndr, const float elapsedTime){
	twiTick(&rndr->twi, elapsedTime);
	rndr->state.alphaPrevious = rndr->state.alphaCurrent;
	rndr->state.alphaCurrent = rndr->state.alpha;
}

void rndrRender(const renderable *const restrict rndr, const skeleton *const restrict skl, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const vec3 centroid, const float interpT){

	// Get texture information for rendering and feed it to the shader.
	const twFrame *const restrict frame = twiState(&rndr->twi, interpT);
	// Bind the texture (if needed).
	gfxMngrBindTexture(gfxMngr, GL_TEXTURE0, frame->image->diffuseID);
	// Feed the texture coordinates to the shader.
	glUniform4fv(gfxMngr->shdrPrgObj.textureFragmentID[0], 1, (const GLfloat *)&frame->subframe);

	if(rndr->mdl->skl != NULL){

		float alpha = rndrStateAlpha(&rndr->state, interpT);

		if(alpha > 0.f){

			vertexIndexNum_t indexNum;
			const void *offset;

			mat4 transform;
			GLuint *bArray = gfxMngr->shdrPrgObj.boneArrayID;

			boneIndex_t boneNum = rndr->mdl->skl->boneNum;
			sklNode *nLayout = rndr->mdl->skl->bones;

			// If there is a valid animated skeleton, apply animation transformations.
			boneIndex_t i;
			for(i = 0; i < boneNum; ++i, ++bArray, ++nLayout){

				/** Use a lookup, same in object.c. **/
				boneIndex_t rndrBone = sklFindBone(skl, i, nLayout->name);

				if(rndrBone >= skl->boneNum){
					// Use the root bone's transformation if
					// the animated bone is not in the model.
					rndrBone = 0;
				}

				// Apply billboarding transformation if required.
				if(rndr->billboardData.flags != BILLBOARD_DISABLED){
					// Use the root bone's global position as the centroid for billboarding.
					transform = billboardState(rndr->billboardData, cam, centroid, gfxMngr->shdrPrgObj.skeletonTransformState[rndrBone]);
				}else{
					transform = gfxMngr->shdrPrgObj.skeletonTransformState[rndrBone];
				}

				// Feed the bone configuration to the shader.
				glUniformMatrix4fv(*bArray, 1, GL_FALSE, &transform.m[0][0]);

			}

			// Feed the translucency multiplier to the shader.
			if(flagsAreSet(rndr->state.flags, RENDERABLE_STATE_ALPHA_DITHER)){
				// Negative alpha values indicate dithering.
				alpha = -alpha;
			}
			glUniform1f(gfxMngr->shdrPrgObj.alphaID, alpha);

			// Render the model.
			glBindVertexArray(rndr->mdl->buffers.vaoID);
			/**if(rndr->mdl->buffers.indexNum > 0){**/
				mdlFindCurrentLOD(rndr->mdl, &indexNum, &offset, distance, gfxMngr->shdrShared.biasLOD);
				if(indexNum){
					glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, offset);
				}
			/**}else{
				glDrawArrays(GL_TRIANGLES, 0, rndr->mdl->buffers.vertexNum);
			}**/

		}

	}

}