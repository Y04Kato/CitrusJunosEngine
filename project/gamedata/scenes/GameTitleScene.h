#pragma once
#include "components/manager/Iscene.h"
#include "components/audio/Audio.h"
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "components/manager/TextureManager.h"
#include "components/2d/CreateTriangle.h"
#include "components/2d/CreateSprite.h"
#include "components/2d/CreateParticle.h"
#include "components/3d/CreateSphere.h"
#include "components/3d/Model.h"
#include "components/debugcamera/DebugCamera.h"
#include "components/utilities/collisionManager/CollisionManager.h"
#include "components/utilities/collisionManager/CollisionConfig.h"

#include "baseCharacter/player/Player.h"

class GameTitleScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	CitrusJunosEngine* CJEngine_;

	Input* input_ = nullptr;

	TextureManager* textureManager_;

	std::unique_ptr <CreateSprite> sprite_[5];
	EulerTransform spriteTransform_;
	EulerTransform SpriteuvTransform_;
	Vector4 spriteMaterial_;

	float spriteAlpha_ = 256.0f;
	float fadeAlpha_ = 0.0f;

	uint32_t pageAll_;
	uint32_t start_;
	uint32_t title_;
	uint32_t tutorial_;

	bool changeAlpha_ = false;

	int count = 0;

	bool pageChange_ = false;

	Audio* audio_;
	SoundData soundData1_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;
	ViewProjection viewProjection_;

	DebugCamera* debugCamera_;
};