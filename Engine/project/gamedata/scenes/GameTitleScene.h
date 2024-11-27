/**
 * @file GameTitleScene.h
 * @brief ゲームタイトルのシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "baseCharacter/player/Player.h"
#include "transition/Transition.h"

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
	std::unique_ptr <CreateSprite> uiSprite_[4];
	EulerTransform uiSpriteTransform_;
	EulerTransform uiSpriteuvTransform_;
	Vector4 uiSpriteMaterial_;

	//テクスチャ
	uint32_t pageAll_;
	uint32_t start_;
	uint32_t title_;
	uint32_t tutorial_;

	//各種フェード用
	bool changeAlpha_ = false;//アルファ加算か減算か
	float spriteAlpha_ = 256.0f;//UI用フェードアルファ
	const float fadeSpeed_ = 8.0f;//フェードスピード

	//シーン内の演出遷移用カウント
	int sceneCount_ = 0;
	const int maxSceneCount_ = 2;
	const float transitionStartPoint_ = 50.0f;

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
	const float stageAnimationTime_ = 1.0f;//アニメーション時間
	const float stageAnimationStartTime_ = 1.0f;//アニメーション開始時間
	float stageAnimationTimer_ = 1.0f;//アニメーション用のタイマー

	//背景モデル
	static const int modelMaxCount_ = 4;
	std::unique_ptr<Model> bgModel_[modelMaxCount_];
	WorldTransform worldTransformBGModel_[modelMaxCount_];
	Vector4 bgModelMaterial_[modelMaxCount_];
	ModelData posterModelData_;
	uint32_t posterTexture_;

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