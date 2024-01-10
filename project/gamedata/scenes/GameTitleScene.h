#pragma once
#include "components/manager/Iscene.h"
#include "CJEngine.h"
#include "components/input/Input.h"

class GameTitleScene :public Iscene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	CitrusJunosEngine* CJEngine_;

	Input* input_ = nullptr;

	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);

	Quaternion interpolate0 = Slerp(0.0f, rotation0, rotation1);
	Quaternion interpolate1 = Slerp(0.3f, rotation0, rotation1);
	Quaternion interpolate2 = Slerp(0.5f, rotation0, rotation1);
	Quaternion interpolate3 = Slerp(0.7f, rotation0, rotation1);
	Quaternion interpolate4 = Slerp(1.0f, rotation0, rotation1);
};
