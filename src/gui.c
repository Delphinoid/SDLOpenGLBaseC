#include "gui.h"
#include "inline.h"

void guiElementTick(guiElement *const restrict element, const float elapsedTime){
	if(flagsAreSet(element->flags, GUI_ELEMENT_TYPE_WINDOW)){
		twiTick(&element->data.window.body, elapsedTime);
		twiTick(&element->data.window.border, elapsedTime);
	}
}

guiElement *guiElementAddChild(guiElement *const restrict element){
	//
}

/** The lines below should eventually be removed. **/
#define guiElementRenderRenderable NULL
#define guiElementRenderObject     NULL

void (* const guiElementRenderJumpTable[4])(
	const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT
) = {
	guiElementRenderText,
	guiElementRenderWindow,
	guiElementRenderRenderable,
	guiElementRenderObject
};
__FORCE_INLINE__ void guiElementRender(const guiElement *const restrict element, graphicsManager *const restrict gfxMngr, const camera *const restrict cam, const float distance, const float interpT){
	guiElementRenderJumpTable[element->flags & GUI_ELEMENT_TYPE_MASK](element, gfxMngr, cam, distance, interpT);
}

void guiElementDelete(guiElement *element){
	//
}