/**
 * @file Ground.h
 * @brief ゲームシーンでの床を管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Model.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include <memory>

class Ground {
public:
	void Initialize(Model* model, Vector3 translation, Vector3 Scale);
	
	void Update();
	
	void Draw(const ViewProjection& viewProjection);
	
	//床の座標設定
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	//ステージ鑑賞用の座標設定
	WorldTransform& GetWorldTransformRotate() { return worldTransformRotate_; }

private:
	WorldTransform worldTransform_;
	WorldTransform worldTransformRotate_;
	Model* groundModel_;
};
