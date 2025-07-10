/**
 * @file GameOverScene.h
 * @brief ゲームオーバーシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

class GameOverScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:

};
