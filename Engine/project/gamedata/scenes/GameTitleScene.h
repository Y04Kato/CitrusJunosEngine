#pragma once
#include "Iscene.h"
#include "CJEngine.h"
#include "Input.h"

class GameTitleScene :public Iscene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:
	CitrusJunosEngine* CJEngine_;

	Input* input_ = nullptr;
};
