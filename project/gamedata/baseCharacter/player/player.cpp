#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Player::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	worldTransform_.translation_ = { 0.0f,0.1f,0.0f };

	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	if (worldTransform_.translation_.num[1] < -10.0f) {
		gameOver = true;
	}
	if (!isHit_ || worldTransform_.GetWorldPos().num[1] < 0.0f) {
		IsFallStart();
	}
	else {
		worldTransform_.translation_.num[1] = objectPos_.translation_.num[1] + objectPos_.scale_.num[1] + worldTransform_.scale_.num[1] - 0.6f;
		velocity_.num[1] = 0.001f;
	}

	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_.num[1] += 1.0f;

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });

	ImGui::Begin("player");
	ImGui::Text("Move WASD");
	ImGui::Text("MovePowerChange Space");
	ImGui::End();
}

void Player::Move() {
	if (input_->TriggerKey(DIK_W)) {
		if (moveMode == 0) {
			velocity_.num[2] = 0.5f;
		}
		if (moveMode == 1) {
			velocity_.num[2] = 0.7f;
		}
		if (moveMode == 2) {
			velocity_.num[2] = 0.9f;
		}
	}
	if (input_->TriggerKey(DIK_S)) {
		if (moveMode == 0) {
			velocity_.num[2] = -0.5f;
		}
		if (moveMode == 1) {
			velocity_.num[2] = -0.7f;
		}
		if (moveMode == 2) {
			velocity_.num[2] = -0.9f;
		}
	}
	if (input_->TriggerKey(DIK_A)) {
		if (moveMode == 0) {
			velocity_.num[0] = -0.5f;
		}
		if (moveMode == 1) {
			velocity_.num[0] = -0.7f;
		}
		if (moveMode == 2) {
			velocity_.num[0] = -0.9f;
		}
	}
	if (input_->TriggerKey(DIK_D)) {
		if (moveMode == 0) {
			velocity_.num[0] = 0.5f;
		}
		if (moveMode == 1) {
			velocity_.num[0] = 0.7f;
		}
		if (moveMode == 2) {
			velocity_.num[0] = 0.9f;
		}
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		moveMode++;
		if (moveMode >= 3) {
			moveMode = 0;
		}
	}

	const float kGravityAcceleration = 0.01f;

	if (velocity_.num[0] >= 0.01f) {
		;
		velocity_.num[0] -= kGravityAcceleration;
	}
	else if (velocity_.num[0] <= -0.01f) {
		velocity_.num[0] += kGravityAcceleration;
	}
	else {
		velocity_.num[0] = 0.0f;
	}

	if (velocity_.num[2] >= 0.01f) {
		;
		velocity_.num[2] -= kGravityAcceleration;
	}
	else if (velocity_.num[2] <= -0.01f) {
		velocity_.num[2] += kGravityAcceleration;
	}
	else {
		velocity_.num[2] = 0.0f;
	}
}

void Player::OnCollision() {
	velocity_ = -velocity_ * 1.0f;
}

void Player::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Player::SetWorldTransform(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	velocity_.num[1] = 0.001f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}