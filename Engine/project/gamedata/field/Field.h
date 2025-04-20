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
	VectorInt2 massLocation;
};

class Field {
public:
	void Initialize(Model* model);
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void Finalize();

private:
	//各ブロックの生成
	void CreateBlocks(const int x, const int z);

	//座標を基にブロック位置を取得する
	VectorInt2 GetBlockAt(float x, float z);

	//指定ブロックを中心に周囲のの高さを変える
	void RaiseBlocksAround(const VectorInt2& center, float radius, float deltaY);

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	Input* input_ = nullptr;

	Vector3 centerBlockPos_ = { 0.0f,0.0f,0.0f };//基準となる中心ブロックの座標
	float blockSize_ = 1.0f;//ブロックのサイズ

	std::list<Block> blocks_;//各種ブロック
	static const int verticalSize_ = 10;//縦のブロック数
	static const int horizontalSize_ = 10;//横のブロック数
	float blockWidth_ = 2.1f;//ブロック間隔
	float prevBlockWidth_ = blockWidth_;//前フレームのブロック間隔

	//テスト用変数
	Vector2 nowPos_ = { 0.0f,0.0f };
	float radius_ = 1.0f;
	float deltaY_ = -0.5f;
	Block block_;
};