#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include "GlobalVariables.h"
#include <math.h>

#include "lockOnCamera/LockOn.h"

const std::array<ConstAttack, Player::comboNum_>
Player::kConstAttacks_ = { {
	{5,1,20,10,0.0f,0.0f,0.15f},
	{15,10,15,10,0.2f,0.0f,0.0f},
	{15,10,15,30,0.2f,0.0f,0.0f},
	}
};

void Player::Initialize(const std::vector<Model*>& models) {
	// NULLポインタチェック
	BaseCharacter::Initialize(models);

	models_[kModelIndexBody] = models[kModelIndexBody];
	models_[kModelIndexHead] = models[kModelIndexHead];
	models_[kModelIndexL_arm] = models[kModelIndexL_arm];
	models_[kModelIndexR_arm] = models[kModelIndexR_arm];
	models_[kModelIndexWeapon] = models[kModelIndexWeapon];

	worldTransformHead_.translation_.num[1] = 1.8f;

	worldTransformL_arm_.translation_.num[0] = -0.5f;
	worldTransformL_arm_.translation_.num[1] = 1.8f;

	worldTransformR_arm_.translation_.num[0] = 0.5f;
	worldTransformR_arm_.translation_.num[1] = 1.8f;

	worldTransformWeapon_.translation_.num[0] = -0.2f;
	worldTransformWeapon_.translation_.num[1] = 1.6f;
	worldTransformWeapon_.scale_ = { 1.0f, 1.0f, 1.0f };

	InitializeFloatingGimmick();

	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	worldTransformWeapon_.Initialize();
	worldTransform_.Initialize();

	SetParent(&GetWorldTransformBody());

	input_ = Input::GetInstance();

	quaternion_ = createQuaternion(0.0f, { 0.0f,1.0f,0.0f });
	quaternion_ = Normalize(quaternion_);

	worldTransformBody_.translation_ = { 0.0f,0.1f,0.0f };

	isHit_ = false;
	SetCollisionAttribute(CollisionConfig::kCollisionAttributePlayer);
	SetCollisionMask(~CollisionConfig::kCollisionAttributePlayer);

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "Player";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
	globalVariables->AddItem(groupName, "Head Translation", worldTransformHead_.translation_);
	globalVariables->AddItem(groupName, "L_Arm Translation", worldTransformL_arm_.translation_);
	globalVariables->AddItem(groupName, "R_Arm Translation", worldTransformR_arm_.translation_);
	globalVariables->AddItem(groupName, "Weapon Translation", worldTransformWeapon_.translation_);
	globalVariables->AddItem(groupName, "floatingCycle", floatingCycle_);
	globalVariables->AddItem(groupName, "floatingAmplitude", floatingAmplitude_);
	globalVariables->AddItem(groupName, "DashSpeed", dashSpeed);
}

