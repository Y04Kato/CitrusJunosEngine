#pragma once
#include "Collider.h"
#include <list>

class CollisionManager {
public:
	void AddCollider(Collider* collider) { colliders_.push_back(collider); }
	void ClearColliders() { colliders_.clear(); }
	void CheckAllCollision();
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	static CollisionManager* GetInstance();

private:
	std::list<Collider*> colliders_;
};
