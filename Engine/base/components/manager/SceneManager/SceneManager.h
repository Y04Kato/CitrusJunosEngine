/**
 * @file SceneManager.h
 * @brief 各種シーンや操作、管理クラスの生成を一元管理し、フレーム内の更新処理全体を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"
#include "CJEngine.h"
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "postEffect/PostEffect.h"

#include "SceneNumber.h"

//sceneInclude
#include "GameTitleScene.h"
#include "GamePlayScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "DebugScene.h"
#include "TestScene.h"

class SceneManager {
public:
	//エンジンとゲーム一連の処理を全て行う
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

	SceneNumber* sceneNumber_ = nullptr;
};