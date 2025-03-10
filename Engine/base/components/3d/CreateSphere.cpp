/**
 * @file CreateSphere.cpp
 * @brief 頂点を指定した上で球体の初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#include "CreateSphere.h"
#include "CJEngine.h"
#include<cmath>

void CreateSphere::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	environmentTexture_ = textureManager_->ddsSample;

	kSubDivision_ = 16;
	vertexCount_ = kSubDivision_ * kSubDivision_ * 6;
	SettingVertex();
	SettingColor();
	SettingDictionalLight();
}

void CreateSphere::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material, uint32_t textureIndex) {
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
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]のこと
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureIndex));
	if (isSetEnviromentTexture_ == true) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(environmentTexture_));
	}
	else {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(textureManager_->ddsSample));
	}

	//描画
	dxCommon_->GetCommandList()->DrawIndexedInstanced(vertexCount_, 1, 0, 0, 0);

}

void CreateSphere::Finalize() {

}

void CreateSphere::SettingVertex() {
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * vertexCount_);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount_;

	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * vertexCount_);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

	indexBufferView_.SizeInBytes = sizeof(uint32_t) * vertexCount_;

	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	for (uint32_t i = 0; i < vertexCount_; i++) {
		indexData_[i] = i;
	}

	//経度分割一つ分の角度
	const float kLonEvery = pi * 2.0f / float(kSubDivision_);
	const float kLatEvery = pi / float(kSubDivision_);

	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubDivision_; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubDivision_; ++lonIndex) {
			uint32_t start = (latIndex * kSubDivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			//頂点にデータを入力する　基準点a
			vertexData_[start].position = { cos(lat) * cos(lon),sin(lat),cos(lat) * sin(lon),1.0f };
			vertexData_[start].texcoord = { float(lonIndex) / float(kSubDivision_),1.0f - float(latIndex) / kSubDivision_ };

			vertexData_[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 1].texcoord = { vertexData_[start].texcoord.num[0],vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision_) };

			vertexData_[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 2].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision_),vertexData_[start].texcoord.num[1] };

			vertexData_[start + 3].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 3].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision_),vertexData_[start].texcoord.num[1] };

			vertexData_[start + 4].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 4].texcoord = { vertexData_[start].texcoord.num[0],vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision_) };

			vertexData_[start + 5].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 5].texcoord = { vertexData_[start].texcoord.num[0] + 1.0f / float(kSubDivision_),vertexData_[start].texcoord.num[1] - 1.0f / float(kSubDivision_) };

			vertexData_[start].normal.num[0] = vertexData_[start].position.num[0];
			vertexData_[start].normal.num[1] = vertexData_[start].position.num[1];
			vertexData_[start].normal.num[2] = vertexData_[start].position.num[2];

			vertexData_[start + 1].normal.num[0] = vertexData_[start + 1].position.num[0];
			vertexData_[start + 1].normal.num[1] = vertexData_[start + 1].position.num[1];
			vertexData_[start + 1].normal.num[2] = vertexData_[start + 1].position.num[2];

			vertexData_[start + 2].normal.num[0] = vertexData_[start + 2].position.num[0];
			vertexData_[start + 2].normal.num[1] = vertexData_[start + 2].position.num[1];
			vertexData_[start + 2].normal.num[2] = vertexData_[start + 2].position.num[2];

			vertexData_[start + 3].normal.num[0] = vertexData_[start + 3].position.num[0];
			vertexData_[start + 3].normal.num[1] = vertexData_[start + 3].position.num[1];
			vertexData_[start + 3].normal.num[2] = vertexData_[start + 3].position.num[2];

			vertexData_[start + 4].normal.num[0] = vertexData_[start + 4].position.num[0];
			vertexData_[start + 4].normal.num[1] = vertexData_[start + 4].position.num[1];
			vertexData_[start + 4].normal.num[2] = vertexData_[start + 4].position.num[2];

			vertexData_[start + 5].normal.num[0] = vertexData_[start + 5].position.num[0];
			vertexData_[start + 5].normal.num[1] = vertexData_[start + 5].position.num[1];
			vertexData_[start + 5].normal.num[2] = vertexData_[start + 5].position.num[2];
		}
	}
}

void CreateSphere::SettingColor() {
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void CreateSphere::SettingDictionalLight() {
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

void CreateSphere::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
	isDirectionalLight_ = isDirectionalLight;
	lightNum_ = lightNum;
}