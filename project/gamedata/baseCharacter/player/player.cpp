#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Player::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

void Player::Move() {
	if (input_->TriggerKey(DIK_W)) {
		worldTransform_.translation_.num[1] -= moveSpeed_;
	}
	if (input_->TriggerKey(DIK_S)) {
		worldTransform_.translation_.num[1] += moveSpeed_;
	}
	if (input_->TriggerKey(DIK_A)) {
		worldTransform_.translation_.num[1] -= moveSpeed_;
	}
	if (input_->TriggerKey(DIK_D)) {
		worldTransform_.translation_.num[1] += moveSpeed_;
	}
}