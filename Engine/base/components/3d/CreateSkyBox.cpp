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
	//右面
	vertexData_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面
	vertexData_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前面
	vertexData_[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[11].position = { 1.0f,-1.0f,1.0f,1.0f };

	//背面
	vertexData_[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	//上面
	vertexData_[16].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[17].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[18].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[19].position = { 1.0f,1.0f,-1.0f,1.0f };
	//下面
	vertexData_[20].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[23].position = { 1.0f,-1.0f,-1.0f,1.0f };

	for (uint32_t i = 0; i < vertexCount_; ++i) {
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}

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
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureManager_->white));

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

	for (uint32_t i = 0; i < vertexCount_; i++) {
		indexData_[i] = i;
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