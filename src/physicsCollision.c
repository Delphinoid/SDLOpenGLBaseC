#include "physicsCollision.h"
#include "physicsRigidBody.h"

void physContactSolve(physContact *contact){

}

return_t physCheckCollision(physRigidBody *const restrict bodyA, physRigidBody *const restrict bodyB,
                            physSeparation *const restrict sc, physContact *const restrict cm){
	return cCheckCollision(&bodyA->hull, &bodyB->hull, &sc->separation, &cm->manifold);
}