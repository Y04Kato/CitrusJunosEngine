/**
 * @file Field.h
 * @brief ステージ全般の機能を管理
 * @author KATO
 * @date 2025/04/18
 */

#include "Field.h"

void Field::Initialize(Model* model) {
	model_ = model;
	worldTransform_.Initialize();
}

void Field::Update() {
	worldTransform_.UpdateMatrix();

}

void Field::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}