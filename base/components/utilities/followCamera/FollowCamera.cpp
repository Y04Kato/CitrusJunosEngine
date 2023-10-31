#include "FollowCamera.h"

void FollowCamera::Initialize() {
	viewprojection_.Initialize();
	input_ = Input::GetInstance();
}

void FollowCamera::Update() {
	if (target_) {

		Vector3 offset = { 0.0f, 2.0f, -10.0f };

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);
		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };
		viewprojection_.translation_ = Add(worldTranslate, offset);
	}

	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		const float kRotateSpeed = 0.02f;
		viewprojection_.rotation_.num[1] += (float)joystate.Gamepad.sThumbRX / SHRT_MAX * kRotateSpeed;
	}

	viewprojection_.UpdateViewMatrix();
	viewprojection_.TransferMatrix();

}