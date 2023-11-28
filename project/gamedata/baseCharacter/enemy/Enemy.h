#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"
#define _USE_MATH_DEFINES
#include<math.h>

class Enemy : public BaseCharacter, public Collider {
public:

	void Initialize(const std::vector<Model*>& models) override;

	void Update()override;

	void Draw(const ViewProjection& viewProjection)override;
	void OnCollision() override;
	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	WorldTransform GetWorldTransform()override { return worldTransformBase_; }

	void Move();
	void SetParent(const WorldTransform* parent);
	void ModelUpdateMatrix();
	void InitializeFloatGimmick();
	void UpdateFloatGimmick();

	void ApplyGlobalVariables();

	StructSphere GetStructSphere() { return structSphere_; }

	void SetPos(Vector3 pos) { pos_ = pos; };

	bool GetisAlive() { return isDead; }

	OBB GetOBB() { return Obb_; }

	void SetIsFly(bool isfly) { isFly = isfly; }

	void SetPlayerComboData(Vector3 velocity) { velocity_ = velocity; }
private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Input* input_ = nullptr;

	const ViewProjection* viewProjection_ = nullptr;
	float floatingParametor_ = 0.0f;
	int animationFrame;
	Vector3 move_;

	StructSphere structSphere_;

	int deadTimer = 0;
	int flyTimer = 0;

	OBB Obb_;

	bool isFly = false;
	bool isDead = false;

	Vector3 pos_;

	Vector3 velocity_;

	Vector4 RGBA;
};
