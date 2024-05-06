#pragma once
#include "Iscene.h"
#include "CJEngine.h"
#include "Input.h"
#include "Audio.h"
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "postEffect/PostEffect.h"

//sceneInclude
#include "GameTitleScene.h"
#include "GamePlayScene.h"
#include "GameClearScene.h"
#include "GameDemoScene.h"

class SceneManager {
public:
	void Run();

	void Initialize();
	void Update();
	//void Draw();
	void Finalize();

private:
	CitrusJunosEngine* CJEngine_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	DirectionalLights* directionalLight_ = nullptr;
	PointLights* pointLight_ = nullptr;

	ImGuiManager* imGuiManager_ = nullptr;

	std::unique_ptr<Iscene>scene_[SCENE_MAX];

	PostEffect* postEffect_ = nullptr;
};