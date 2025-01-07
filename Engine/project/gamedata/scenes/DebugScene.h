/**
 * @file DebugScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "dataReceipt/DataReceipt.h"

class DebugScene :public Iscene {
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
	SoundData soundData1_;

	//Camera
	DebugCamera* debugCamera_;

	//Objects
	std::unique_ptr<Model> model_;
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
	std::list<Obj> objects_;

	//Editor
	Editors* editors_;
	bool isEditorMode_ = false;

	//リセット処理フラグ
	bool isReset = true;

	//Lights
	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	//DataReceipt
	DataReceipt datareceipt_;
};