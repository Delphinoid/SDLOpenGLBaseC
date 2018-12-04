#include "colliderAABB.h"

return_t cAABBCollision(const cAABB *c1, const cAABB *c2){
	return ((c1->left  >= c2->left   && c1->left  <= c2->right) || (c1->right  >= c2->left   && c1->right  <= c2->right)) &&
	       ((c1->top   >= c2->bottom && c1->top   <= c2->top)   || (c1->bottom >= c2->bottom && c1->bottom <= c2->top))   &&
	       ((c1->front >= c2->back   && c1->front <= c2->front) || (c1->back   >= c2->back   && c1->back   <= c2->front));
}