void Player::Update() {

	ApplyGlobalVariables();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	XINPUT_STATE joyState;

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
		if (isAttack == false) {
			isAttack = true;
			behaviorRequest_ = Behavior::kAttack;
		}
	}
	else {
		isAttack2 = false;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		behaviorRequest_ = Behavior::kDash;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		if (!aButtonPressed) {
			behaviorRequest_ = Behavior::kJump;
			aButtonPressed = true;
		}
	}
	else {
		aButtonPressed = false;
	}

	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Player::Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Player::Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Player::Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Player::Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Player::Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

	ImGui::Begin("Player");
	ImGui::SliderFloat("Head Translation", &worldTransformHead_.translation_.num[0], -5.0f, 5.0f);
	ImGui::SliderFloat("ArmL Translation", &worldTransformL_arm_.translation_.num[0], -5.0f, 5.0f);
	ImGui::SliderFloat("ArmR Translation", &worldTransformR_arm_.translation_.num[0], -5.0f, 5.0f);
	ImGui::SliderFloat3("Body Translate", worldTransformBody_.translation_.num, -5.0f, 5.0f);
	ImGui::SliderFloat3("Wepon Rotate", worldTransformWeapon_.rotation_.num, -5.0f, 5.0f);
	ImGui::SliderInt("FloatingCycle", (int*)&floatingCycle_, 0, 240);
	ImGui::SliderFloat("FloatingAmplitude", &floatingAmplitude_, -1.0f, 1.0f);
	ImGui::Text("Attack : X");
	ImGui::Text("Dash : B");
	ImGui::Text("Jump : A");
	ImGui::Text("comb : %d", workAtack_.comboIndex);
	ImGui::Text("velocity : %f %f %f", velocity_.num[0], velocity_.num[1], velocity_.num[2]);
	ImGui::End();

	if (worldTransformBody_.translation_.num[1] < -10.0f) {
		gameOver = true;
	}
	if (!isHit_ || worldTransformBody_.GetWorldPos().num[1] < 0.0f) {
		IsFallStart();
	}
	else if (behavior_ == Behavior::kJump) {
	}
	else {
		worldTransformBody_.translation_.num[1] = objectPos_.translation_.num[1] + objectPos_.scale_.num[1] + worldTransformBody_.scale_.num[1] - 1.5f;
		velocity_.num[1] = 0.001f;
	}

	structSphere_.center = worldTransformBody_.GetWorldPos();
	structSphere_.radius = 1.5f;

	obb_.center = worldTransformWeapon_.GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(worldTransformWeapon_.rotation_), obb_.orientation);
	obb_.size.num[0] = worldTransformWeapon_.scale_.num[0] * 2;
	obb_.size.num[1] = worldTransformWeapon_.scale_.num[1] * 3;
	obb_.size.num[2] = worldTransformWeapon_.scale_.num[2];

	worldTransformBase_.UpdateQuaternionMatrix();
	worldTransformBody_.UpdateQuaternionMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	worldTransformWeapon_.UpdateMatrix();
	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	// 3Dモデルを描画
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });

	if (behavior_ == Behavior::kAttack) {
		models_[kModelIndexWeapon]->Draw(worldTransformWeapon_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}
}


void Player::SetParent(const WorldTransform* parent) {
	worldTransformBase_.parent_ = parent;
	worldTransformHead_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransformWeapon_.parent_ = parent;
	worldTransform_.parent_ = parent;
}

void Player::InitializeFloatingGimmick() { floatingParameter_ = 0.0f; }

void Player::UpdateFloatingGimmick() {
	const float step = 2.0f * (float)M_PI / floatingCycle_;

	floatingParameter_ += step;
	floatingParameter_ = (float)std::fmod(floatingParameter_, 2.0f * M_PI);

	//worldTransformBody_.translation_.num[1] = std::sin(floatingParameter_) * floatingAmplitude_;

	worldTransformL_arm_.rotation_.num[0] = std::sin(floatingParameter_) * 0.75f;
	worldTransformR_arm_.rotation_.num[0] = std::sin(floatingParameter_) * 0.75f;
}

void Player::BehaviorRootInitialize() {
	worldTransformL_arm_.rotation_.num[0] = 0.0f;
	worldTransformR_arm_.rotation_.num[0] = 0.0f;
	worldTransformWeapon_.rotation_.num[0] = 0.0f;

	InitializeFloatingGimmick();

	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformWeapon_.Initialize();
}

