#include "CollisionManager.h"

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}


void CollisionManager::CheckAllCollision() {
	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {
		std::list<Collider*>::iterator itrB = itrA;
		++itrB;
		for (; itrB != colliders_.end(); ++itrB) {
			CheckCollisionPair(*(itrA), *(itrB));
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
		!(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return;
	}

	Vector3 posA = colliderA->GetWorldTransform().GetWorldPos();
	Vector3 posB = colliderB->GetWorldTransform().GetWorldPos();
	float radA = colliderA->Getradius();
	float radB = colliderB->Getradius();

	Vector3 Distance = {
		(posB.num[0] - posA.num[0])* (posB.num[0] - posA.num[0]), (posB.num[1] - posA.num[1])* (posB.num[1] - posA.num[1]),
		(posB.num[2] - posA.num[2]) * (posB.num[2] - posA.num[2]) };

	if (Distance.num[0] + Distance.num[1] + Distance.num[2] <= (radA + radB) * (radA + radB)) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}