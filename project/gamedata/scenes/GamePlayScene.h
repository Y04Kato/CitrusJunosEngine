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

#include "ground/Ground.h"
#include "baseCharacter/player/Player.h"
#include "baseCharacter/enemy/Enemy.h"

class GamePlayScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawPostEffect() override;
	void Finalize() override;

	void ApplyGlobalVariables();

	void SetEnemy(Vector3 pos);

private:
	CitrusJunosEngine* CJEngine_;
	DirectXCommon* dxCommon_;
	ViewProjection viewProjection_;
	TextureManager* textureManager_;

	Audio* audio_;
	SoundData soundData1_;
	SoundData soundData2_;

	Input* input_;

	DebugCamera* debugCamera_;

	CollisionManager* collisionManager_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;

	std::unique_ptr<Ground>ground_;
	std::unique_ptr<Model> groundModel_;
	OBB Obb_;

	std::unique_ptr<CreateParticle> particle_;
	Emitter testEmitter_{};
	AccelerationField accelerationField;
	uint32_t particleResourceNum_;

	std::list<Enemy*> enemys_;
	std::unique_ptr<Model> enemyModel_;

	std::unique_ptr <CreateSprite> sprite_[5];
	EulerTransform spriteTransform_;
	EulerTransform SpriteuvTransform_;
	Vector4 spriteMaterial_;

	uint32_t background_;
	uint32_t move1_;
	uint32_t move2_;
	uint32_t move3_;

	int enemyDethCount = 0;

	bool gameStart = true;

	Vector4 test = { 1.0f,1.0f,1.0f,1.0f };

	float fadeAlpha_ = 256.0f;
	bool isfadeIn = false;

	bool gameover = false;
	bool gameclear = false;

	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	PointLights* pointLights_;
	PointLight pointLight_;
};
