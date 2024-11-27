/**
 * @file Enemy.cpp
 * @brief プレイヤーの邪魔となる敵を管理
 * @author KATO
 * @date 未記録
 */

#include "Enemy.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Enemy::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	structSphere_.radius = sphereSize_;
}

void Enemy::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	structSphere_.center = worldTransform_.GetWorldPos();

	if (worldTransform_.translation_.num[1] < -10.0f) {//Yが-10以上で
		isDead_ = true;
	}
	if (!isHitOnFloor_ || worldTransform_.GetWorldPos().num[1] < 0.0f) {//地面と当たっていなければ
		IsFallStart();
	}
	else {
		worldTransform_.translation_.num[1] = objectPos_.translation_.num[1] + objectPos_.scale_.num[1] + worldTransform_.scale_.num[1] - 0.6f;
		velocity_.num[1] = 0.0f;
	}

	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_.num[1] += rotateViewSpeed_;

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

void Enemy::Move() {
	//加速度減衰処理
	if (isGravityAccelerationCalc == true) {
		if (velocity_.num[0] >= 0.01f) {
			velocity_.num[0] -= movingAttenuation_;
		}
		else if (velocity_.num[0] <= -0.01f) {
			velocity_.num[0] += movingAttenuation_;
		}
		else {
			velocity_.num[0] = 0.0f;
		}

		if (velocity_.num[2] >= 0.01f) {
			velocity_.num[2] -= movingAttenuation_;
		}
		else if (velocity_.num[2] <= -0.01f) {
			velocity_.num[2] += movingAttenuation_;
		}
		else {
			velocity_.num[2] = 0.0f;
		}
	}
}

void Enemy::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	Vector3 accelerationVector = { 0.0f,-gravityAcceleration_,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Enemy::SetWorldTransform(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
	velocity_.num[1] = 0.0f;
	isDead_ = false;
}

void Enemy::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Enemy::SetVelocity(const Vector3 velocity) {
	velocity_ = velocity;
	isGravityAccelerationCalc = true;
}