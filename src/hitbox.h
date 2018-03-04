#ifndef HITBOX_H
#define HITBOX_H

// The size of the largest collider type.
// In this case, it tied between the mesh and capsule at 28 bytes.
#define HB_MAX_COLLIDER_SIZE 28

typedef struct {
	unsigned char type;  // 0 = mesh, 1 = capsule, 2 = sphere, 3 = AABB
	char hull[HB_MAX_COLLIDER_SIZE];  // Stores a collider of the type specified by "type". Needs to be casted.
	                                  // May be affected by translation, rotation and scaling, depending on the type.
} hitbox;

typedef struct {
	hitbox **hitboxes;
} hbArray;

#endif
