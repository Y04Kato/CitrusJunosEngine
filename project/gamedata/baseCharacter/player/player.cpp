#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Player::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	worldTransform_.translation_ = { 0.0f,0.1f,0.0f };
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	if (isHitEnemy_ == true) {
		hitTimer_++;
	}

	if (hitTimer_ >= 20) {
		hitTimer_ = 0;
		isHitEnemy_ = false;
	}

	if (moveFlag_ == false) {
		moveCount_++;
	}
	if (moveCount_ >= 60) {
		moveFlag_ = true;
		moveCount_ = 0;
	}

	if (worldTransform_.translation_.num[1] < -10.0f) {
		gameOver = true;
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

void Player::UpdateView() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_.num[1] += 1.0f;

	worldTransform_.UpdateMatrix();

	if (isHitEnemy_ == true) {
		hitTimer_++;
	}

	if (hitTimer_ >= 20) {
		hitTimer_ = 0;
		isHitEnemy_ = false;
	}
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });

	ImGui::Begin("player");
	ImGui::Text("Move WASD");
	ImGui::Text("MovePowerChange Space");
	ImGui::DragFloat3("vector", velocity_.num);
	ImGui::DragFloat3("vectorC", velocityC_.num);
	ImGui::End();
}

void Player::Move() {
	if (moveFlag_ == true) {
		if (input_->TriggerKey(DIK_W)) {
			moveFlag_ = false;
			if (moveMode_ == 0) {
				velocity_.num[2] = 0.5f;
			}
			if (moveMode_ == 1) {
				velocity_.num[2] = 0.7f;
			}
			if (moveMode_ == 2) {
				velocity_.num[2] = 0.9f;
			}
		}
		if (input_->TriggerKey(DIK_S)) {
			moveFlag_ = false;
			if (moveMode_ == 0) {
				velocity_.num[2] = -0.5f;
			}
			if (moveMode_ == 1) {
				velocity_.num[2] = -0.7f;
			}
			if (moveMode_ == 2) {
				velocity_.num[2] = -0.9f;
			}
		}
		if (input_->TriggerKey(DIK_A)) {
			moveFlag_ = false;
			if (moveMode_ == 0) {
				velocity_.num[0] = -0.5f;
			}
			if (moveMode_ == 1) {
				velocity_.num[0] = -0.7f;
			}
			if (moveMode_ == 2) {
				velocity_.num[0] = -0.9f;
			}
		}
		if (input_->TriggerKey(DIK_D)) {
			moveFlag_ = false;
			if (moveMode_ == 0) {
				velocity_.num[0] = 0.5f;
			}
			if (moveMode_ == 1) {
				velocity_.num[0] = 0.7f;
			}
			if (moveMode_ == 2) {
				velocity_.num[0] = 0.9f;
			}
		}
		if (input_->TriggerKey(DIK_SPACE)) {
			moveFlag_ = false;
			moveMode_++;
			if (moveMode_ >= 3) {
				moveMode_ = 0;
			}
		}
	}

	XINPUT_STATE joystate;

	if (moveFlag_ == true) {
		if (Input::GetInstance()->GetJoystickState(0, joystate)) {
			if (input_->PushXButton(joystate)) {
				moveFlag_ = false;
				moveMode_++;
				if (moveMode_ >= 3) {
					moveMode_ = 0;
				}
			}
			float kCharacterSpeed;
			if (moveMode_ == 0) {
				kCharacterSpeed = 0.5f;
			}
			if (moveMode_ == 1) {
				kCharacterSpeed = 0.7f;
			}
			if (moveMode_ == 2) {
				kCharacterSpeed = 0.9f;
			}
			if (input_->PushAButton(joystate)) {
				moveFlag_ = false;
				velocityC_ = { (float)joystate.Gamepad.sThumbLX / SHRT_MAX, 0.0f,(float)joystate.Gamepad.sThumbLY / SHRT_MAX };

				Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
				velocity_.num[0] = TransformNormal(velocityC_, rotateMatrix).num[0];
				velocity_.num[2] = TransformNormal(velocityC_, rotateMatrix).num[2];
				velocity_.num[0] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[0];
				velocity_.num[2] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[2];

			}
		}
		else {

		}
	}

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
	velocity_ = { 0.0f,0.0f,0.0f };
	velocity_.num[1] = 0.001f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Player::SetVelocity(const Vector3 velocity) {
	if (isHitEnemy_ == false) {
		velocity_ = velocity;
		isHitEnemy_ = true;
	}
}