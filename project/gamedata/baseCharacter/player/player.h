#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Player : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void UpdateView();

	void Draw(const ViewProjection& viewProjection) override;

	void Move();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	void SetWorldTransform(const Vector3 translation);

	void IsFallStart();

	void SetScale(const Vector3 scale) { worldTransform_.scale_ = scale; }

	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHitOnFloor;

	bool isGameover() { return gameOver; }

	StructSphere GetStructSphere() { return structSphere_; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	Vector3 GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3 velocity);

	int GetMoveMode() { return moveMode_; }

	bool GetIsHitEnemy() { return isHitEnemy_; }

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.5f;

	Vector3 velocity_ = {};
	Vector3 velocityC_ = {};

	WorldTransform objectPos_;

	bool gameOver = false;

	StructSphere structSphere_;

	const ViewProjection* viewProjection_ = nullptr;

	//0~2で弱~強
	int moveMode_ = 0;

	int hitTimer_ = 0;
	bool isHitEnemy_ = false;
};