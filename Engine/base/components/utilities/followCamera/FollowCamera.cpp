/**
 * @file FloowCamera.cpp
 * @brief 指定した対象からオフセットを元にカメラ管理を行う
 * @author KATO
 * @date 未記録
 */

#include "FollowCamera.h"
#include "GlobalVariables.h"

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
		Vector3 offset = offset_;

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);

		Vector3 worldTranslate = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };

		interTarget_ = Lerp(interTarget_, worldTranslate, latency);

		viewprojection_.translation_ = interTarget_ + offset;
	}

	XINPUT_STATE joystate;

	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		const float kRotateSpeed = 0.04f;

		destinationAngleY_ += (float)joystate.Gamepad.sThumbRX / SHRT_MAX * kRotateSpeed;

		if (joystate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = 0.0f;
		}
	}

	if (target_) {
		viewprojection_.rotation_ = target_->rotation_;
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

	Vector3 offset = offset_;

	Matrix4x4 rotateMatrix = MakeRotateMatrix(viewprojection_.rotation_);

	offset = TransformNormal(offset, rotateMatrix);

	viewprojection_.translation_ = interTarget_ + offset;
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

void FollowCamera::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";

	latency = globalVariables->GetFloatValue(groupName, "Latency");
}

void FollowCamera::ShakeCamera(int shakePower, int dividePower) {
	//ランダムなシェイクを加える
	viewprojection_.translation_.num[0] += (rand() % shakePower - shakePower / 2 + static_cast<float>(rand()) / RAND_MAX) / dividePower;
	viewprojection_.translation_.num[1] += (rand() % shakePower - shakePower / 2 + static_cast<float>(rand()) / RAND_MAX) / dividePower;
	viewprojection_.translation_.num[2] += (rand() % shakePower - shakePower / 2 + static_cast<float>(rand()) / RAND_MAX) / dividePower;

	//ビュー行列を更新
	viewprojection_.UpdateViewMatrix();
	viewprojection_.TransferMatrix();
}

void FollowCamera::SetCamera(Vector3 translation, Vector3 rotation) {
	//カメラの位置と回転を設定
	viewprojection_.translation_ = translation;
	viewprojection_.rotation_ = rotation;

	//ビュー行列を更新
	viewprojection_.UpdateViewMatrix();
	viewprojection_.TransferMatrix();
}