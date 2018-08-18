#include "hitbox.h"

void hbInit(hitbox *hb, const hitboxType_t type){
	hb->type = type;
}
void hbDelete(hitbox *hb){
	if(hb->type == 0){
		hbMeshDelete((hbMesh *)&hb->hull);
	}
}

void hbArrayDelete(hbArray *hba){
	if(hba->hitboxes != NULL){
		free(hba->hitboxes);
	}
}
