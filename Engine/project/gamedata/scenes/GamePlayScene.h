/**
 * @file GamePlayScene.h
 * @brief ゲームシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "ground/Ground.h"
#include "baseCharacter/player/Player.h"
#include "baseCharacter/enemy/Enemy.h"
#include "baseCharacter/boss/Boss.h"
#include "explosion/Explosion.h"
#include "transition/Transition.h"

class GamePlayScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	//ゲーム開始時の処理
	void GameStartProcessing();

	//ゲーム開始時の演出処理
	void GameEntryProcessing();

	//ゲーム中の処理
	void GameProcessing();

	//ポーズ中の処理
	void GamePauseProcessing();

	//ゲームクリア時の処理
	void GameClearProcessing();

	//ゲームオーバー時の処理
	void GameOverProcessing();

	//ステージのリセット処理
	void StageReset();

	//当たり判定まとめ
	void CollisionConclusion();

	//接触時の音量計算
	void ContactVolume(Vector3 velocity);

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

	//SceneNo
	SceneNumber* sceneNumber_;

	Audio* audio_;
	SoundData soundData1_;
	SoundData soundData2_;

	Input* input_;

	//各種カメラ
	DebugCamera* debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	int cameraChangeTimer_ = 0;//カメラ切り替えの時間
	int startCameraChangeTimer_ = 0;//スタート演出カメラ切り替えの時間
	const int cameraChangeMaxTimer_ = 35;//カメラ切り替えの終了時間
	const float cameraMoveSpeed_ = 0.05f;//MoveCameraの移動速度
	bool cameraChange_ = false;//カメラの切り替えが終わっているか否か
	bool isBirdseyeMode_ = false;//俯瞰視点か否か

	//Player
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;
	//PlayerParticle
	std::unique_ptr<CreateParticle> playerParticle_;
	Emitter playerEmitter_{};
	AccelerationField playerAccelerationField_;
	uint32_t playerParticleResource_;

	//Boss
	std::unique_ptr<Boss> boss_;
	std::unique_ptr<Model> bossModel_;
	const float bodyBreakSpeed_ = 0.6f;//衝突時の壊れる為に必要な速度

	//Enemy
	std::list<Enemy*> enemys_;
	std::unique_ptr<Model> enemyModel_;
	const int enemyMaxCount_ = 20;//エネミーの最大発生数
	int enemyAliveCount_ = 0;//エネミーの生存数

	//Ground
	std::unique_ptr<Ground>ground_;
	std::unique_ptr<Model> groundModel_;
	OBB groundObb_;

	//SkyBox
	std::unique_ptr <CreateSkyBox> skyBox_;
	WorldTransform worldTransformSkyBox_;
	Vector4 skyBoxMaterial_;

	//Flag
	static const int flagCount_ = 4;
	std::unique_ptr<Model> flagModel_[flagCount_];
	WorldTransform flagWorldTransforms_[flagCount_];

	//背景モデル
	static const int modelMaxCount_ = 4;
	std::unique_ptr<Model> bgModel_[modelMaxCount_];
	WorldTransform worldTransformBGModel_[modelMaxCount_];
	Vector4 bgModelMaterial_[modelMaxCount_];

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
	const int collisionParticleOccursNum_ = 10;

	//Sprite
	std::unique_ptr <CreateSprite> uiSprite_[6];
	EulerTransform uiSpriteTransform_;
	EulerTransform uiSpriteuvTransform_;
	Vector4 uiSpriteMaterial_;
	EulerTransform uiSpriteTransform4_;//sprite_[4]のみゲーム開始演出用で動く為
	const float magnificationPower_ = 0.02f;//sprite_[4]の拡大率

	//Texture
	uint32_t background_;
	uint32_t move1_;
	uint32_t move2_;
	uint32_t move3_;
	uint32_t skyboxTex_;
	uint32_t purpose_;
	uint32_t pause_;

	//Other
	bool isGameStart_ = true;//ゲームスタート時のフラグ
	bool isGameEntry_ = false;//ゲームスタート演出のフラグ
	int entryCount_ = 0;//スタート演出のカウント
	const float startCameraChangeTime_ = 120.0f;//スタート演出をいつまでやるか
	bool isGameover_ = false;//ゲームオーバー時のフラグ
	bool isGameclear_ = false;//ゲームクリア時のフラグ
	bool isGameEnd_ = false;//ゲームリセット時のフラグ
	bool isGamePause_ = false;//ゲームポーズフラグ

	//押し戻しの倍率
	float pushbackMultiplier_ = 2.0f;
	float pushbackMultiplierObj_ = 1.5f;

	//反発係数
	float repulsionCoefficient_ = 1.2f;

	//Lights
	DirectionalLights* directionalLights_;
	DirectionalLight directionalLight_;

	PointLights* pointLights_;
	PointLight pointLight_;

	//PostEffect
	PostEffect* postEffect_;
	uint32_t noiseTexture_;
	MaskData maskData_;
	const float maskThresholdSpeed_ = 0.02f;

	//Explosion
	Explosion* explosion_;
	const int explosionMaxTimer_ = 30;
	int explosionTimer_ = 0;
	bool isExplosion_ = false;

	//シーン遷移
	Transition* transition_;

	//再帰回数
	const int maxAttempts_= 100;
};
