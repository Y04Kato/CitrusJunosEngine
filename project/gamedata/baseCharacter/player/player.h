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

	void Draw(const ViewProjection& viewProjection) override;

	void Move();

	WorldTransform GetWorldTransform()override { return worldTransform_; }

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.5f;
};