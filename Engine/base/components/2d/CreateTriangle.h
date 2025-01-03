/**
 * @file CreateTriangle.h
 * @brief 2D三角形の初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include<wrl.h>

class CitrusJunosEngine;

class CreateTriangle {
public:
	void Initialize();

	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material, uint32_t textureIndex);

	void Finalize();

	//環境マップ用テクスチャの設定
	void SetEnvironmentTexture(const uint32_t& envTex) { environmentTexture_ = envTex; isSetEnviromentTexture_ = true; }

private:
	void SettingVertex();

	void SettingColor();

	void SettingDictionalLight();

private:
	CitrusJunosEngine* CJEngine_;

	DirectXCommon* dxCommon_;

	TextureManager* textureManager_;

	VertexData* vertexData_;

	Material* materialData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;

	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	bool isSetEnviromentTexture_ = false;
	uint32_t environmentTexture_;
};
