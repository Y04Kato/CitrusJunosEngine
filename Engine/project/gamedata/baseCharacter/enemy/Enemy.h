/**
 * @file Enemy.h
 * @brief プレイヤーの邪魔となる敵を管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Enemy : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	void Move();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	void SetWorldTransform(const Vector3 translation);

	void IsFallStart();

	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHitOnFloor = false;

	bool GetisDead() { return isDead_; }
	void SetisDead() { isDead_ = true; }

	StructSphere GetStructSphere() { return structSphere_; }

	Vector3 GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3 velocity);

private:
	Input* input_ = nullptr;

	Vector3 velocity_ = {};

	WorldTransform objectPos_;

	bool isDead_ = false;

	StructSphere structSphere_;

	const ViewProjection* viewProjection_ = nullptr;

	bool isGravityAccelerationCalc = false;
};
