#include"SceneManager.h"

void SceneManager::Run() {
	Initialize();
	Update();
	Finalize();
}

void SceneManager::Initialize() {
	//WindowTitle
	const wchar_t kWindowTitle[] = { L"CG2_カトウ" };
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

	//TextureManager
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//CSV
	GlobalVariables::GetInstance()->LoadFiles();

	//Scene
	scene_[TITLE_SCENE] = std::make_unique<GameTitleScene>();
	scene_[GAME_SCENE] = std::make_unique<GamePlayScene>();
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Initialize();
	}

	//タイトルシーンから開始
	Iscene::sceneNo = TITLE_SCENE;
}


void SceneManager::Update() {
	while (true) {
		//windowのメッセージを最優先で処理させる
		if (WinApp::GetInstance()->Procesmessage()) {
			break;
		}

		CJEngine_->BeginFrame();
		input_->Update();
		GlobalVariables::GetInstance()->Update();
		directionalLight_->Update();
		scene_[Iscene::sceneNo]->Update();
		scene_[Iscene::sceneNo]->Draw();
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
	for (int i = 0; i < SCENE_MAX; i++) {
		scene_[i]->Finalize();
	}
	CoUninitialize();
}