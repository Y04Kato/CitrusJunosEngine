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

	bool isDead = false;
	OBB GetOBB() { return Obb_; }
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

	OBB Obb_;
};
