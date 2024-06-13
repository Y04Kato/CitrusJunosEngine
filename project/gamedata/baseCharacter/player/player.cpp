#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Player::Initialize(Model* model) {
	// NULLポインタチェック
	BaseCharacter::Initialize(model);

	input_ = Input::GetInstance();

	worldTransform_.translation_ = { 0.0f,0.1f,0.0f };

	quaternion_ = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f },0.0f);
	quaternion_ = Normalize(quaternion_);

	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);
	SetRadius(1.4f);
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	Move();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	if (isHit == true) {
		hitCount++;
	}

	if (hitCount >= 20) {
		hitCount = 0;
		isHit = false;
	}

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

void Player::UpdateView() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	structSphere_.center = worldTransform_.GetWorldPos();
	structSphere_.radius = 1.5f;

	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_.num[1] += 1.0f;

	worldTransform_.UpdateMatrix();

	if (isHit == true) {
		hitCount++;
	}

	if (hitCount >= 20) {
		hitCount = 0;
		isHit = false;
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

	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		const float kCharacterSpeed = 0.5f;
		if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			velocityC_ = { (float)joystate.Gamepad.sThumbLX / SHRT_MAX, 0.0f,(float)joystate.Gamepad.sThumbLY / SHRT_MAX };

			Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
			velocity_.num[0] = TransformNormal(velocityC_, rotateMatrix).num[0];
			velocity_.num[2] = TransformNormal(velocityC_, rotateMatrix).num[2];
			velocity_.num[0] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[0];
			velocity_.num[2] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[2];

		}
		/*Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
			velocity_.num[0] = TransformNormal(velocityC_, rotateMatrix).num[0];
			velocity_.num[2] = TransformNormal(velocityC_, rotateMatrix).num[2];
			velocity_.num[0] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[0];
			velocity_.num[2] = Multiply(kCharacterSpeed, Normalize(velocity_)).num[2];*/

			//worldTransform_.translation_ = Add(velocity_, worldTransform_.translation_);
			/*preQuaternion_ = quaternion_;

			Vector3 newPos = Subtruct(Add(worldTransform_.translation_, velocity_), worldTransform_.translation_);
			Vector3 Direction = TransformNormal({ 1.0f,0.0f,0.0f }, MakeRotateMatrix(quaternion_));;


			Direction = TransformNormal({ 1.0f,0.0f,0.0f }, MakeRotateMatrix(quaternion_));

			Direction = Normalize(Direction);
			Vector3 newDirection = Normalize(newPos);
			float cosin = Dot(Direction, newDirection);

			Quaternion newquaternion_;
			newquaternion_ = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f },cosin);

			quaternion_ = Normalize(quaternion_);
			newquaternion_ = Normalize(newquaternion_);

			quaternion_ = Multiply(quaternion_, newquaternion_);
			if (CompereQuaternion(quaternion_, preQuaternion_) && !CompereVector3(velocity_, preMove_)) {
				cosin = -1.0f;
				quaternion_ = Multiply(quaternion_, MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f },cosin));
			}

			preMove_ = velocity_;*/
	}
	//worldTransform_.quaternion_ = Slerp(0.3f, worldTransform_.quaternion_, quaternion_);

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

void Player::OnCollision() {

}

void Player::IsFallStart() {
	worldTransform_.translation_.num[1] += velocity_.num[1] / 2;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Player::SetWorldTransform(const Vector3 translation) {
	worldTransform_.translation_ = translation;
	velocity_ = {0.0f,0.0f,0.0f};
	velocity_.num[1] = 0.001f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}

void Player::SetVelocity(const Vector3 velocity) {
	if (isHit == false) {
		velocity_ = velocity;
		isHit = true;
	}
}