/**
 * @file WorldTransform.h
 * @brief WorldTransformの初期化及び管理を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "MathCalculation.h"

struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; // ローカル→ワールド変換行列
	Matrix4x4 inverseTranspose; // ローカル→ワールド変換行列
};

struct WorldTransform {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;
	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// XYZ軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };
	Quaternion quaternion_;
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	void Initialize(uint32_t numInstance);
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer(uint32_t numInstance);
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	//全部まとめて更新
	void UpdateMatrix();

	//クォータニオンの更新
	void  UpdateQuaternionMatrix();

	Vector3 GetWorldPos();
};