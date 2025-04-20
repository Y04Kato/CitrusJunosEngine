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

	//指定ブロックを中心に周囲の高さを変える(減衰なし)
	void RaiseBlocksAround(const VectorInt2& center, float radius, float deltaY);

	//指定ブロックを中心に周囲の高さを変える(減衰あり)
	void RaiseBlocksAroundWithAttenuation(const VectorInt2& center, float radius, float deltaY);

	/// <summary>
	/// 各ブロックの高さで色を変える
	/// 第一引数:Yが高い時の色の最大値
	/// 第二引数:Yが低い時の色の最大値
	/// 第三引数:中心Y座標
	/// 第四引数:+-範囲を色が最大となる値
	/// </summary>
	void ColorAdjustmentByHeight(const Vector4& highColor, const Vector4& lowColor , float centerY, float rangeY);

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
	Vector4 highColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };
	Vector4 lowColor_ = { 0.0f, 0.0f, 1.0f, 1.0f };
};