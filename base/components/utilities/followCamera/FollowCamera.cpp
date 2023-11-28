#include "FollowCamera.h"
#include "GlobalVariables.h"
#include "lockOnCamera/LockOn.h"

void FollowCamera::Initialize() {
	viewprojection_.Initialize();
	input_ = Input::GetInstance();

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "FollowCamera";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Latency", latency);
}

void FollowCamera::Update() {
	ApplyGlobalVariables();

	if (target_) {
		Vector3 offset = { 0.0f,2.0f,-10.0f };

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);

		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };

		interTarget_ = Lerp(interTarget_, worldTranslate, latency);

		viewprojection_.translation_ = interTarget_ + offset;
	}

	if (lockOn_ && lockOn_->Existtarget()) {
		Vector3 LockOntrans = lockOn_->GetTargetPos();
		Vector3 sub = LockOntrans - GetTargetWordPos();

		viewprojection_.rotation_.num[1] = Angle({0.0f,0.0f,1.0f}, sub);
	}
	else {
		XINPUT_STATE joystate;

		if (Input::GetInstance()->GetJoystickState(0, joystate)) {
			const float kRotateSpeed = 0.04f;

			destinationAngleY_ += (float)joystate.Gamepad.sThumbRX / SHRT_MAX * kRotateSpeed;

			if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
				destinationAngleY_ = 0.0f;
			}
		}

		viewprojection_.rotation_.num[1] = LerpShortAngle(viewprojection_.rotation_.num[1], destinationAngleY_, 0.2f);
	}

	viewprojection_.UpdateViewMatrix();
	viewprojection_.TransferMatrix();
}

void FollowCamera::Reset() {
	if (target_) {
		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };
		interTarget_ = Lerp(interTarget_, worldTranslate, 0.5f);
		viewprojection_.rotation_.num[1] = target_->rotation_.num[1];
	}
	destinationAngleY_ = viewprojection_.rotation_.num[1];

	Vector3 offset = { 0.0f,2.0f,-10.0f };

	Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

	offset = TransformNormal(offset, rotateMatrix);

	viewprojection_.translation_ = interTarget_ + offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

Vector3 FollowCamera::GetTargetWordPos(){
	Vector3 result;
	result.num[0] = target_->matWorld_.m[3][0];
	result.num[1] = target_->matWorld_.m[3][1];
	result.num[2] = target_->matWorld_.m[3][2];
	return result;
}

void FollowCamera::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";

	latency = globalVariables->GetFloatValue(groupName, "Latency");
}