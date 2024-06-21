#include "DebugCamera.h"

DebugCamera* DebugCamera::GetInstance() {
	static DebugCamera instance;
	return &instance;
}

void DebugCamera::initialize() {
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0,00,-30 };
	viewProjection_.rotation_ = { 0,0,0 };

	input_ = Input::GetInstance();
}

void DebugCamera::Update() {
#ifdef _DEBUG
	if (input_->PressKey(DIK_W)) {
		viewProjection_.rotation_.num[0] -= 0.02f;
	}
	if (input_->PressKey(DIK_S)) {
		viewProjection_.rotation_.num[0] += 0.02f;
	}
	if (input_->PressKey(DIK_D)) {
		viewProjection_.rotation_.num[1] += 0.02f;
	}
	if (input_->PressKey(DIK_A)) {
		viewProjection_.rotation_.num[1] -= 0.02f;
	}

	if (input_->PressKey(DIK_UP)) {
		viewProjection_.translation_.num[1] += 2.0f;
	}
	if (input_->PressKey(DIK_DOWN)) {
		viewProjection_.translation_.num[1] -= 2.0f;
	}
	if (input_->PressKey(DIK_RIGHT)) {
		viewProjection_.translation_.num[0] += 2.0f;
	}
	if (input_->PressKey(DIK_LEFT)) {
		viewProjection_.translation_.num[0] -= 2.0f;
	}

	viewProjection_.translation_.num[2] = input_->GetMousePosition().Scroll / 40.0f;

	ImGui::Begin("DebugCamera");
	ImGui::DragFloat3("rotation", viewProjection_.rotation_.num, 0.1f);
	ImGui::DragFloat3("translation", viewProjection_.translation_.num, 0.1f);
	ImGui::End();
#endif

	if (isMovingCamera == true) {
		timer_ += timerCountr_;
		movingEndTranslate_.num[0] += movingSpeed_.num[0];
		movingEndTranslate_.num[1] += movingSpeed_.num[1];
		movingEndTranslate_.num[2] += movingSpeed_.num[2];
		if (timer_ >= 1.0f) {
			isMovingCamera = false;
			timer_ = 1.0f;
		}
		viewProjection_.translation_.num[0] = (1.0f - timer_) * movingStartTranslate_.num[0] + timer_ * movingEndTranslate_.num[0];
		viewProjection_.translation_.num[1] = (1.0f - timer_) * movingStartTranslate_.num[1] + timer_ * movingEndTranslate_.num[1];
		viewProjection_.translation_.num[2] = (1.0f - timer_) * movingStartTranslate_.num[2] + timer_ * movingEndTranslate_.num[2];
		viewProjection_.rotation_.num[0] = (1.0f - timer_) * movingStartRotate_.num[0] + timer_ * movingEndRotate_.num[0];
		viewProjection_.rotation_.num[1] = (1.0f - timer_) * movingStartRotate_.num[1] + timer_ * movingEndRotate_.num[1];
		viewProjection_.rotation_.num[2] = (1.0f - timer_) * movingStartRotate_.num[2] + timer_ * movingEndRotate_.num[2];
	}


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

void DebugCamera::MovingCamera(Vector3 translation, Vector3 rotation, float timerSpeed) {
	timer_ = 0.0f;
	timerCountr_ = timerSpeed;
	movingStartTranslate_ = viewProjection_.translation_;
	movingStartRotate_ = viewProjection_.rotation_;
	movingEndTranslate_ = translation;
	movingEndRotate_ = rotation;
	isMovingCamera = true;
}

void DebugCamera::SetMovingSpeed(Vector3 moveSpeed) {
	movingSpeed_ = moveSpeed;
}