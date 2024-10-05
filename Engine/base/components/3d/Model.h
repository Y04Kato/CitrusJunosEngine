/**
 * @file Model.h
 * @brief .objや.fbx、.gltfを使用したモデルの初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "CJEngine.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include <string.h>
#include <fstream>
#include <sstream>
#include "DirectionalLight.h"
#include "PointLight.h"
#include <wrl.h>
#include "SRVManager/SRVManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	//通常のモデルや、KeyFrameアニメーションに対応した関数
	void Initialize(const std::string& directoryPath, const std::string& filename);//ディレクトリパスを引数にイニシャライズ
	void Initialize(const ModelData modeldata, const uint32_t texture);//既に読み込み済みのモデルを引数にイニシャライズ
	static Model* CreateModel(const std::string& directoryPath, const std::string& filename);//ディレクトリパスを引数にモデルを生成する
	static Model* CreateModel(const ModelData modeldata, const uint32_t texture);//既に読み込み済みのモデルを引数にモデルを生成する
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material);//通常描画

	//Skinningモデル専用の関数
	void SkinningInitialize(const std::string& directoryPath, const std::string& filename);//ディレクトリパスを引数にイニシャライズ
	void SkinningInitialize(const ModelData modeldata,const uint32_t texture);//既に読み込み済みのモデルを引数にイニシャライズ
	static Model* CreateSkinningModel(const std::string& directoryPath, const std::string& filename);//ディレクトリパスを引数にモデルを生成する
	static Model* CreateSkinningModel(const ModelData modeldata, const uint32_t texture);//既に読み込み済みのモデルを引数にモデルを生成する
	void SkinningDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material);//Skinning専用描画

	void Finalize();

	ModelData modelData_;

	/// <summary>
	/// ライティングを行うかどうか＆ライティングの種類
	/// 1:HalfLambert
	/// 2:PhongReflection
	/// 3:BlinnPhongReflection
	/// 4:Enviroment+
	/// </summary>
	void SetDirectionalLightFlag(bool isDirectionalLight, int lightNum);

	//各種ロード関数
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);//通常のモデルや、KeyFrameアニメーションモデル用
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);//Skinningモデル用

	//アニメーション時間の設定
	void SetAnimationTime(float animationTime);
	float GetAnimationMaxTime() { return animation_.duration; }

	//環境マップ用の.ddsの指定用
	void SetEnvironmentTexture(const uint32_t& envTex) { environmentTexture_ = envTex; isSetEnviromentTexture_ = true; }

	/// <summary>
	/// VATに必要なテクスチャのロード(テクスチャの名前は固定、モデルファイルと同じディレクトリを参照)
	/// 1:VATpos.png
	/// 2:VATrot.png
	/// </summary>
	void LoadVATData(const std::string& directoryPath, const VATData* vatdata);

	//VATのアニメーション時間の設定
	void SetVATTime(const float& animTime) { vatData_->VATTime = animTime; }

private:
	DirectXCommon* dxCommon_;
	CitrusJunosEngine* CJEngine_;
	TextureManager* textureManager_;
	SRVManager* srvManager_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Material* material_;

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

	bool isKeyframeAnim_ = false;//KeyframeAnimationかどうか
	bool isManualAnimTime_ = false;//AnimationTimeを手動でやっているか否か
	float animationTime_ = 0.0f;
	Animation animation_;

	Skeleton skeleton_;
	SkinCluster skinCluster_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* mappedIndex_ = 0;

	bool isVAT_ = false;//VATモデルかどうか
	uint32_t vatPosTex_;
	uint32_t vatRotTex_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vatResource_;
	VATData* vatData_;

	WorldTransform worldModels_;

	bool isSetEnviromentTexture_ = false;
	uint32_t environmentTexture_;

private:
	//頂点や色、ライティング関係
	void CreateVartexData();
	void SetColor();
	void CreateLight();

	//SkinClusterの作成用関数
	SkinCluster CreateSkinCluster();
	Node ReadNode(aiNode* node);

	//Skeletonの作成や接続用
	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	void Update(Skeleton& skeleton, SkinCluster& skinCluster);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
};