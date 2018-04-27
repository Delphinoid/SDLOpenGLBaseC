#include "hitbox.h"

void hbArrayDelete(hbArray *hba){
	if(hba->hitboxes != NULL){
		free(hba->hitboxes);
	}
}