void Player::BehaviorRootUpdate() {
	isAttack = false;

	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		if (!isHit_ || worldTransformBody_.GetWorldPos().num[1] < 0.0f) {}
		else {
			const float kCharacterSpeed = 0.3f;
			velocity_ = {
				(float)joystate.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
				(float)joystate.Gamepad.sThumbLY / SHRT_MAX };
			if (CompereVector3(velocity_, { 0.0f,0.0f,0.0f })) {
				isMove_ = false;
			}
			else {
				isMove_ = true;
			}
			if (isMove_ == true) {
				Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
				velocity_ = TransformNormal(velocity_, rotateMatrix);
				velocity_ = Multiply(kCharacterSpeed, Normalize(velocity_));
				worldTransformBody_.translation_ = Add(velocity_, worldTransformBody_.translation_);
				preQuaternion_ = quaternion_;

				Vector3 newPos = Subtruct(Add(worldTransformBody_.translation_, velocity_), worldTransformBody_.translation_);
				Vector3 Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));;


				Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));

				Direction = Normalize(Direction);
				Vector3 newDirection = Normalize(newPos);
				float cosin = Dot(Direction, newDirection);

				Quaternion newquaternion_;
				newquaternion_ = createQuaternion(cosin, { 0.0f,1.0f,0.0f });

				quaternion_ = Normalize(quaternion_);
				newquaternion_ = Normalize(newquaternion_);

				quaternion_ = Multiply(quaternion_, newquaternion_);
				if (CompereQuaternion(quaternion_, preQuaternion_) && !CompereVector3(velocity_, preMove_)) {
					cosin = -1.0f;
					quaternion_ = Multiply(quaternion_, createQuaternion(cosin, { 0.0f,1.0f,0.0f }));
				}

				preMove_ = velocity_;
			}
			else if (LockOn_ && LockOn_->Existtarget()) {
				Vector3 Direction;
				//プレイヤーの現在の向き
				Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));

				Direction = Normalize(Direction);
				Vector3 newPos = Subtruct(LockOn_->GetTargetPos(), worldTransformBody_.translation_);
				Vector3 newDirection = Normalize(newPos);
				float cosin = Dot(Direction, newDirection);

				//行きたい方向のQuaternionの作成
				Quaternion newquaternion_;

				newquaternion_ = createQuaternion(cosin, { 0.0f,1.0f,0.0f });

				//quaternionの合成
				quaternion_ = Normalize(quaternion_);
				newquaternion_ = Normalize(newquaternion_);

				quaternion_ = Multiply(quaternion_, newquaternion_);
			}
			worldTransformBody_.quaternion_ = Slerp(0.3f, worldTransformBody_.quaternion_, quaternion_);
		}
	}

	UpdateFloatingGimmick();
}

void Player::BehaviorAttackInitialize() {
	worldTransformL_arm_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformR_arm_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformWeapon_.rotation_ = { 0.0f ,0.0f,0.0f };
	animationFrame_ = 0;
	workAtack_.rotate = (float)M_PI;
	workAtack_.hammerRotate = 0.0f;
	workAtack_.Time = 0;
	workAtack_.comboNext = false;
	workAtack_.comboIndex = 0;
	isAttack2 = true;
}

void Player::BehaviorAttackUpdate() {
	XINPUT_STATE joystate;
	uint32_t anticipationTime = kConstAttacks_[workAtack_.comboIndex].anticipationTime;
	uint32_t chargeTime = kConstAttacks_[workAtack_.comboIndex].anticipationTime + kConstAttacks_[workAtack_.comboIndex].chargeTime;
	uint32_t swingTime = chargeTime + kConstAttacks_[workAtack_.comboIndex].swingTime;
	uint32_t recoveryTime = swingTime + kConstAttacks_[workAtack_.comboIndex].recoveryTime;

	if (workAtack_.comboIndex < comboNum_ - 1) {
		if (Input::GetInstance()->GetJoystickState(0, joystate)) {
			if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_X && isAttack2 == false) {
				workAtack_.comboNext = true;
				isAttack2 = true;
			}
		}
	}

	if (LockOn_ && LockOn_->Existtarget()) {
		Vector3 Direction;
		//プレイヤーの現在の向き
		Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));

		Direction = Normalize(Direction);
		Vector3 newPos = Subtruct(LockOn_->GetTargetPos(), worldTransformBody_.translation_);
		Vector3 newDirection = Normalize(newPos);
		float cosin = Dot(Direction, newDirection);

		//行きたい方向のQuaternionの作成
		Quaternion newquaternion_;

		newquaternion_ = createQuaternion(cosin, { 0.0f,1.0f,0.0f });

		//quaternionの合成
		quaternion_ = Normalize(quaternion_);
		newquaternion_ = Normalize(newquaternion_);

		quaternion_ = Multiply(quaternion_, newquaternion_);

		worldTransformBody_.quaternion_ = Slerp(0.3f, worldTransformBody_.quaternion_, quaternion_);

		worldTransform_.quaternion_ = worldTransformBody_.quaternion_;
	}

	if (workAtack_.Time >= recoveryTime) {
		if (workAtack_.comboNext) {
			workAtack_.comboNext = false;
			workAtack_.Time = 0;
			workAtack_.comboIndex++;
			switch (workAtack_.comboIndex) {
			case 0:
				workAtack_.rotate = (float)M_PI;
				workAtack_.hammerRotate = 0.0f;
				workAtack_.Time = 0;
				break;

			case 1:
				workAtack_.rotate = (float)M_PI;
				workAtack_.hammerRotate = 0.0f;
				workAtack_.Time = 0;
				worldTransformL_arm_.rotation_ = { 0.0f,0.0f,0.0f };
				worldTransformR_arm_.rotation_ = { 0.0f,0.0f,0.0f };
				worldTransformWeapon_.rotation_ = { 0.0f ,0.0f,1.5f };
				break;

			case 2:
				workAtack_.rotate = (float)M_PI;
				workAtack_.hammerRotate = 0.0f;
				workAtack_.Time = 0;
				worldTransformL_arm_.rotation_ = { 0.0f,0.0f,0.0f };
				worldTransformR_arm_.rotation_ = { 0.0f,0.0f,0.0f };
				worldTransformWeapon_.rotation_ = { 0.0f ,0.0f,1.5f };
				break;
			}
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
			workAtack_.comboIndex = 0;
		}
	}

	switch (workAtack_.comboIndex) {
	case 0:
		if (workAtack_.Time < 12) {
			worldTransformL_arm_.rotation_.num[0] += 0.05f;
			worldTransformR_arm_.rotation_.num[0] += 0.05f;

			worldTransformWeapon_.rotation_.num[0] += 0.05f;
		}
		else if (worldTransformWeapon_.rotation_.num[0] >= -2.0f * (float)M_PI / 4) {
			worldTransformL_arm_.rotation_.num[0] -= 0.1f;
			worldTransformR_arm_.rotation_.num[0] -= 0.1f;

			worldTransformWeapon_.rotation_.num[0] -= 0.1f;
			worldTransformBody_.translation_ += velocity_;
		}
		break;

	case 1:
		worldTransformWeapon_.translation_.num[1] = 1.6f;
		if (workAtack_.Time < 12) {
			worldTransformWeapon_.rotation_.num[0] += 0.15f;
		}
		else if (worldTransformWeapon_.rotation_.num[0] >= -3.0f * (float)M_PI / 4) {
			worldTransformWeapon_.rotation_.num[0] -= 0.3f;
			worldTransformBody_.translation_ += velocity_;
		}
		break;

	case 2:
		worldTransformWeapon_.translation_.num[1] = 1.6f;
		if (workAtack_.Time < 12) {
		}
		else if (worldTransformWeapon_.rotation_.num[0] >= -3.0f * (float)M_PI / 4) {
			worldTransformWeapon_.rotation_.num[0] += 0.3f;
		}
		break;
	}

	workAtack_.Time++;
}

