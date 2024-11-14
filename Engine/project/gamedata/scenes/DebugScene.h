/**
 * @file DebugScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "ground/Ground.h"
#include "baseCharacter/player/Player.h"
#include "baseCharacter/enemy/Enemy.h"

class DebugScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	//リセット処理
	void ResetProcessing();

	//シーン終了時処理
	void SceneEndProcessing();

	//当たり判定まとめ
	void CollisionConclusion();

	//敵の配置
	void SetEnemy(Vector3 pos, Vector3 velocity);

private:
	CitrusJunosEngine* CJEngine_;
	ViewProjection viewProjection_;
	TextureManager* textureManager_;

	Input* input_ = nullptr;

	Audio* audio_;
	SoundData soundData1_;

	DebugCamera* debugCamera_;

	//Player
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;

	//Enemy
	std::list<Enemy*> enemys_;
	std::unique_ptr<Model> enemyModel_;
	const int enemyMaxCount_ = 20;//エネミーの最大発生数

	//Ground
	std::unique_ptr<Ground>ground_;
	std::unique_ptr<Model> groundModel_;
	OBB groundObb_;

	//Objects
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
	std::list<Obj> objects_;

	//Editor
	Editors* editors_;
	bool isEditorMode_ = false;

	//リセット処理フラグ
	bool isReset = true;

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
};