#include"SceneManager.h"

void SceneManager::Run() {
	Initialize();
	Update();
	Finalize();
}

void SceneManager::Initialize() {
	//WindowTitle
	const char kWindowTitle[] = "title";
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//Engine
	CJEngine_ = CitrusJunosEngine::GetInstance();
	CJEngine_->Initialize(kWindowTitle, 1280, 720);
	//DirectX
	dxCommon_ = DirectXCommon::GetInstance();
	//Audio
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//Input
	input_ = Input::GetInstance();
	input_->Initialize();

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
	scene_[DEMO_SCENE] = std::make_unique<GameDemoScene>();
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Initialize();
	}

	//タイトルシーンから開始
	Iscene::sceneNo = TITLE_SCENE;

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
		input_->Update();
		GlobalVariables::GetInstance()->Update();
		directionalLight_->Update();
		pointLight_->Update();
		scene_[Iscene::sceneNo]->Update();
		scene_[Iscene::sceneNo]->Draw();
		imGuiManager_->End();

		CJEngine_->BeginFrame();
		postEffect_->PreCopy();

		scene_[Iscene::sceneNo]->DrawPostEffect();

		postEffect_->Draw();

		scene_[Iscene::sceneNo]->DrawUI();

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
	audio_->Finalize();
	imGuiManager_->Finalize();
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Finalize();
	}
	CoUninitialize();
}