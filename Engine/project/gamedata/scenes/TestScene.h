/**
 * @file TestScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "field/Field.h"

class TestScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	//リセット処理
	void ResetProcessing();

	//シーン終了時処理
	void SceneEndProcessing();

private:
	CitrusJunosEngine* CJEngine_;
	ViewProjection viewProjection_;
	TextureManager* textureManager_;

	//SceneNo管理
	SceneNumber* sceneNumber_;

	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_;

	//Camera
	DebugCamera* debugCamera_;

	//Objects
	std::unique_ptr<Model> model_;

	//リセット処理フラグ
	bool isReset = true;

	//Lights
	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	//Field
	Field* field_;
};