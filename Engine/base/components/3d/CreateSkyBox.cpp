#include "CreateSkyBox.h"
#include "CJEngine.h"

void CreateSkyBox::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	SettingVertex();
	SettingColor();
	SettingDictionalLight();
}

void CreateSkyBox::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const Vector4& material, uint32_t textureIndex) {
	EulerTransform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.num[2]));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));

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

	//描画
	dxCommon_->GetCommandList()->DrawIndexedInstanced(vertexCount_, 1, 0, 0, 0);
}

void CreateSkyBox::Finalize() {

}

void CreateSkyBox::SettingVertex() {
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * vertexCount_);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点2つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount_;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * vertexCount_);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

	indexBufferView_.SizeInBytes = sizeof(uint32_t) * vertexCount_;

	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//Boxに必要な8頂点を定義
	const float halfSize = 1.0f;
	VertexData vertices[8] = {
		{{halfSize, halfSize, halfSize, 1.0f}, {0.0f, 1.0f}}, //0
		{{-halfSize, halfSize, halfSize, 1.0f}, {1.0f, 1.0f}}, //1
		{{-halfSize, -halfSize, halfSize, 1.0f}, {1.0f, 0.0f}}, //2
		{{halfSize, -halfSize, halfSize, 1.0f}, {0.0f, 0.0f}}, //3
		{{halfSize, halfSize, -halfSize, 1.0f}, {0.0f, 1.0f}}, //4
		{{-halfSize, halfSize, -halfSize, 1.0f}, {1.0f, 1.0f}}, //5
		{{-halfSize, -halfSize, -halfSize, 1.0f}, {1.0f, 0.0f}}, //6
		{{halfSize, -halfSize, -halfSize, 1.0f}, {0.0f, 0.0f}} //7
	};

	//36個のindexを定義
	uint32_t indices[36] = {
		0, 1, 2, 2, 3, 0, //前面
		4, 5, 6, 6, 7, 4, //背面
		0, 1, 5, 5, 4, 0, //下面
		2, 3, 7, 7, 6, 2, //上面
		0, 3, 7, 7, 4, 0, //左側面
		1, 2, 6, 6, 5, 1  //右側面
	};

	//vertexDataをコピー
	for (uint32_t i = 0; i < 8; ++i) {
		vertexData_[i] = vertices[i];
		vertexData_[i].normal.num[0] = vertices[i].position.num[0];
		vertexData_[i].normal.num[1] = vertices[i].position.num[1];
		vertexData_[i].normal.num[2] = vertices[i].position.num[2];
	}

	//indexDataをコピー
	for (uint32_t i = 0; i < 36; ++i) {
		indexData_[i] = indices[i];
	}
}

void CreateSkyBox::SettingColor() {
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void CreateSkyBox::SettingDictionalLight() {
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

void CreateSkyBox::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
	isDirectionalLight_ = isDirectionalLight;
	lightNum_ = lightNum;
}