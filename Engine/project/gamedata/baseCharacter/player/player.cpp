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
	worldTransform_.rotation_.num[1] += 1.0f;

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

void Player::Move() {
	if (input_->PressKey(DIK_A)) {
		worldTransform2_.rotation_.num[1] -= 0.1f;
		rotate_.num[0] -= 0.1f;
	}
	if (input_->PressKey(DIK_D)) {
		worldTransform2_.rotation_.num[1] += 0.1f;
		rotate_.num[0] += 0.1f;
	}

	rotate_.num[1] = worldTransform2_.rotation_.num[1];

	if (moveMode_ == 0) {
		CharacterSpeed_ = 0.5f;
	}
	if (moveMode_ == 1) {
		CharacterSpeed_ = 0.7f;
	}
	if (moveMode_ == 2) {
		CharacterSpeed_ = 0.9f;
	}

	if (moveFlag_ == true) {
		if (input_->TriggerKey(DIK_SPACE)) {
			moveFlag_ = false;
			Vector3 direction = Normalize(viewProjection_->translation_ - worldTransform2_.translation_);
			velocity_ = -(velocity_ + direction * CharacterSpeed_);
		}
	}

	//加速度のモード変更
	if (input_->TriggerKey(DIK_W)) {
		moveMode_++;
		if (moveMode_ >= 3) {
			moveMode_ = 0;
		}
	}
	if (input_->TriggerKey(DIK_S)) {
		moveMode_--;
		if (moveMode_ < 0) {
			moveMode_ = 2;
		}
	}

	XINPUT_STATE joystate;

	if (moveFlag_ == true) {
		if (Input::GetInstance()->GetJoystickState(0, joystate)) {
			if (Input::GetInstance()->GetJoystickState(0, joystate)) {
				if (input_->TriggerAButton(joystate)) {
					moveFlag_ = false;
					
					//スティックの左右入力に基づいて回転を加算減算
					float rotationSpeed = 0.1f;//回転速度を調整
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
					moveMode_++;
					if (moveMode_ >= 3) {
						moveMode_ = 0;
					}
				}
			}
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