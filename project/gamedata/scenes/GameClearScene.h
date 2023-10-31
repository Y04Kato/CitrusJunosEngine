#pragma once
#include "components/manager/Iscene.h"

class GameClearScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
};