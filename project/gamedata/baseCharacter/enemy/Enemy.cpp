#include "Enemy.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Enemy::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();
}

void Enemy::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	if (isHitPlayer == true) {
		hitPlayerTimer_++;
	}

	if (hitPlayerTimer_ >= 10) {
		hitPlayerTimer_ = 0;
		isHitPlayer = false;
	}

	if (isHitEnemy == true) {
		hitEnemyTimer_++;
	}

	if (hitEnemyTimer_ >= 15) {
		hitEnemyTimer_ = 0;
		isHitEnemy = false;
	}

	if (worldTransform_.translation_.num[1] < -10.0f) {
		isDead_ = true;
	}
	if (!isHitOnFloor || worldTransform_.GetWorldPos().num[1] < 0.0f) {
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

void Enemy::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

void Enemy::Move() {
	const float kGravityAcceleration = 0.01f;
	if (isGravityAccelerationCalc == true) {
		if (velocity_.num[0] >= 0.01f) {
			velocity_.num[0] -= kGravityAcceleration;
		}
		else if (velocity_.num[0] <= -0.01f) {
			velocity_.num[0] += kGravityAcceleration;
		}
		else {
			velocity_.num[0] = 0.0f;
		}

		if (velocity_.num[2] >= 0.01f) {
			velocity_.num[2] -= kGravityAcceleration;
		}
		else if (velocity_.num[2] <= -0.01f) {
			velocity_.num[2] += kGravityAcceleration;
		}
		else {
			velocity_.num[2] = 0.0f;
		}
	}
}

void Enemy::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Enemy::SetWorldTransform(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	velocity_.num[1] = 0.001f;
	isDead_ = false;
}

void Enemy::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Enemy::SetVelocity(const Vector3 velocity) {
	if (isHitPlayer == false) {
		velocity_ = velocity;
		isHitPlayer = true;
		isGravityAccelerationCalc = true;
	}
	if (isHitEnemy == false) {
		velocity_ = velocity;
		isHitEnemy = true;
		isGravityAccelerationCalc = true;
	}
}