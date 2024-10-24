/**
 * @file Enemy.h
 * @brief プレイヤーの邪魔となる敵を管理
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

class Enemy : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	//移動処理
	void Move();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	void SetWorldTransform(const Vector3 translation);

	//落下処理
	void IsFallStart();

	//座標の設定
	void SetObjectPos(const WorldTransform& worldtransform);

	bool isHitOnFloor = false;

	//生死フラグ
	bool GetisDead() { return isDead_; }
	void SetisDead() { isDead_ = true; }

	//当たり判定用のSphereを得る
	StructSphere GetStructSphere() { return structSphere_; }

	//移動量を得る、設定する
	Vector3 GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3 velocity);

private:
	Input* input_ = nullptr;

	Vector3 velocity_ = {};

	WorldTransform objectPos_;

	bool isDead_ = false;

	StructSphere structSphere_;

	const ViewProjection* viewProjection_ = nullptr;

	bool isGravityAccelerationCalc = false;
};