void Player::BehaviorDashInitialize() {
	workDash_.dashParameter_ = 0;
}

void Player::BehaviorDashUpdate() {
	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		if (!isHit_ || worldTransformBody_.GetWorldPos().num[1] < 0.0f) {}
		else {
			const float kCharacterSpeed = dashSpeed;
			velocity_ = {
				(float)joystate.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
				(float)joystate.Gamepad.sThumbLY / SHRT_MAX };
			if (CompereVector3(velocity_, { 0.0f,0.0f,0.0f })) {
				isMove_ = false;
			}
			else {
				isMove_ = true;
			}
			if (isMove_ == true) {
				Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
				velocity_ = TransformNormal(velocity_, rotateMatrix);
				velocity_ = Multiply(kCharacterSpeed, Normalize(velocity_));
				worldTransformBody_.translation_ = Add(velocity_, worldTransformBody_.translation_);
				preQuaternion_ = quaternion_;

				Vector3 newPos = Subtruct(Add(worldTransformBody_.translation_, velocity_), worldTransformBody_.translation_);
				Vector3 Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));;


				Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));

				Direction = Normalize(Direction);
				Vector3 newDirection = Normalize(newPos);
				float cosin = Dot(Direction, newDirection);

				Quaternion newquaternion_;
				newquaternion_ = createQuaternion(cosin, { 0.0f,1.0f,0.0f });

				quaternion_ = Normalize(quaternion_);
				newquaternion_ = Normalize(newquaternion_);

				quaternion_ = Multiply(quaternion_, newquaternion_);
				if (CompereQuaternion(quaternion_, preQuaternion_) && !CompereVector3(velocity_, preMove_)) {
					cosin = -1.0f;
					quaternion_ = Multiply(quaternion_, createQuaternion(cosin, { 0.0f,1.0f,0.0f }));
				}

				preMove_ = velocity_;
			}
			else if (LockOn_ && LockOn_->Existtarget()) {
				Vector3 Direction;
				//プレイヤーの現在の向き
				Direction = TransformNormal({ 1.0f,0.0f,0.0f }, quaternionToMatrix(quaternion_));

				Direction = Normalize(Direction);
				Vector3 newPos = Subtruct(LockOn_->GetTargetPos(), worldTransformBody_.translation_);
				Vector3 newDirection = Normalize(newPos);
				float cosin = Dot(Direction, newDirection);

				//行きたい方向のQuaternionの作成
				Quaternion newquaternion_;

				newquaternion_ = createQuaternion(cosin, { 0.0f,1.0f,0.0f });

				//quaternionの合成
				quaternion_ = Normalize(quaternion_);
				newquaternion_ = Normalize(newquaternion_);

				quaternion_ = Multiply(quaternion_, newquaternion_);
			}

			worldTransformBody_.quaternion_ = Slerp(0.3f, worldTransformBody_.quaternion_, quaternion_);
		}
	}

	const uint32_t behaviorDashTime = 10;

	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJumpInitialize() {
	worldTransformBody_.translation_.num[1] = 0.5f;
	worldTransformL_arm_.rotation_.num[0] = 0;
	worldTransformR_arm_.rotation_.num[0] = 0;

	//ジャンプ初速
	const float kJumpFirstSpeed = 0.7f;
	velocity_.num[1] = kJumpFirstSpeed;
}

