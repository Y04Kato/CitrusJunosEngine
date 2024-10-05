/**
 * @file CollisionManager.h
 * @brief Colliderに登録された全てを管理する
 * @author KATO
 * @date 未記録
 */

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
