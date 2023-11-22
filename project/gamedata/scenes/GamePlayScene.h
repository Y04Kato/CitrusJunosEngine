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
#include "components/utilities/followCamera/FollowCamera.h"

#include "baseCharacter/BaseCharacter.h"
#include "baseCharacter/player/Player.h"
#include "baseCharacter/enemy/Enemy.h"
#include "skydome/Skydome.h"
#include "ground/GroundManager.h"
#include "goal/Goal.h"

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

	Audio* audio_;
	SoundData soundData1_;

	Input* input_;

	DebugCamera* debugCamera_;

	std::unique_ptr<CollisionManager> collisionManager_;

	std::unique_ptr<Skydome> skyDome_ = nullptr;
	std::unique_ptr<Model> skyDomeModel_ = nullptr;
	std::unique_ptr<Model> playerModel_ = nullptr;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModelBody_ = nullptr;
	std::unique_ptr<Model> playerModelHead_ = nullptr;
	std::unique_ptr<Model> playerModelL_Arm_ = nullptr;
	std::unique_ptr<Model> playerModelR_Arm_ = nullptr;
	std::unique_ptr<Model> playerModelWeapon_ = nullptr;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<GroundManager> groundmanager_;
	std::unique_ptr<Goal> goal_;

	std::unique_ptr<Enemy>enemy_;
	std::unique_ptr<Model> enemyModelHead = nullptr;
	std::unique_ptr<Model> enemyModelBody = nullptr;
	std::unique_ptr<Model> enemyModelL_arm = nullptr;
	std::unique_ptr<Model> enemyModelR_arm = nullptr;
	std::unique_ptr<Model> enemyModelWeapon_ = nullptr;

	int count_;

	std::unique_ptr<CreateParticle> particle_;
	Emitter testEmitter_{};
	AccelerationField accelerationField;

	uint32_t particleResourceNum_;
};
