#include "FollowCamera.h"

void FollowCamera::Initialize() {
	viewprojection_.Initialize();
	input_ = Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {
		Vector3 offset = { 0.0f,2.0f,-10.0f };

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);

		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };

		interTarget_ = Lerp(interTarget_, worldTranslate, 0.5f);

		viewprojection_.translation_ = interTarget_ + offset;
	}

	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		const float kRotateSpeed = 0.04f;	

		destinationAngleY_ += (float)joystate.Gamepad.sThumbRX / SHRT_MAX * kRotateSpeed;
	}

	viewprojection_.rotation_.num[1] = LerpShortAngle(viewprojection_.rotation_.num[1], destinationAngleY_, 0.2f);

	viewprojection_.UpdateViewMatrix();
	viewprojection_.TransferMatrix();

}

void FollowCamera::Reset() {
	if (target_) {
		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };
		interTarget_ = worldTranslate;
		viewprojection_.rotation_.num[1] = target_->rotation_.num[1];
	}
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}