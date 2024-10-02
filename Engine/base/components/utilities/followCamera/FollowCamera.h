#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class FollowCamera {
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);
	const ViewProjection& GetViewProjection() { return viewprojection_; }

	void Reset();

	void ApplyGlobalVariables();

	void ShakeCamera(int shakePower, int dividePower);

	//カメラの場所決め
	void SetCamera(Vector3 translation, Vector3 rotation);

private:
	ViewProjection viewprojection_;
	const WorldTransform* target_ = nullptr;
	Input* input_ = nullptr;

	Vector3 interTarget_ = {};

	float destinationAngleY_ = 0.0f;

	float latency = 0.1f;
};