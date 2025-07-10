/**
 * @file Iscene.cpp
 * @brief 各種シーンをまとめる
 * @author KATO
 * @date 未記録
 */

#include "Iscene.h"

void Iscene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();

	//TextureManager
	textureManager_ = TextureManager::GetInstance();

	//Lights
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	//デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//SceneNumber
	sceneNumber_ = SceneNumber::GetInstance();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//Editors
	editors_ = Editors::GetInstance();
	editors_->Initialize();
}

void Iscene::Update() {
	//Edirots更新
	editors_->Update();

	//Camera更新
	debugCamera_->Update();

	//ビュープロジェクション更新
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();
}

void Iscene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void Iscene::Finalize() {
	editors_->Finalize();
}