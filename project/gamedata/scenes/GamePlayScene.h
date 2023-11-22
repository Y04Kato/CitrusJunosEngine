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

class GamePlayScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void ApplyGlobalVariables();

private:
	CitrusJunosEngine* CJEngine_;
	DirectXCommon* dxCommon_;
	ViewProjection viewProjection_;
	TextureManager* textureManager_;

	int blendCount_;

	std::unique_ptr <CreateTriangle> triangle_[2];
	WorldTransform worldTransformTriangle_[2];
	Vector4 triangleMaterial_[2];

	std::unique_ptr <CreateSprite> sprite_[2];
	Transform spriteTransform_[2];
	Transform SpriteuvTransform_[2];
	Vector4 spriteMaterial_[2];

	std::unique_ptr<CreateParticle> particle_;
	Emitter testEmitter_{};
	AccelerationField accelerationField;

	std::unique_ptr <CreateSphere> sphere_;
	WorldTransform worldTransformSphere_;
	Vector4 sphereMaterial_;

	std::unique_ptr<Model> model_;
	WorldTransform worldTransformModel_;
	Vector4 modelMaterial_;

	uint32_t uvResourceNum_;
	uint32_t monsterBallResourceNum_;
	uint32_t particleResourceNum_;

	Audio* audio_;
	SoundData soundData1_;

	Input* input_;

	DebugCamera* debugCamera_;

	CollisionManager* collisionManager_;

	int texture_;

	bool isTriangleDraw_[2];
	bool isSphereDraw_;
	bool isSpriteDraw_[2];
	bool isModelDraw_;
	bool isParticleDraw_;
};
