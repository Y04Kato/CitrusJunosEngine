#include "MovingGround.h"

void MovingGround::Initialize(Model* model, Vector3 translation, Vector3 scale) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = translation;
	worldTransform_.scale_ = scale;

	groundModel_ = model;
	move_ = { 0.0f,0.0f,0.2f };
}

void MovingGround::Update() {
	Move();
	if (worldTransform_.translation_.num[2] >= 37.0f || worldTransform_.translation_.num[2] <= 11.0f) {
		move_ = Multiply(-1.0f, move_);
	}

	worldTransform_.UpdateMatrix();
}

void MovingGround::Draw(const ViewProjection& viewProjection) {
	groundModel_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}


void MovingGround::Move() {
	worldTransform_.translation_ = Add(worldTransform_.translation_, move_);
}
