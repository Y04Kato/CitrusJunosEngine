#pragma once
#include "ViewProjection.h"
#include "Input.h"

class DebugCamera{
public:
	static DebugCamera* GetInstance();

	void initialize();

	void Update();

	void ShakeCamera(int shakePower,int dividePower);

	void SetCamera(Vector3 translation, Vector3 rotation);

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
};
