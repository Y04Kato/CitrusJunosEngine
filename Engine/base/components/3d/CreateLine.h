#pragma once
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include<wrl.h>

class CitrusJunosEngine;

class CreateLine {
public:
	void Initialize();

	void Draw(const WorldTransform& worldTransform1, const WorldTransform& worldTransform2, const ViewProjection& viewProjection, const Vector4& material);

	void Finalize();

	/// <summary>
	/// ライティングを行うかどうか＆ライティングの種類
	/// 1:HalfLambert
	/// 2:PhongReflection
	/// 3:BlinnPhongReflection
	/// 4:Enviroment+
	/// </summary>
	void SetDirectionalLightFlag(bool isDirectionalLight, int lightNum);

	//ラインの太さ
	void SetLineThickness(float thickness) { size_ = thickness; };

	void SetEnvironmentTexture(const uint32_t& envTex) { environmentTexture_ = envTex; isSetEnviromentTexture_ = true; }

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

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	bool isDirectionalLight_ = false;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	uint32_t vertexCount_ = 36;

	PointLights* pointLights_;
	PointLight* pointLight_;
	Microsoft::WRL::ComPtr <ID3D12Resource> pointLightResource_;

	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_;

	int lightNum_;

	WorldTransform world_;
	float size_ = 0.1f;

	Microsoft::WRL::ComPtr <ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;

	bool isSetEnviromentTexture_ = false;
	uint32_t environmentTexture_;
};