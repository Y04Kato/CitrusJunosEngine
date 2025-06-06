/**
 * @file DebugCamera.h
 * @brief カメラ及び視点関係の初期化及び管理を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "ViewProjection.h"
#include "Input.h"

class DebugCamera {
public:
	static DebugCamera* GetInstance();

	void initialize();

	void Update();

	//カメラを指定した強さでシェイクする
	void ShakeCamera(int shakePower, int dividePower);

	//カメラの場所決め
	void SetCamera(Vector3 translation, Vector3 rotation);

	//カメラを移動させる
	void MovingCamera(Vector3 translation, Vector3 rotation, float timerSpeed);

	//目標が動いている場合使用
	void SetMovingSpeed(Vector3 moveSpeed);

	ViewProjection* GetViewProjection() { return &viewProjection_; }

private:
	ViewProjection viewProjection_;

	Input* input_;

	bool isMovingCamera = false;
	Vector3 movingStartTranslate_;
	Vector3 movingStartRotate_;
	Vector3 movingEndTranslate_;
	Vector3 movingEndRotate_;
	float timer_ = 0.0f;
	float timerCountr_ = 0.01f;
	Vector3 movingSpeed_ = { 0.0f,0.0f,0.0f };

	//キーでカメラを操作するか否かのフラグ
	bool isKeyControlCamera_ = false;
	float moveSpeed_ = 0.3f;//カメラの移動速度
	float mouseSensitivity_ = 0.005f;//マウス感度
	bool prevTabKeyState_ = false;//マウス操作するか否か
};