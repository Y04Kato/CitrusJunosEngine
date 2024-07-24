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
#include "postEffect/PostEffect.h"
#include "editors/Editors.h"

#include "ground/Ground.h"
#include "baseCharacter/player/Player.h"
#include "baseCharacter/enemy/Enemy.h"

class GamePlayScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	void ApplyGlobalVariables();

	//ゲーム開始時の処理
	void GameStartProcessing();

	//当たり判定まとめ
	void CollisionConclusion();

	//敵の配置
	void SetEnemy(Vector3 pos);
	//ランダムな座標を返す
	Vector3 GenerateRandomPosition();
	//他オブジェクトに接触しているか確認
	bool IsValidPosition(const Vector3 pos);
	//指定した回数、座標が問題ないか確認し座標を返す
	Vector3 FindValidPosition();

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

	//Player
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;

	std::unique_ptr<CreateParticle> playerParticle_;
	Emitter playerEmitter_{};
	AccelerationField playerAccelerationField_;
	uint32_t playerParticleResource_;

	//Enemy
	std::list<Enemy*> enemys_;
	std::unique_ptr<Model> enemyModel_;
	const int enemyMaxCount_ = 20;//エネミーの最大発生数
	int enemyAliveCount_ = 0;//エネミーの生存数

	//Ground
	std::unique_ptr<Ground>ground_;
	std::unique_ptr<Model> groundModel_;
	OBB groundObb_;

	//Flag
	std::unique_ptr<Model> flagModel_[5];
	WorldTransform world_[5];
	float playerFlagRotate_ = 30.0f;//Playerに付随しているFlagの回転

	//Editor
	Editors* editors_;
	bool isEditorMode_ = false;

	//Objects
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
	std::list<Obj> objects_;

	//CollisionParticle
	std::unique_ptr<CreateParticle> collisionParticle_;
	Emitter collisionEmitter_{};
	AccelerationField collisionAccelerationField_;
	uint32_t collisionParticleResource_;
	const int collisionParticleOccursNum_ = 20;

	//Sprite
	std::unique_ptr <CreateSprite> sprite_[5];
	EulerTransform spriteTransform_;
	EulerTransform SpriteuvTransform_;
	Vector4 spriteMaterial_;

	//Texture
	uint32_t background_;
	uint32_t move1_;
	uint32_t move2_;
	uint32_t move3_;

	//Fade
	float fadeAlpha_ = 256.0f;
	bool isfadeIn_ = false;

	//Other
	bool isGameStart_ = true;//ゲームスタート時の処理
	bool isGameover_ = false;//ゲームオーバー時の処理
	bool isGameclear_ = false;//ゲームクリア時の処理

	//押し戻しの倍率
	float pushbackMultiplier_ = 2.0f;
	float pushbackMultiplierObj_ = 1.5f;

	//反発係数
	float repulsionCoefficient_ = 0.8f;

	//Lights
	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	PointLights* pointLights_;
	PointLight pointLight_;

	//PostEffect
	PostEffect* postEffect_;
	uint32_t noiseTexture_;
	MaskData maskData_;
};
