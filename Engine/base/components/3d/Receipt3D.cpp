/**
 * @file Receipt3D.cpp
 * @brief 頂点を指定した上でモデルの初期化及び描画を行う
 * @author KATO
 * @date 2025/01/07
 */

#include "Receipt3D.h"
#include <sstream>
#include <iostream>

void Receipt3D::LoadFromString(const std::string& data) {
	std::stringstream ss(data);
	std::string line;

	while (std::getline(ss, line)) {
		ParseLine(line);
	}
}

void Receipt3D::ParseLine(const std::string& line) {
	if (line.find("Translate") == 0) {
		ParseTranslate(line);
	}
	else if (line.find("Rotate") == 0) {
		ParseRotate(line);
	}
	else if (line.find("Scale") == 0) {
		ParseScale(line);
	}
	else if (line.find("v ") == 0) {
		ParseVertex(line);
	}
	else if (line.find("vn ") == 0) {
		ParseNormal(line);
	}
	else if (line.find("f ") == 0) {
		ParsePolygon(line);
	}
}

void Receipt3D::ParseTranslate(const std::string& line) {
	std::stringstream ss(line.substr(9));  // "Translate " を除いた部分
	ss >> translate_.num[0] >> translate_.num[1] >> translate_.num[2];
}

void Receipt3D::ParseRotate(const std::string& line) {
	std::stringstream ss(line.substr(7));  // "Rotate " を除いた部分
	ss >> rotate_.num[0] >> rotate_.num[1] >> rotate_.num[2];
}

void Receipt3D::ParseScale(const std::string& line) {
	std::stringstream ss(line.substr(6));  // "Scale " を除いた部分
	ss >> scale_.num[0] >> scale_.num[1] >> scale_.num[2];
}

void Receipt3D::ParseVertex(const std::string& line) {
	Vector3 vertex;
	std::stringstream ss(line.substr(2));  // "v " を除いた部分
	ss >> vertex.num[0] >> vertex.num[1] >> vertex.num[2];
	vertices_.push_back(vertex);
}

void Receipt3D::ParseNormal(const std::string& line) {
	Vector3 normal;
	std::stringstream ss(line.substr(3));  // "vn " を除いた部分
	ss >> normal.num[0] >> normal.num[1] >> normal.num[2];
	normals_.push_back(normal);
}

void Receipt3D::ParsePolygon(const std::string& line) {
	std::vector<int> polygon;
	std::stringstream ss(line.substr(2));  // "f " を除いた部分
	std::string vertex_data;

	while (ss >> vertex_data) {
		std::stringstream vertex_ss(vertex_data);
		std::string index;
		std::getline(vertex_ss, index, '/');  // 頂点インデックスを取得
		polygon.push_back(std::stoi(index));
	}
	polygons_.push_back(polygon);
}

void Receipt3D::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	modelData_ = ConstructModelData();

	worldModels_.Initialize();

	environmentTexture_ = textureManager_->ddsSample;

	CreateVartexData();
	SetColor();
	CreateLight();
}

void Receipt3D::Draw(const ViewProjection& viewProjection, const Vector4& material) {
	worldModels_.translation_ = translate_;
	worldModels_.rotation_ = rotate_;
	worldModels_.scale_ = scale_;
	worldModels_.UpdateMatrix();

	EulerTransform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

	cameraData_->worldPosition = viewProjection.translation_;

	if (isDirectionalLight_ == false) {
		*materialData_ = { material,0 };
	}
	else {
		*materialData_ = { material,lightNum_ };
	}
	materialData_->uvTransform = uvtransformMtrix;
	materialData_->shininess = 50.0f;
	*directionalLight_ = directionalLights_->GetDirectionalLight();
	*pointLight_ = pointLights_->GetPointLight();

	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	//形状を設定。PS0に設定しているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldModels_.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]のこと
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureManager_->white));
	if (isSetEnviromentTexture_ == true) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(environmentTexture_));
	}
	else {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(textureManager_->ddsSample));
	}

	//描画
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Receipt3D::Finalize() {

}

ModelData Receipt3D::ConstructModelData() {
	ModelData modelData;
	std::vector<Vector4> positions; // 位置
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::vector<Vector3> normals;   // 法線

	// 頂点情報を登録
	for (const auto& vertex : vertices_) {
		Vector4 position = { vertex.num[0], vertex.num[1], vertex.num[2], 1.0f };
		positions.push_back(position);

		// 仮のテクスチャ座標を登録
		Vector2 texcoord = { 0.5f, 0.5f }; // 適当な値
		texcoords.push_back(texcoord);
	}

	// ポリゴンごとの法線を計算
	std::vector<Vector3> polygonNormals(polygons_.size());
	for (size_t i = 0; i < polygons_.size(); ++i) {
		const auto& polygon = polygons_[i];
		if (polygon.size() < 3) {
			std::cerr << "Invalid polygon with less than 3 vertices" << std::endl;
			continue;
		}

		// ポリゴンの頂点を取得
		Vector3 v0 = { positions[polygon[0] - 1].num[0], positions[polygon[0] - 1].num[1], positions[polygon[0] - 1].num[2] };
		Vector3 v1 = { positions[polygon[1] - 1].num[0], positions[polygon[1] - 1].num[1], positions[polygon[1] - 1].num[2] };
		Vector3 v2 = { positions[polygon[2] - 1].num[0], positions[polygon[2] - 1].num[1], positions[polygon[2] - 1].num[2] };

		// 法線を計算
		Vector3 edge1 = Subtruct(v1, v0);
		Vector3 edge2 = Subtruct(v2, v0);
		Vector3 normal = Normalize(Cross(edge1, edge2));
		polygonNormals[i] = normal;
	}

	// ポリゴン情報を登録
	uint32_t indexOffset = 0;
	for (size_t i = 0; i < polygons_.size(); ++i) {
		const auto& polygon = polygons_[i];
		const Vector3& normal = polygonNormals[i];

		for (size_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
			int index = polygon[faceVertex] - 1;

			if (index < 0 || index >= static_cast<int>(positions.size())) {
				std::cerr << "Index out of range: " << index << std::endl;
				continue;
			}

			Vector4 position = positions[index];
			Vector2 texcoord = texcoords[index];

			VertexData vertex = { position, texcoord, normal };
			modelData.vertices.push_back(vertex);
			modelData.indices.push_back(indexOffset++);
		}
	}

	modelData.material = MaterialData{};
	modelData.textureIndex = 0;
	modelData.rootNode = Node{};
	modelData.directoryPath = "";
	modelData.filename = "";

	return modelData;
}

void Receipt3D::CreateVartexData() {
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(VertexData) * (UINT)modelData_.vertices.size();

	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//IndexBuffer用のResourceとIndexBufferViewを作成
	indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//ResourceにIndexの内容をコピー
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
}

void Receipt3D::SetColor() {
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Receipt3D::CreateLight() {
	//DirectionalLight
	directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));

	//PointLight
	pointLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(PointLight));
	pointLightResource_->Map(0, NULL, reinterpret_cast<void**>(&pointLight_));

	//ライティング用のカメラリソース
	cameraResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = { 0,0,0 };
}


void Receipt3D::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
	isDirectionalLight_ = isDirectionalLight;
	lightNum_ = lightNum;
}
