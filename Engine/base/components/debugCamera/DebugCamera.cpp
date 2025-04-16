/**
 * @file DebugCamera.cpp
 * @brief カメラ及び視点関係の初期化及び管理を行う
 * @author KATO
 * @date 未記録
 */

#include "DebugCamera.h"

DebugCamera* DebugCamera::GetInstance() {
	static DebugCamera instance;
	return &instance;
}

void DebugCamera::initialize() {
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f,0.0f,-45.0f };
	viewProjection_.rotation_ = { 0.0f,0.0f,0.0f };

	input_ = Input::GetInstance();
}

void DebugCamera::Update() {
#ifdef _DEBUG
	// TABキーでキー操作ON/OFFをトグル
	bool currentTabKey = input_->PressKey(DIK_TAB);
	if (currentTabKey && !prevTabKeyState_) {
		isKeyControlCamera_ = !isKeyControlCamera_;
		input_->ToggleCursor();
	}
	prevTabKeyState_ = currentTabKey;

	if (isKeyControlCamera_ == true) {
		SetCursorPos(750, 450);//カーソル固定

		// 回転から向きを算出
		Matrix4x4 rotMat = MakeRotateMatrix(viewProjection_.rotation_);
		Vector3 forward = rotMat * Vector3{ 0.0f, 0.0f, 1.0f };
		Vector3 right = rotMat * Vector3{ 1.0f, 0.0f, 0.0f };
		Vector3 up = Vector3{ 0.0f, 1.0f, 0.0f }; //上方向はY軸固定

		// 移動入力
		Vector3 move = { 0.0f, 0.0f, 0.0f };
		if (input_->PressKey(DIK_W)) move += forward;
		if (input_->PressKey(DIK_S)) move -= forward;
		if (input_->PressKey(DIK_D)) move += right;
		if (input_->PressKey(DIK_A)) move -= right;
		if (input_->PressKey(DIK_SPACE)) move += up;
		if (input_->PressKey(DIK_LSHIFT)) move -= up;

		// 正規化して移動（斜めでも速度一定に）
		if (Length(move) > 0.0f) {
			move = Normalize(move) * moveSpeed_;
			viewProjection_.translation_ += move;
		}

		// マウス移動でカメラ回転（Y: 横回転, X: 縦回転）
		Vector2 mouseMove = input_->GetMousePosition().Velocity;
		viewProjection_.rotation_.num[1] += mouseMove.num[0] * mouseSensitivity_;
		viewProjection_.rotation_.num[0] += mouseMove.num[1] * mouseSensitivity_;

		// X軸の回転制限（上下見すぎ防止）
		viewProjection_.rotation_.num[0] = std::clamp(viewProjection_.rotation_.num[0], -89.9f, 89.9f);
	}
#endif

	// マウス感度や移動速度を ImGui で調整
	ImGui::Begin("DebugCamera");
	ImGui::Checkbox("KeyControlCamera", &isKeyControlCamera_);
	ImGui::DragFloat3("rotation", viewProjection_.rotation_.num, 0.1f);
	ImGui::DragFloat3("translation", viewProjection_.translation_.num, 0.1f);
	ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity_, 0.01f, 0.01f, 2.0f);
	ImGui::DragFloat("Move Speed", &moveSpeed_, 0.01f, 0.01f, 2.0f);
	ImGui::End();

	if (isMovingCamera == true) {
		timer_ += timerCountr_;
		movingEndTranslate_.num[0] += movingSpeed_.num[0];
		movingEndTranslate_.num[1] += movingSpeed_.num[1];
		movingEndTranslate_.num[2] += movingSpeed_.num[2];

		if (timer_ >= 1.0f) {
			isMovingCamera = false;
			timer_ = 1.0f;
		}

		//各回転成分を正規化し、短い回転経路を選択
		float rotationDiffX = NormalizeAngle(movingEndRotate_.num[0] - movingStartRotate_.num[0]);
		float rotationDiffY = NormalizeAngle(movingEndRotate_.num[1] - movingStartRotate_.num[1]);
		float rotationDiffZ = NormalizeAngle(movingEndRotate_.num[2] - movingStartRotate_.num[2]);

		//回転の内挿
		viewProjection_.translation_.num[0] = (1.0f - timer_) * movingStartTranslate_.num[0] + timer_ * movingEndTranslate_.num[0];
		viewProjection_.translation_.num[1] = (1.0f - timer_) * movingStartTranslate_.num[1] + timer_ * movingEndTranslate_.num[1];
		viewProjection_.translation_.num[2] = (1.0f - timer_) * movingStartTranslate_.num[2] + timer_ * movingEndTranslate_.num[2];

		viewProjection_.rotation_.num[0] = movingStartRotate_.num[0] + timer_ * rotationDiffX;
		viewProjection_.rotation_.num[1] = movingStartRotate_.num[1] + timer_ * rotationDiffY;
		viewProjection_.rotation_.num[2] = movingStartRotate_.num[2] + timer_ * rotationDiffZ;

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

	viewProjection_.UpdateMatrix();
	isMovingCamera = false;
}

void DebugCamera::MovingCamera(Vector3 translation, Vector3 rotation, float timerSpeed) {
	movingStartTranslate_ = viewProjection_.translation_;
	movingStartRotate_ = viewProjection_.rotation_;

	timer_ = 0.0f;
	timerCountr_ = timerSpeed;
	movingEndTranslate_ = translation;
	movingEndRotate_ = rotation;
	isMovingCamera = true;
}

void DebugCamera::SetMovingSpeed(Vector3 moveSpeed) {
	movingSpeed_ = moveSpeed;
}