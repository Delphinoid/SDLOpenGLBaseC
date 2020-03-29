#include "gui.h"
#include "moduleGUI.h"

void guiInit(guiElement *const __RESTRICT__ element, const flags_t type){
	element->children = NULL;
	element->type = type;
	boneInit(&element->root);
}

guiElement *guiNewChild(guiElement *const element){
	guiElement *const child = moduleGUIElementAppend(&element->children);
	child->parent = element;
	return child;
}

static void guiTickController(guiElement *const element, const float elapsedTime){

	// Controllers generally have nothing to render.
	// Instead, they handle the rendering of other elements.
	guiElement *current = element->children;
	guiElement *next;

	if(current != NULL){
		for(;;){

			// Find the "leftmost" child.
			while(current->children != NULL){
				current = current->children;
			}

			guiTick(current, elapsedTime);

			// Get the next child. Go to the parent if necessary.
			while((next = (guiElement *)memSLinkDataGetNext(current)) == NULL){
				if(current->parent == element){
					return;
				}
				current = current->parent;
				guiTick(current, elapsedTime);
			}
			current = next;

		}
	}

}

/** The lines below should eventually be removed. **/
#define guiTickRenderable NULL
#define guiTickObject     NULL

void (* const guiTickJumpTable[4])(
	guiElement *const element, const float elapsedTime
) = {
	guiTickController,
	guiTickText,
	guiTickWindow,
	guiTickRenderable,
	guiTickObject
};

__FORCE_INLINE__ void guiTick(guiElement *const element, const float elapsedTime){
	guiTickJumpTable[element->type](element, elapsedTime);
}

static void guiRenderController(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){

	// Controllers generally have nothing to render.
	// Instead, they handle the rendering of other elements.
	const guiElement *current = element->children;
	const guiElement *next;

	if(current != NULL){
		for(;;){

			// Find the "leftmost" child.
			while(current->children != NULL){
				current = current->children;
			}

			guiRender(current, gfxMngr, cam, distance, interpT);

			// Get the next child. Go to the parent if necessary.
			while((next = (guiElement *)memSLinkDataGetNext(current)) == NULL){
				if(current->parent == element){
					return;
				}
				current = current->parent;
				guiRender(current, gfxMngr, cam, distance, interpT);
			}
			current = next;

		}
	}

}

/** The lines below should eventually be removed. **/
#define guiRenderRenderable NULL
#define guiRenderObject     NULL

void (* const guiRenderJumpTable[4])(
	const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT
) = {
	guiRenderController,
	guiRenderText,
	guiRenderWindow,
	guiRenderRenderable,
	guiRenderObject
};
__FORCE_INLINE__ void guiRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){
	guiRenderJumpTable[element->type](element, gfxMngr, cam, distance, interpT);
}

void guiDelete(guiElement *element){
	//
}