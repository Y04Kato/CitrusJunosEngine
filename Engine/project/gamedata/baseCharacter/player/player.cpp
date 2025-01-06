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
	worldTransformForCameraReference_ = worldTransform_;

	structSphere_.radius = sphereSize_;
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	worldTransformForCameraReference_.TransferMatrix();

	//移動フラグがTrueなら移動操作を行えるように
	if (isMove_ == true) {
		Move();
	}

	//移動減衰
	MoveAttenuation();

	//判定のSphere座標を同期
	structSphere_.center = worldTransform_.GetWorldPos();

	//行動クールタイム
	if (moveFlag_ == false) {
		moveCount_++;
	}
	if (moveCount_ >= moveCountMax_) {
		moveFlag_ = true;
		moveCount_ = 0;
	}

	//ゲームオーバー判定
	if (worldTransform_.translation_.num[1] < gameoverSite_) {
		gameOver = true;
		moveMode_ = 0;
		worldTransformForCameraReference_.rotation_ = { 0.0f,0.0f,0.0f };
	}

	//落下開始判定
	if (!isHitOnFloor_ || worldTransform_.GetWorldPos().num[1] < dropSite_) {
		IsFallStart();
	}
	else {
		worldTransform_.translation_.num[1] = objectPos_.translation_.num[1] + objectPos_.scale_.num[1] + worldTransform_.scale_.num[1] - 0.6f;
		velocity_.num[1] = 0.0f;
	}

	//加速を適用
	worldTransform_.translation_ += velocity_;
	//回転を適用
	worldTransform_.rotation_.num[1] += rotateViewSpeed_;

	worldTransformForCameraReference_.translation_ = worldTransform_.translation_;

	//更新
	worldTransform_.UpdateMatrix();
	worldTransformForCameraReference_.UpdateMatrix();

	//ImGui
	ImGui::Begin("player");
	ImGui::Text("Move WASD");
	ImGui::Text("MovePowerChange Space");
	ImGui::DragFloat3("vector", velocity_.num);
	ImGui::DragFloat3("vectorC", velocityC_.num);
	ImGui::End();
}

void Player::UpdateView() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//判定のSphere座標を同期
	structSphere_.center = worldTransform_.GetWorldPos();

	//加速を適用
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.rotation_.num[0] == 0.0f && worldTransform_.rotation_.num[2] == 0.0f) {
		worldTransform_.rotation_.num[1] += rotateViewSpeed_;
	}

	//更新
	worldTransform_.UpdateMatrix();

	//ImGui
	ImGui::Begin("player");
	ImGui::DragFloat3("rotate", &worldTransform_.rotation_.num[0]);
	ImGui::End();
}

void Player::Draw(const ViewProjection& viewProjection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

Vector3 Player::CalculateForwardVector() {
	//Y軸の回転（worldTransform2_.rotation_.num[1]）を用いて前方ベクトルを計算
	float radians = worldTransformForCameraReference_.rotation_.num[1];
	return Vector3{ std::sin(radians), 0.0f, std::cos(radians) };
}

void Player::Move() {
	//左右の回転処理
	if (input_->PressKey(DIK_A)) {//左回転
		worldTransformForCameraReference_.rotation_.num[1] -= rotateCameraSpeed_;
		rotateForCameraReference_.num[0] -= rotateCameraSpeed_;
	}
	if (input_->PressKey(DIK_D)) {//右回転
		worldTransformForCameraReference_.rotation_.num[1] += rotateCameraSpeed_;
		rotateForCameraReference_.num[0] += rotateCameraSpeed_;
	}

	rotateForCameraReference_.num[1] = worldTransformForCameraReference_.rotation_.num[1];

	//移動速度をモードに応じて設定
	switch (moveMode_) {
	case 0: CharacterSpeed_ = lowVelocity_; break;
	case 1: CharacterSpeed_ = duringVelocity_; break;
	case 2: CharacterSpeed_ = highVelocity_; break;
	}

	//クールタイムが明けているなら
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

			if (joystate.Gamepad.sThumbLX > 5000) {//右方向入力
				worldTransformForCameraReference_.rotation_.num[1] += rotateCameraSpeed_;
				rotateForCameraReference_.num[0] += rotateCameraSpeed_;
			}
			else if (joystate.Gamepad.sThumbLX < -5000) {//左方向入力
				worldTransformForCameraReference_.rotation_.num[1] -= rotateCameraSpeed_;
				rotateForCameraReference_.num[0] -= rotateCameraSpeed_;
			}
		}

		if (input_->TriggerXButton(joystate)) {
			moveFlag_ = false;
			moveMode_ = (moveMode_ + 1) % 3;
		}
	}
}

void Player::MoveAttenuation() {
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

void Player::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	Vector3 accelerationVector = { 0.0f,-gravityAcceleration_,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Player::SetTranslate(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
	velocity_.num[1] = 0.0f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Player::SetVelocity(const Vector3 velocity) {
	velocity_ = velocity;
}