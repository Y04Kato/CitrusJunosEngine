/**
 * @file Receipt3D.h
 * @brief 頂点を指定した上でモデルの初期化及び描画を行う
 * @author KATO
 * @date 2025/01/07
 */

#pragma once

#include <vector>
#include <string>

#include "CJEngine.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include<wrl.h>

class Receipt3D {
public:
	//オブジェクトの変換情報
	Vector3 translate_;
	Vector3 rotate_;
	Vector3 scale_;

	//頂点と法線のリスト
	std::vector<Vector3> vertices_;
	std::vector<Vector3> normals_;
	std::vector<std::vector<int>> polygons_;

	//文字列データの解析
	void LoadFromString(const std::string& data);

	//解析
	void Initialize();

	void Draw(const ViewProjection& viewProjection, const Vector4& material);

	void Finalize();

	ModelData ConstructModelData();

	/// <summary>
	/// ライティングを行うかどうか＆ライティングの種類
	/// 1:HalfLambert
	/// 2:PhongReflection
	/// 3:BlinnPhongReflection
	/// 4:Enviroment+
	/// </summary>
	void SetDirectionalLightFlag(bool isDirectionalLight, int lightNum);

	//環境マップ用テクスチャの設定
	void SetEnvironmentTexture(const uint32_t& envTex) { environmentTexture_ = envTex; isSetEnviromentTexture_ = true; }

private:
	//解析
	void ParseLine(const std::string& line);
	void ParseTranslate(const std::string& line);
	void ParseRotate(const std::string& line);
	void ParseScale(const std::string& line);
	void ParseVertex(const std::string& line);
	void ParseNormal(const std::string& line);
	void ParsePolygon(const std::string& line);

	//描画
	void CreateVartexData();
	void SetColor();
	void CreateLight();

private:
	DirectXCommon* dxCommon_;
	CitrusJunosEngine* CJEngine_;
	TextureManager* textureManager_;

	ModelData modelData_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* mappedIndex_ = 0;

	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* materialData_;

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	bool isDirectionalLight_ = false;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	PointLights* pointLights_;
	PointLight* pointLight_;
	Microsoft::WRL::ComPtr <ID3D12Resource> pointLightResource_;

	int lightNum_;

	Microsoft::WRL::ComPtr <ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;

	WorldTransform worldModels_;

	bool isSetEnviromentTexture_ = false;
	uint32_t environmentTexture_;
};
