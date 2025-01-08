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
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/metal.mp3");

	//デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//Objects
	model_.reset(Model::CreateModel("project/gamedata/resources/block", "block.obj"));
	model_->SetDirectionalLightFlag(true, 3);
	ObjModelData_ = model_->LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	//Editors
	editors_ = Editors::GetInstance();
	editors_->AddGroupName((char*)"DebugScene");

	//Lights
	directionalLights_ = DirectionalLights::GetInstance();

	//DataReceipt
	datareceipt_.Initialize(50001);
	datareceipt_.start();
}

void DebugScene::Update() {
	ImGui::Begin("DebugOperate");
	if (ImGui::Button("TitleScene")) {
		SceneEndProcessing();
		sceneNumber_->SetSceneNumber(TITLE_SCENE);
	}
	if (ImGui::Button("GameClearScene")) {
		SceneEndProcessing();
		sceneNumber_->SetSceneNumber(CLEAR_SCENE);
	}
	if (ImGui::Button("GameOverScene")) {
		SceneEndProcessing();
		sceneNumber_->SetSceneNumber(OVER_SCENE);
	}
	ImGui::Text("CameraReset:Q key");
	ImGui::End();

	//Edirots更新
	editors_->Update();

	//Camera更新
	debugCamera_->Update();

	//Light更新
	directionalLights_->SetTarget(directionalLight_);

	//ビュープロジェクション更新
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

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
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	audio_->SoundUnload(&soundData1_);

	editors_->Finalize();

	datareceipt_.stop();
}

void DebugScene::ResetProcessing() {
	debugCamera_->SetCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f });

	editors_->SetModels(ObjModelData_, ObjTexture_);
	editors_->SetGroupName((char*)"DebugScene");

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.5f };
}

void DebugScene::SceneEndProcessing() {
	isReset = true;
	directionalLight_.intensity = 1.0f;

	editors_->Finalize();
}