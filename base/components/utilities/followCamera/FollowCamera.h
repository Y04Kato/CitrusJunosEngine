#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class FollowCamera {
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const ViewProjection& GetViewProjection() { return viewprojection_; }

private:
	ViewProjection viewprojection_;
	const WorldTransform* target_ = nullptr;
	Input* input_ = nullptr;

};
