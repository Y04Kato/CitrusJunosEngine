/**
 * @file Boss.h
 * @brief プレイヤーを倒す為に動くボスを管理
 * @author KATO
 * @date 2024/10/11
 */

#pragma once
#include "baseCharacter/BaseCharacter.h"

class Boss : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	//バイクの玩具で引く攻撃
	void Attack1();

	//独楽を撃ちだす攻撃
	void Attack2();

	WorldTransform GetWorldTransform() { return worldTransform_; }
	void SetWorldTransform(const Vector3 translation) { worldTransform_.translation_ = translation; }

protected:
	const int maxHP_ = 10;//最大体力
	const int faseChangePoint_ = 5;//体力がこの値以下になったらフェイズ2へ
	int hp_ = maxHP_;//現在体力
};
