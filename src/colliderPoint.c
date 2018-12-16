#include "colliderPoint.h"

return_t cPointCollision(const cPoint *const restrict c1, const cPoint *const restrict c2){
	return c1->x == c2->x && c1->y == c2->y && c1->z == c2->z;
}