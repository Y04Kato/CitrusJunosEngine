/**
 * @file Ground.cpp
 * @brief ゲームシーンでの床を管理
 * @author KATO
 * @date 未記録
 */

#include "ground.h"

void Ground::Initialize(Model* model, Vector3 translation, Vector3 scale) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = translation;
	worldTransform_.scale_ = scale;

	worldTransformRotate_ = worldTransform_;

	groundModel_ = model;
}

void Ground::Update() {
	worldTransformRotate_.rotation_.num[1] += 0.005f;

	worldTransform_.UpdateMatrix();
	worldTransformRotate_.UpdateMatrix();
}

void Ground::Draw(const ViewProjection& viewProjection) {
	groundModel_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}
