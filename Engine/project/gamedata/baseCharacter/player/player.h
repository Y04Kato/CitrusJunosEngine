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

	//移動操作
	void Move();

	//移動速度減衰
	void MoveAttenuation();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	const WorldTransform& GetWorldTransformPlayer() { return worldTransform2_; }
	void SetWorldTransform(const Vector3 translation);

	//落下処理
	void IsFallStart();

	void SetScale(const Vector3 scale) { worldTransform_.scale_ = scale; }

	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHitOnFloor;

	bool isGameover() { return gameOver; }

	StructSphere GetStructSphere() { return structSphere_; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	Vector3 GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3 velocity);

	bool GetIsMove() { return isMove_; }
	void SetIsMove(const bool isMove) { isMove_ = isMove; }

	int GetMoveMode() { return moveMode_; }

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