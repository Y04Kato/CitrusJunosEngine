/**
 * @file DebugScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"
#include "CJEngine.h"

class DebugScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:
	CitrusJunosEngine* CJEngine_;
	
};