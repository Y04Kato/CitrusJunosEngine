#pragma once
#include "CJEngine.h"
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include<wrl.h>

class CreateParticle {
public:
	void Initialize(const Vector2& size_);
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material, uint32_t textureIndex);
	void Finalize();

private:
	void SettingVertex();

	void SettingColor();

	void SettingDictionalLight();

private:
	DirectXCommon* dxCommon_;
	TextureManager* textureManager_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceSprite_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* materialData_;

	CitrusJunosEngine* CJEngine_;

	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_;

	Vector2 size_;
};


