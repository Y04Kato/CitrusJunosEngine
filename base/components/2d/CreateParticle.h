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
	void Initialize(int kNumInstance);
	void Update();
	void Finalize();
	void Draw(const ViewProjection& viewProjection);

private:
	void SettingVertex();

	void SettingColor();

	void SettingDictionalLight();

	uint32_t LoadBuffer(int kNumInstance);
	void LoadBuffer(uint32_t index, int kNumInstance);

private:
	DirectXCommon* dxCommon_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	VertexData* vertexData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Material* materialData_ = nullptr;

	//パーティクルの数
	int kNumInstance_;
	//パーティクルの数と同じ数のTransform
	std::vector<WorldTransform> transform_;

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	uint32_t bufferIndex_;
	ConstBufferDataWorldTransform* instancingData_ = nullptr;

};