void Player::BehaviorJumpUpdate() {
	worldTransformBody_.translation_ += velocity_;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];

	if (worldTransformBody_.translation_.num[1] <= objectPos_.translation_.num[1]) {
		worldTransformBody_.translation_.num[1] = objectPos_.translation_.num[1];
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	worldTransformHead_.translation_ =
		globalVariables->GetVector3Value(groupName, "Head Translation");
	worldTransformL_arm_.translation_ =
		globalVariables->GetVector3Value(groupName, "L_Arm Translation");
	worldTransformR_arm_.translation_ =
		globalVariables->GetVector3Value(groupName, "R_Arm Translation");
	worldTransformWeapon_.translation_ =
		globalVariables->GetVector3Value(groupName, "Weapon Translation");

	floatingCycle_ = globalVariables->GetIntValue(groupName, "floatingCycle");
	floatingAmplitude_ = globalVariables->GetFloatValue(groupName, "floatingAmplitude");

	dashSpeed = globalVariables->GetFloatValue(groupName, "DashSpeed");
}

void Player::IsFallStart() {
	worldTransformBody_.translation_.num[1] += velocity_.num[1] / 2;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_.num[1] += accelerationVector.num[1];
}

void Player::OnCollision() {
	gameOver = true;
}

void Player::Setparent(const WorldTransform* parent) {
	worldTransformBody_.parent_ = parent;
}

void Player::IsCollision(const WorldTransform& worldtransform) {
	if (!worldTransformBody_.parent_) {

		worldTransformBody_.translation_.num[1] = worldtransform.translation_.num[1] + worldtransform.scale_.num[1] + worldTransformBody_.scale_.num[1];
		Vector3 worldPos = worldTransformBody_.GetWorldPos();
		Vector3 objectWorldPos = { worldtransform.matWorld_.m[3][0],worldtransform.matWorld_.m[3][1],worldtransform.matWorld_.m[3][2] };
		Vector3 Position = worldPos - objectWorldPos;

		Matrix4x4 rotatematrix = MakeRotateXYZMatrix({ -worldtransform.rotation_.num[0] ,-worldtransform.rotation_.num[1] ,-worldtransform.rotation_.num[2] });
		Position = TransformNormal(Position, rotatematrix);

		worldTransformBody_.translation_ = Position;
		Setparent(&worldtransform);
		gameOver = false;
	}
}

void Player::DeleteParent() {
	if (worldTransformBody_.parent_) {
		worldTransformBody_.translation_ = worldTransformBody_.GetWorldPos();
		worldTransformBody_.parent_ = nullptr;
	}
}

void Player::SetWorldTransform(const Vector3 translation) {
	worldTransformBody_.translation_ = translation;
	velocity_.num[1] = 0.001f;
	gameOver = false;
}

void Player::SetObjectPos(const WorldTransform& worldtransform) {
	objectPos_ = worldtransform;
}