#pragma once
#include "DirectXCommon.h"
#include "CJEngine.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include <string>
#include <fstream>
#include <sstream>
#include "DirectionalLight.h"
#include "PointLight.h"
#include <wrl.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	void Initialize(const std::string& directoryPath, const std::string& filename);

	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material);
	
	void Finalize();

	ModelData modelData_;
	
	/// <summary>
	/// ライティングを行うかどうか＆ライティングの種類
	/// 1:HalfLambert
	/// 2:PhongReflection
	/// 2:BlinnPhongReflection
	/// </summary>
	void SetDirectionalLightFlag(bool isDirectionalLight,int lightNum);

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	Node ReadNode(aiNode* node);

	static Model* CreateModelFromObj(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// VATに必要なテクスチャのロード(テクスチャの名前は固定)
	/// 1:VATpos.png
	/// 2:VATrot.png
	/// </summary>
	void LoadVATData(const std::string& directoryPath);

private:
	DirectXCommon* dxCommon_;
	CitrusJunosEngine* CJEngine_;
	TextureManager* textureManager_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* material_;

	uint32_t texture_;

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

	bool isLoadTexCoord_ = false;//TexCoordがモデルに設定されているか

	Appdata* appData_;
	bool isVAT_ = false;//VATモデルかどうか
	uint32_t vatPosTex_;
	uint32_t vatRotTex_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vatResource_;

private:
	void CreateVartexData();
	void SetColor();
	void CreateLight();

};