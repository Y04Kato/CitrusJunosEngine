#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Enemy : public BaseCharacter, public Collider {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	void Move();

	WorldTransform GetWorldTransform()override { return worldTransform_; }

	void OnCollision() override;

	void IsFallStart();

	void SetWorldTransform(const Vector3 translation);

	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHit_;

	bool isHit = false;

	bool isCollision_;

	bool isDead() { return dead_; }

	void SetisDead();

	StructSphere GetStructSphere() { return structSphere_; }

	void SetVelocity(const Vector3 velocity);

	Vector3 GetVelocity() { return velocity_; }

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.5f;

	Vector3 velocity_ = {};

	WorldTransform objectPos_;

	bool dead_ = false;

	StructSphere structSphere_;

	//WSAD順
	bool isMove_[4] = { false,false,false,false };

	Quaternion quaternion_;
	Vector3 preMove_;
	Quaternion preQuaternion_;

	const ViewProjection* viewProjection_ = nullptr;

	int hitCount = 0;

	int moveCount = 0;

	bool test = false;
};
