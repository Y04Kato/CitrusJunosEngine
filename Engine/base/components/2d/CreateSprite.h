/**
 * @file CreateSprite.h
 * @brief 2DSpriteの初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "CJEngine.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include<wrl.h>

class CreateSprite {
public:
	void Initialize(Vector2 size, uint32_t textureIndex);
	void Draw(const EulerTransform& transform, const EulerTransform& uvTransform, const Vector4& material);
	void Finalize();

	static std::unique_ptr <CreateSprite> CreateSpriteFromTexture(Vector2 size, uint32_t textureIndex);

	//サイズ変更
	void SetSize(Vector2 size);

	//アンカーポイントの設定
	void SetAnchor(Vector2 anchor);

	//フリップの設定
	void SetFlip(bool isFlipX, bool isFlipY);

	//テクスチャ範囲指定
	void SetTextureLTSize(Vector2 textureLeftTop, Vector2 textureSize);

	//テクスチャの大きさと同じサイズに変更
	void SetTextureInitialSize();

	//テクスチャ変更
	void SetTextureIndex(uint32_t textureIndex);

	//指定したポイントのワールド座標を返す
	Vector2 GetLocalPosition(const Vector2& localPoint, const EulerTransform& transform);

	Vector2* GetSize() { return &size_; }
	Vector2* GetTextureLeftTop() { return &textureLeftTop_; }
	Vector2* GetTextureSize() { return &textureSize_; }

private:
	DirectXCommon* dxCommon_;
	TextureManager* textureManager_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceSprite_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* materialData_;

	CitrusJunosEngine* CJEngine_;

	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSprite_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	uint32_t* indexDataSprite_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Matrix4x4* wvpData_;

	Vector2 size_;
	Vector2 anchor_;

	Vector2 textureLeftTop_;
	Vector2 textureSize_;

	bool isFlipX_ = false;
	bool isFlipY_ = false;

	uint32_t index_;

private:
	void SettingVertex();
	void SettingColor();
	void SettingTransform();

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
};