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
    if (isKeyControlCamera_ == true) {
        // マウス感度の調整 (ImGuiで調整)
        ImGui::Begin("DebugCamera");
        ImGui::Checkbox("KeyControlCamera", &isKeyControlCamera_);
        ImGui::DragFloat3("rotation", viewProjection_.rotation_.num, 0.1f);
        ImGui::DragFloat3("translation", viewProjection_.translation_.num, 0.1f);
        ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity_, 0.01f, 0.01f, 10.0f); // マウス感度の調整
        ImGui::End();

        // WASDでの移動を現在の向きに基づいて計算
        Vector3 forward = { 0.0f, 0.0f, 1.0f };  // 前方方向ベクトル
        Vector3 right = { 1.0f, 0.0f, 0.0f };   // 右方向ベクトル

        // 回転行列を使って、進行方向ベクトルを計算
        Matrix4x4 rotationMatrix = MakeRotateMatrix(viewProjection_.rotation_);
        forward = rotationMatrix * forward;
        right = rotationMatrix * right;

        // WASDによる移動
        if (input_->PressKey(DIK_W)) {
            viewProjection_.translation_ += forward * 0.1f; // 前進
        }
        if (input_->PressKey(DIK_S)) {
            viewProjection_.translation_ -= forward * 0.1f; // 後退
        }
        if (input_->PressKey(DIK_D)) {
            viewProjection_.translation_ += right * 0.1f;   // 右移動
        }
        if (input_->PressKey(DIK_A)) {
            viewProjection_.translation_ -= right * 0.1f;   // 左移動
        }

        // SPACEで上昇
        if (input_->PressKey(DIK_SPACE)) {
            viewProjection_.translation_.num[1] += 0.1f; // 上昇
        }

        // LSHIFTで下降
        if (input_->PressKey(DIK_LSHIFT)) {
            viewProjection_.translation_.num[1] -= 0.1f; // 下降
        }

        // マウスで方向転換
        Vector2 mouseMove = input_->GetMousePosition().Velocity;  // マウスの移動量取得
        viewProjection_.rotation_.num[1] += mouseMove.x * mouseSensitivity_; // 横向き回転 (Y軸)
        viewProjection_.rotation_.num[0] += mouseMove.y * mouseSensitivity_; // 縦向き回転 (X軸)

        // X軸（縦方向）の回転制限
        if (viewProjection_.rotation_.num[0] > 90.0f) {
            viewProjection_.rotation_.num[0] = 90.0f;
        }
        else if (viewProjection_.rotation_.num[0] < -90.0f) {
            viewProjection_.rotation_.num[0] = -90.0f;
        }
    }
#endif

    // カメラ移動の処理
    if (isMovingCamera == true) {
        timer_ += timerCountr_;
        movingEndTranslate_.num[0] += movingSpeed_.num[0];
        movingEndTranslate_.num[1] += movingSpeed_.num[1];
        movingEndTranslate_.num[2] += movingSpeed_.num[2];

        if (timer_ >= 1.0f) {
            isMovingCamera = false;
            timer_ = 1.0f;
        }

        // 各回転成分を正規化し、短い回転経路を選択
        float rotationDiffX = NormalizeAngle(movingEndRotate_.num[0] - movingStartRotate_.num[0]);
        float rotationDiffY = NormalizeAngle(movingEndRotate_.num[1] - movingStartRotate_.num[1]);
        float rotationDiffZ = NormalizeAngle(movingEndRotate_.num[2] - movingStartRotate_.num[2]);

        // 回転の内挿
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