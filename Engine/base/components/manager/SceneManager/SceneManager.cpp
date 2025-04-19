/**
 * @file SceneManager.cpp
 * @brief 各種シーンや操作、管理クラスの生成を一元管理し、フレーム内の更新処理全体を行う
 * @author KATO
 * @date 未記録
 */

#include"SceneManager.h"

void SceneManager::Run() {
	Initialize();
	Update();
	Finalize();
}

void SceneManager::Initialize() {
	//WindowTitle
	const char kWindowTitle[] = "独楽弾き";
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//Engine
	CJEngine_ = CitrusJunosEngine::GetInstance();
	CJEngine_->Initialize(kWindowTitle, 1280, 720);
	//DirectX
	dxCommon_ = DirectXCommon::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Light
	directionalLight_ = DirectionalLights::GetInstance();
	directionalLight_->Initialize();
	pointLight_ = PointLights::GetInstance();
	pointLight_->Initialize();

	//TextureManager
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//ImGui
	imGuiManager_ = ImGuiManager::GetInstance();
	imGuiManager_->Initialize(WinApp::GetInstance(), dxCommon_);

	//CSV
	GlobalVariables::GetInstance()->LoadFiles();

	//Scene
	scene_[TITLE_SCENE] = std::make_unique<GameTitleScene>();
	scene_[GAME_SCENE] = std::make_unique<GamePlayScene>();
	scene_[CLEAR_SCENE] = std::make_unique<GameClearScene>();
	scene_[OVER_SCENE] = std::make_unique<GameOverScene>();
	scene_[DEBUG_SCENE] = std::make_unique<DebugScene>();
	scene_[TEST_SCENE] = std::make_unique<TestScene>();
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Initialize();
	}

	//タイトルシーンから開始
	sceneNumber_ = SceneNumber::GetInstance();
	sceneNumber_->Initialize(TITLE_SCENE);

	postEffect_ = PostEffect::GetInstance();
	postEffect_->Initialize();
	postEffect_->ALLCreate();
}


void SceneManager::Update() {
	while (true) {
		//windowのメッセージを最優先で処理させる
		if (WinApp::GetInstance()->Procesmessage()) {
			break;
		}
		postEffect_->PreDraw();

		imGuiManager_->Begin();
		CJEngine_->Update();
		GlobalVariables::GetInstance()->Update();
		directionalLight_->Update();
		pointLight_->Update();
		scene_[sceneNumber_->GetSceneNumber()]->Update();
		scene_[sceneNumber_->GetSceneNumber()]->Draw();
		imGuiManager_->End();

		CJEngine_->BeginFrame();
		postEffect_->PreCopy();

		scene_[sceneNumber_->GetSceneNumber()]->DrawPostEffect();

		postEffect_->Draw();

		scene_[sceneNumber_->GetSceneNumber()]->DrawUI();

		imGuiManager_->Draw();

		postEffect_->PostCopy();

		CJEngine_->EndFrame();

		//// ESCキーが押されたらループを抜ける
		if (input_->TriggerKey(DIK_ESCAPE)) {
			break;
		}
	}
}

//void SceneManager::Draw() {
//
//}

void SceneManager::Finalize() {
	CJEngine_->Finalize();
	imGuiManager_->Finalize();
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Finalize();
	}
	CoUninitialize();
}