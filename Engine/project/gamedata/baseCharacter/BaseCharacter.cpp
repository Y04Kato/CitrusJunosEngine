/**
 * @file BaseCharacter.cpp
 * @brief 自機や敵などの基本的なキャラクターの継承元
 * @author KATO
 * @date 未記録
 */

#include "BaseCharacter.h"

void BaseCharacter::Initialize(Model* model) {
	model_ = model;
	worldTransform_.Initialize();
}

void BaseCharacter::Update() { worldTransform_.UpdateMatrix(); }

void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}