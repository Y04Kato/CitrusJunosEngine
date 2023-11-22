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

	Vector3 axis_ = Normalize(Vector3{ 1.0f,1.0f,1.0f });
	float angle_ = 0.44f;
};
