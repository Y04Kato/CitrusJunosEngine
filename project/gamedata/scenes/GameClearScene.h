#pragma once
#include "Iscene.h"
#include "CJEngine.h"
#include "components/input/Input.h"
#include "TextureManager.h"
#include "components/2d/CreateSprite.h"
#include "components/audio/Audio.h"

class GameClearScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	CitrusJunosEngine* CJEngine_;

	Input* input_ = nullptr;

	TextureManager* textureManager_;

	std::unique_ptr <CreateSprite> sprite_[4];
	EulerTransform spriteTransform_;
	EulerTransform SpriteuvTransform_;
	Vector4 spriteMaterial_;

	float spriteAlpha_ = 256.0f;
	float fadeAlpha_ = 256.0f;

	uint32_t pageAll_;
	uint32_t start_;
	uint32_t game_;

	bool changeAlpha_ = false;

	int count = 0;

	bool pageChange_ = false;

	Audio* audio_;
	SoundData soundData1_;
};