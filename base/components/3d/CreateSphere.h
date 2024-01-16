#pragma once
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include<wrl.h>

class CitrusJunosEngine;

class CreateSphere{
public:
	void Initialize();

	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material, uint32_t textureIndex);
	
	void Finalize();

	//ライティングを行うかどうか、trueに1でHalfLambert、2でPhongReflection
	void SetDirectionalLightFlag(bool isDirectionalLight, int lightNum);

private:
	void SettingVertex();
	void SettingColor();
	void SettingDictionalLight();

private:
	DirectXCommon* dxCommon_;
	CitrusJunosEngine* CJEngine_;
	TextureManager* textureManager_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* materialData_;

	const float pi = 3.1415f;
	uint32_t kSubDivision_;
	uint32_t vertexCount_;

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	bool isDirectionalLight_ = false;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSphere_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere_{};
	uint32_t* indexDataSphere_;

	int lightNum_;

	Microsoft::WRL::ComPtr <ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;
};