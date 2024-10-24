/**
 * @file GameTitleScene.h
 * @brief ゲームタイトルのシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"
#include "components/audio/Audio.h"
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "TextureManager.h"
#include "components/2d/CreateTriangle.h"
#include "components/2d/CreateSprite.h"
#include "components/2d/CreateParticle.h"
#include "components/3d/CreateSphere.h"
#include "components/3d/CreateSkyBox.h"
#include "components/3d/Model.h"
#include "components/debugcamera/DebugCamera.h"
#include "components/utilities/collisionManager/CollisionManager.h"
#include "components/utilities/collisionManager/CollisionConfig.h"

#include "baseCharacter/player/Player.h"
#include "transition/transition.h"

class GameTitleScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	//ゲーム開始時の処理
	void GameStartProcessing();

private:
	CitrusJunosEngine* CJEngine_;

	Input* input_ = nullptr;

	TextureManager* textureManager_;

	//Audio
	Audio* audio_;
	SoundData soundData1_;

	//Sprite
	std::unique_ptr <CreateSprite> sprite_[4];
	EulerTransform spriteTransform_;
	EulerTransform SpriteuvTransform_;
	Vector4 spriteMaterial_;

	//テクスチャ
	uint32_t pageAll_;
	uint32_t start_;
	uint32_t title_;
	uint32_t tutorial_;

	//各種フェード用
	bool changeAlpha_ = false;//アルファ加算か減算か
	float spriteAlpha_ = 256.0f;//UI用フェードアルファ

	//シーン内の演出遷移用カウント
	int sceneCount_ = 0;

	//シーン遷移
	Transition* transition_;

	//ViewProjection
	ViewProjection viewProjection_;

	//自機とモデル座標
	WorldTransform worldModels_[3];

	//自機
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;

	//ステージモデル
	std::unique_ptr<Model> stage_[2];
	float stageAnimationTimer_ = 1.0f;//アニメーション用のタイマー

	//
	static const int modelMaxCount_ = 4;
	std::unique_ptr<Model> model_[modelMaxCount_];
	WorldTransform worldTransformModel_[modelMaxCount_];
	Vector4 modelMaterial_[modelMaxCount_];
	ModelData posterModelData_;
	uint32_t posterTexture1_;

	//SkyBox
	std::unique_ptr <CreateSkyBox> skyBox_;
	WorldTransform worldTransformSkyBox_;
	Vector4 skyBoxMaterial_;
	uint32_t skyboxTex_;

	//Camera
	DebugCamera* debugCamera_;

	//Particle
	std::unique_ptr<CreateParticle> particle_;
	Emitter particleEmitter_{};
	AccelerationField accelerationField;
	uint32_t particleResourceNum_;

	//Other
	bool isGameStart_ = false;//ゲームスタート時の処理

	//ライト
	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	PointLights* pointLights_;
	PointLight pointLight_;
};