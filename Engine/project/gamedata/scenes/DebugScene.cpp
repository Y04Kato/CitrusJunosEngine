/**
 * @file DebugScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "DebugScene.h"

#include "GlobalVariablesGroup.h"

void DebugScene::Initialize() {
	Iscene::Initialize();
	//DataReceipt
	datareceipt_.Initialize(50001);
	datareceipt_.start();

	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("DebugScene");
	gvg->SetValue("Test", &testData_);
}

void DebugScene::Update() {
	Iscene::Update();
	ImGui::Begin("DebugOperate");
	ImGui::Text("CameraReset:Q key");
	ImGui::End();

	//DataReceipt
	datareceipt_.receiveMessage();

	//カメラリセット
	if (input_->TriggerKey(DIK_Q)) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,0.0f,-20.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
	}
}

void DebugScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);
	//Editors
	editors_->Draw(viewProjection_);

	datareceipt_.Draw(viewProjection_);

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
	Iscene::DrawPostEffect();
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	Iscene::Finalize();
	datareceipt_.stop();
}