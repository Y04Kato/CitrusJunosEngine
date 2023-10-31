#include "DebugCamera.h"

DebugCamera* DebugCamera::GetInstance() {
	static DebugCamera instance;
	return &instance;
}

void DebugCamera::initialize() {
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0,0,-30 };
	viewProjection_.rotation_ = { 0,0,0 };

	input_ = Input::GetInstance();
}

void DebugCamera::Update() {
#ifdef _DEBUG
	if (input_->PressKey(DIK_UPARROW)) {
		viewProjection_.rotation_.num[0] -= 0.05f;
	}
	if (input_->PressKey(DIK_DOWNARROW)) {
		viewProjection_.rotation_.num[0] += 0.05f;
	}
	if (input_->PressKey(DIK_RIGHTARROW)) {
		viewProjection_.rotation_.num[1] += 0.05f;
	}
	if (input_->PressKey(DIK_LEFTARROW)) {
		viewProjection_.rotation_.num[1] -= 0.05f;
	}

	ImGui::Begin("DebugCamera");
	ImGui::SliderFloat3("rotation", viewProjection_.rotation_.num, -20, 20);
	ImGui::SliderFloat3("translation", viewProjection_.translation_.num, -20, 20);
	ImGui::End();
#endif
	viewProjection_.UpdateMatrix();
}

void DebugCamera::ShakeCamera(int shakePower, int dividePower) {
	viewProjection_.translation_.num[0] += (rand() % shakePower - shakePower / 2 + rand() / (float)RAND_MAX) / dividePower;
	viewProjection_.translation_.num[1] += (rand() % shakePower - shakePower / 2 + rand() / (float)RAND_MAX) / dividePower;
	viewProjection_.translation_.num[2] += (rand() % shakePower - shakePower / 2 + rand() / (float)RAND_MAX) / dividePower;
}

void DebugCamera::SetCamera(Vector3 translation, Vector3 rotation) {
	viewProjection_.translation_ = translation;
	viewProjection_.rotation_ = rotation;
}