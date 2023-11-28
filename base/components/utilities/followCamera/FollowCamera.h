#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class LockOn;
class FollowCamera {
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);
	const ViewProjection& GetViewProjection() { return viewprojection_; }
	Vector3 GetTargetWordPos();

	void Reset();

	void ApplyGlobalVariables();

	void SetlockOn(const LockOn* lockon) { lockOn_ = lockon; }
	void DeletelockOn() { lockOn_ = nullptr; }

private:
	ViewProjection viewprojection_;
	const WorldTransform* target_ = nullptr;
	Input* input_ = nullptr;
	
	const LockOn* lockOn_ = nullptr;

	Vector3 interTarget_ = {};

	float destinationAngleY_ = 0.0f;

	float latency = 0.1f;
};
