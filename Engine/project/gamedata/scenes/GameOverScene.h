/**
 * @file GameOverScene.h
 * @brief ゲームオーバーシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "baseCharacter/player/Player.h"
#include "transition/Transition.h"

class GameOverScene :public Iscene {
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

	std::unique_ptr <CreateSprite> uiSprite_[3];
	EulerTransform uiSpriteTransform_;
	EulerTransform uiSpriteuvTransform_;
	Vector4 uiSpriteMaterial_;

	float spriteAlpha_ = 256.0f;
	const float fadeSpeed_ = 8.0f;//フェードスピード

	uint32_t pageAll_;
	uint32_t start_;
	uint32_t game_;

	bool changeAlpha_ = false;

	int sceneCount_ = 0;

	Audio* audio_;
	SoundData soundData1_;

	//自機
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;

	//ViewProjection
	ViewProjection viewProjection_;

	//Camera
	DebugCamera* debugCamera_;

	//シーン遷移
	Transition* transition_;

	//Other
	bool isGameStart_ = true;//ゲームスタート時の処理
	const int sceneChangeMaxTime_ = 120;
	int sceneChangeTimer_ = 0;
};
