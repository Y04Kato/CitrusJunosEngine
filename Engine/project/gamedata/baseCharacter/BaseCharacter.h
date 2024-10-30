/**
 * @file BaseCharacter.h
 * @brief 自機や敵などの基本的なキャラクターの継承元
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Model.h>
#include <vector>

class BaseCharacter {
public:
	virtual void Initialize(Model* model);

	virtual void Update();

	virtual void Draw(const ViewProjection& viewProjection);

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	WorldTransform worldTransform_;
	Model* model_;
};
