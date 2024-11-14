/**
 * @file Player.cpp
 * @brief プレイヤーが操作を行う自機を管理する
 * @author KATO
 * @date 未記録
 */

#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Player::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	worldTransform_.translation_ = { 0.0f,0.1f,0.0f };
	worldTransform2_ = worldTransform_;

	structSphere_.radius = 1.5f;
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	worldTransform2_.TransferMatrix();

	//移動フラグがTrueなら移動操作を行えるように
	if (isMove_ == true) {
		Move();
	}

	//移動減衰
	MoveAttenuation();

	structSphere_.center = worldTransform_.GetWorldPos();

	if (moveFlag_ == false) {
		moveCount_++;
	}
	if (moveCount_ >= moveCountMax_) {
		moveFlag_ = true;
		moveCount_ = 0;
	}

	if (worldTransform_.translation_.num[1] < -10.0f) {
		gameOver = true;
		moveMode_ = 0;
		worldTransform2_.rotation_ = { 0.0f,0.0f,0.0f };
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

	worldTransform2_.translation_ = worldTransform_.translation_;

	worldTransform_.UpdateMatrix();
	worldTransform2_.UpdateMatrix();

	Vector3 test = viewProjection_->translation_;

	ImGui::Begin("player");
	ImGui::Text("Move WASD");
	ImGui::Text("MovePowerChange Space");
	ImGui::DragFloat3("vector", velocity_.num);
	ImGui::DragFloat3("vectorC", velocityC_.num);
	ImGui::DragFloat3("vectorC", test.num);
	ImGui::End();
}

void Player::UpdateView() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	worldTransform_.translation_ += velocity_;
	if (worldTransform_.rotation_.num[0] == 0.0f && worldTransform_.rotation_.num[2] == 0.0f) {
		worldTransform_.rotation_.num[1] += 1.0f;
	}

	worldTransform_.UpdateMatrix();

	ImGui::Begin("player");
	ImGui::DragFloat3("rotate", &worldTransform_.rotation_.num[0]);
	ImGui::End();
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

Vector3 Player::CalculateForwardVector() {
	//Y軸の回転（worldTransform2_.rotation_.num[1]）を用いて前方ベクトルを計算
	float radians = worldTransform2_.rotation_.num[1];
	return Vector3{ std::sin(radians), 0.0f, std::cos(radians) };
}

void Player::Move() {
	//左右の回転処理
	if (input_->PressKey(DIK_A)) {
		worldTransform2_.rotation_.num[1] -= 0.1f;//左回転
		rotate_.num[0] -= 0.1f;
	}
	if (input_->PressKey(DIK_D)) {
		worldTransform2_.rotation_.num[1] += 0.1f;//右回転
		rotate_.num[0] += 0.1f;
	}

	rotate_.num[1] = worldTransform2_.rotation_.num[1];

	//移動速度をモードに応じて設定
	switch (moveMode_) {
	case 0: CharacterSpeed_ = 0.5f; break;
	case 1: CharacterSpeed_ = 0.7f; break;
	case 2: CharacterSpeed_ = 0.9f; break;
	}

	if (moveFlag_) {
		if (input_->TriggerKey(DIK_SPACE)) {
			moveFlag_ = false;
			Vector3 forward = CalculateForwardVector();//前方ベクトルを計算
			velocity_ = forward * CharacterSpeed_;//前方ベクトルに速度を掛けて移動方向を決定
		}
	}

	//加速度モードの変更
	if (input_->TriggerKey(DIK_W)) {
		moveMode_ = (moveMode_ + 1) % 3;
	}
	if (input_->TriggerKey(DIK_S)) {
		moveMode_ = (moveMode_ == 0) ? 2 : moveMode_ - 1;
	}

	//ジョイスティックによる回転処理
	XINPUT_STATE joystate;
	if (moveFlag_ && Input::GetInstance()->GetJoystickState(0, joystate)) {
		if (input_->TriggerAButton(joystate)) {
			moveFlag_ = false;

			float rotationSpeed = 0.1f;
			if (joystate.Gamepad.sThumbLX > 5000) {//右方向入力
				worldTransform2_.rotation_.num[1] += rotationSpeed;
				rotate_.num[0] += rotationSpeed;
			}
			else if (joystate.Gamepad.sThumbLX < -5000) {//左方向入力
				worldTransform2_.rotation_.num[1] -= rotationSpeed;
				rotate_.num[0] -= rotationSpeed;
			}
		}

		if (input_->TriggerXButton(joystate)) {
			moveFlag_ = false;
			moveMode_ = (moveMode_ + 1) % 3;
		}
	}
}

void Player::MoveAttenuation() {
	//加速度減衰処理
	const float kGravityAcceleration = 0.01f;

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

void Player::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Player::SetWorldTransform(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
	velocity_.num[1] = 0.001f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Player::SetVelocity(const Vector3 velocity) {
	velocity_ = velocity;
}