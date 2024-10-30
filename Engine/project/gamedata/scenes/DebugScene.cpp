/**
 * @file DebugScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "DebugScene.h"

void DebugScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

}

void DebugScene::Update() {
	
}

void DebugScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);


#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	

}