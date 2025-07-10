/**
 * @file GameTitleScene.h
 * @brief ゲームタイトルのシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

class GameTitleScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:

};