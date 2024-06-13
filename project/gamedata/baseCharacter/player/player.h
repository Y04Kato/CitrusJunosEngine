#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Player : public BaseCharacter, public Collider {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void UpdateView();

	void Draw(const ViewProjection& viewProjection) override;

	void Move();

	WorldTransform GetWorldTransform()override { return worldTransform_; }

	void OnCollision() override;

	void IsFallStart();

	void SetWorldTransform(const Vector3 translation);
	void SetScale(const Vector3 scale) { worldTransform_.scale_ = scale; }

	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHit_;

	bool isGameover() { return gameOver; }

	StructSphere GetStructSphere() { return structSphere_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	void SetVelocity(const Vector3 velocity);

	Vector3 GetVelocity() { return velocity_; }

	Quaternion GetRotateQuaternion() {return quaternion_;}

	int GetMoveMode() { return moveMode; }

	bool GetIsHit() { return isHit; }

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.5f;

	Vector3 velocity_ = {};
	Vector3 velocityC_ = {};
	Vector3 rotateVelocity_ = {};

	WorldTransform objectPos_;

	bool gameOver = false;

	StructSphere structSphere_;

	Quaternion quaternion_;
	Vector3 preMove_;
	Quaternion preQuaternion_;

	const ViewProjection* viewProjection_ = nullptr;

	//0~2で弱~強
	int moveMode = 0;

	int hitCount = 0;
	bool isHit = false;
};