#include "gui.h"
#include "moduleGUI.h"

void guiInit(guiElement *const __RESTRICT__ element, const flags_t type){
	element->children = NULL;
	element->type = type;
	tfInit(&element->root);
}

guiElement *guiNewChild(guiElement *const element){
	guiElement *const child = moduleGUIElementAppend(&element->children);
	child->parent = element;
	return child;
}

/** Remove this and use a temporary recursive solution for guiTransforms, probably. **/
static void guiControllerTick(guiElement *const element, const float dt_ms){

	// Controllers generally have nothing to render.
	// Instead, they handle the rendering of other elements.
	guiElement *current = element;
	guiElement *next;

	for(;;){

		// Go down to the deepest child, rendering each
		// one on the way.  This is so that certain
		// operations such as masking are respected.
		while(current->children != NULL){
			current = current->children;
			guiTick(current, dt_ms);
		}

		// Get the next child. Go to the parent if necessary.
		while((next = (guiElement *)memSLinkDataGetNext(current)) == NULL){
			if(current->parent == element){
				// We're back where we started, at the controller.
				return;
			}
			current = current->parent;
		}
		current = next;

	}

}

/** The lines below should eventually be removed. **/
#define guiTransformTick  NULL
#define guiRenderableTick NULL
#define guiObjectTick     NULL

void (* const guiTickJumpTable[6])(
	guiElement *const element, const float dt_ms
) = {
	guiControllerTick,
	guiTextTick,
	guiWindowTick,
	guiTransformTick,
	guiRenderableTick,
	guiObjectTick
};

__FORCE_INLINE__ void guiTick(guiElement *const element, const float dt_ms){
	guiTickJumpTable[element->type](element, dt_ms);
}

/** Remove this and use a temporary recursive solution for guiTransforms, probably. **/
static void guiControllerRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){

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
#define guiTransformRender  NULL
#define guiRenderableRender NULL
#define guiObjectRender     NULL

void (* const guiRenderJumpTable[6])(
	const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT
) = {
	guiControllerRender,
	guiTextRender,
	guiWindowRender,
	guiTransformRender,
	guiRenderableRender,
	guiObjectRender
};
__FORCE_INLINE__ void guiRender(const guiElement *const element, graphicsManager *const gfxMngr, const camera *const cam, const float distance, const float interpT){
	guiRenderJumpTable[element->type](element, gfxMngr, cam, distance, interpT);
}

void guiDelete(guiElement *element){
	//
}