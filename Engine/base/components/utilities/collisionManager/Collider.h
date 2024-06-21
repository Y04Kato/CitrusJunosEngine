#pragma once
#include "Vector.h"
#include "WorldTransform.h"
#include <cstdint>

class Collider {
private:
	float radius_ = 1.0f;
	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t CollisionMask_ = 0xffffffff;

public:
	virtual ~Collider() {}

	float Getradius() const { return radius_; }
	void SetRadius(float rad) { radius_ = rad; }

	virtual void OnCollision() = 0;

	virtual WorldTransform GetWorldTransform() = 0;

	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }

	uint32_t GetCollisionMask() const { return CollisionMask_; }
	void SetCollisionMask(uint32_t mask) { CollisionMask_ = mask; }
};
