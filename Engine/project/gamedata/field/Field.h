/**
 * @file Field.h
 * @brief ステージ全般の機能を管理
 * @author KATO
 * @date 2025/04/18
 */

#pragma once
#include "Model.h"

struct Block {
	Model model;
	WorldTransform world;
	Vector4 color;
};

class Field {
public:
	void Initialize(Model* model);
	void Update();
	void Draw(const ViewProjection& viewProjection);

private:
	WorldTransform worldTransform_;
	Model* model_;

	Vector3 CenterBlockPos = { 0.0f,0.0f,0.0f };//基準となる中心ブロックの座標
	float BlockSize = 1.0f;//ブロックのサイズ

	std::list<Block> blocks_;//各種ブロック
	static const int verticalSize_ = 10;//縦のブロック数
	static const int horizontalSize_ = 10;//横のブロック数

};