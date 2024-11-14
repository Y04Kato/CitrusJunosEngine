/**
 * @file Player.h
 * @brief プレイヤーが操作を行う自機を管理する
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Player : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	//通常のUpdate
	void Update() override;

	//ユーザー操作や判定のないプレビューだけのUpdate
	void UpdateView();

	//描画
	void Draw(const ViewProjection& viewProjection) override;

	//
	Vector3 CalculateForwardVector();

	//移動操作
	void Move();

	//移動速度減衰
	void MoveAttenuation();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	const WorldTransform& GetWorldTransformPlayer() { return worldTransform2_; }
	void SetWorldTransform(const Vector3 translation);
	void SetRotate(const Vector3 rotate) { worldTransform_.rotation_ = rotate; }

	//落下処理
	void IsFallStart();

	//スケールの設定
	void SetScale(const Vector3 scale) { worldTransform_.scale_ = scale; }

	//座標の設定
	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHitOnFloor;

	//ゲームオーバーか否か
	bool isGameover() { return gameOver; }
	void SetIsGameover(const bool isGameover) { gameOver = isGameover; }

	//当たり判定用のSphereを得る
	StructSphere GetStructSphere() { return structSphere_; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	//移動量を得る、設定する
	Vector3 GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3 velocity);

	//移動可能か否かの設定
	bool GetIsMove() { return isMove_; }
	void SetIsMove(const bool isMove) { isMove_ = isMove; }

	//現在の移動モードの設定
	int GetMoveMode() { return moveMode_; }

	//移動クールダウンの設定
	bool GetIsMoveFlag() { return moveFlag_; }

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.5f;

	Vector3 velocity_ = {};
	Vector3 velocityC_ = {};

	WorldTransform objectPos_;

	WorldTransform worldTransform2_;
	Vector3 rotate_;

	bool gameOver = false;

	StructSphere structSphere_;

	const ViewProjection* viewProjection_ = nullptr;

	//0~2で弱~強
	int moveMode_ = 0;

	float CharacterSpeed_ = 0.5f;
	
	bool moveFlag_ = true;//行動入力受け付けフラグ
	const int moveCountMax_ = 60;//次に行動可能になるまでの時間
	int moveCount_ = 0;//行動入力可能までのタイマー
	const int keyboardAdditionalInputsTimerMax_ = 10;//キーボード時限定、追加入力受付時間

	bool isMove_ = true;//行動可能フラグ
};