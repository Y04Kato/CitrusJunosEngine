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
	void DrawUI() override;
	void DrawPostEffect() override;
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

	WorldTransform world_[3];
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;
	ViewProjection viewProjection_;

	std::unique_ptr<Model> stage_[2];
	float testTimer_ = 1.0f;

	int cTimer_ = 0;
	bool cFlag = true;

	DebugCamera* debugCamera_;

	std::unique_ptr<CreateParticle> particle_;
	Emitter testEmitter_{};
	AccelerationField accelerationField;
	uint32_t particleResourceNum_;

	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	PointLights* pointLights_;
	PointLight pointLight_;
};