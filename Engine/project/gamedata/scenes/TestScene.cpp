/**
 * @file TestScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "TestScene.h"

void TestScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//TextureManager
	textureManager_ = TextureManager::GetInstance();

	//SceneNumber
	sceneNumber_ = SceneNumber::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();

	//デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//Objects
	model_.reset(Model::CreateModel("project/gamedata/resources/block", "block.obj"));
	model_->SetDirectionalLightFlag(true, 3);

	//Lights
	directionalLights_ = DirectionalLights::GetInstance();
}

void TestScene::Update() {
	ImGui::Begin("DebugOperate");
	if (ImGui::Button("TitleScene")) {
		SceneEndProcessing();
		sceneNumber_->SetSceneNumber(TITLE_SCENE);
	}
	ImGui::Text("CameraReset:Q key");
	ImGui::End();

	//Camera更新
	debugCamera_->Update();

	//Light更新
	directionalLights_->SetTarget(directionalLight_);

	//ビュープロジェクション更新
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	//カメラリセット
	if (input_->TriggerKey(DIK_Q)) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
	}
}

void TestScene::Draw() {
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

void TestScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void TestScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void TestScene::Finalize() {

}

void TestScene::ResetProcessing() {
	debugCamera_->SetCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f });

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.5f };
}

void TestScene::SceneEndProcessing() {
	isReset = true;
	directionalLight_.intensity = 1.0f;
}