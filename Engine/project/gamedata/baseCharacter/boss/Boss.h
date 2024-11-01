/**
 * @file Boss.h
 * @brief プレイヤーを倒す為に動くボスを管理
 * @author KATO
 * @date 2024/10/11
 */

#pragma once
#include "baseCharacter/BaseCharacter.h"
#include "CreateParticle.h"

struct Body {
	Model model;
	WorldTransform world;
	Vector4 material;
};

class Boss : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	//バイクの玩具で引く攻撃
	void Attack1();

	//独楽を撃ちだす攻撃
	void Attack2();

	//生死フラグ
	bool GetisDead() { return isDead_; }
	void SetisDead(const bool flag) { isDead_ = flag; }

	WorldTransform GetWorldTransform() { return worldTransform_; }
	void SetWorldTransform(const Vector3 translation) { worldTransform_.translation_ = translation; }

	void SpawnBody(EulerTransform transform, Vector4 color);

protected:
	TextureManager* textureManager_;

	const int maxHP_ = 6;//最大体力
	const int faseChangePoint_ = 3;//体力がこの値以下になったらフェイズ2へ
	int hp_ = maxHP_;//現在体力

	std::unique_ptr<Model> bikeModel_;
	WorldTransform bikeWorld_;

	std::list<Body> bodys_;
	ModelData bodyModelData_;
	uint32_t bodyTexture_;
	EulerTransform bodyTransform_;

	//生死フラグ
	bool isDead_ = false;

	//攻撃に関するモノ
	const int resetTime_ = 180;//攻撃のクールタイム
	int attackTimer_ = resetTime_;//現在のクールタイム
	int nowAttackPattern_ = 1;//次の攻撃内容。各Attack関数の末尾の数字

	//Other
	std::random_device seedGenerator;
};
