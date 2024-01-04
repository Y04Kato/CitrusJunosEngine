#include "GamePlayScene.h"
#include "components/utilities/globalVariables/GlobalVariables.h"

void GamePlayScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoadWave("project/gamedata/resources/conjurer.wav");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, false);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();

	ground_ = std::make_unique<Ground>();
	model_.reset(Model::CreateModelFromObj("project/gamedata/resources/floor", "Floor.obj"));
	ground_->Initialize(model_.get(), { 0.0f,0.0f,-5.0f }, { 30.0f,1.0f,30.0f });

	//CollisionManager
	collisionManager_ = CollisionManager::GetInstance();

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
}

void GamePlayScene::Update() {
	ApplyGlobalVariables();

	Obb_.center = ground_->GetWorldTransform().GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(ground_->GetWorldTransform().rotation_), Obb_.orientation);
	Obb_.size = ground_->GetWorldTransform().scale_;

	ground_->Update();

	collisionManager_->ClearColliders();
	collisionManager_->CheckAllCollision();

	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

}

void GamePlayScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->PreDraw2D();

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->PreDraw3D();

	ground_->Draw(viewProjection_);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->PreDrawParticle();

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();

#pragma endregion
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}