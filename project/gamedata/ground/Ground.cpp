#include "ground.h"

void Ground::Initialize(Model* model, Vector3 translation, Vector3 scale) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = translation;
	worldTransform_.scale_ = scale;

	groundModel_ = model;
}

void Ground::Update() {
	worldTransform_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection& viewProjection) {
	groundModel_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}
