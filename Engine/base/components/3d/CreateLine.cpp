#include "CreateLine.h"
#include "CJEngine.h"

void CreateLine::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	world_.Initialize();

	environmentTexture_ = textureManager_->ddsSample;

	SettingVertex();
	SettingColor();
	SettingDictionalLight();
}

void CreateLine::Draw(const WorldTransform& worldTransform1, const WorldTransform& worldTransform2, const ViewProjection& viewProjection, const Vector4& material) {	
	//上面
	vertexData_[12].position = { worldTransform1.translation_.num[0] - size_,worldTransform1.translation_.num[1],worldTransform1.translation_.num[2] - size_ ,1.0f };
	vertexData_[13].position = { worldTransform1.translation_.num[0] - size_,worldTransform1.translation_.num[1],worldTransform1.translation_.num[2] + size_ ,1.0f };
	vertexData_[14].position = { worldTransform1.translation_.num[0] + size_,worldTransform1.translation_.num[1],worldTransform1.translation_.num[2] + size_ ,1.0f };
	vertexData_[15].position = vertexData_[12].position;
	vertexData_[16].position = { worldTransform1.translation_.num[0] + size_,worldTransform1.translation_.num[1],worldTransform1.translation_.num[2] - size_ ,1.0f };
	vertexData_[17].position = vertexData_[14].position;
	//底面
	vertexData_[18].position = { worldTransform2.translation_.num[0] - size_,worldTransform2.translation_.num[1],worldTransform2.translation_.num[2] - size_ ,1.0f };
	vertexData_[19].position = { worldTransform2.translation_.num[0] - size_,worldTransform2.translation_.num[1],worldTransform2.translation_.num[2] + size_ ,1.0f };
	vertexData_[20].position = { worldTransform2.translation_.num[0] + size_,worldTransform2.translation_.num[1],worldTransform2.translation_.num[2] + size_ ,1.0f };
	vertexData_[21].position = vertexData_[18].position;
	vertexData_[22].position = { worldTransform2.translation_.num[0] + size_,worldTransform2.translation_.num[1],worldTransform2.translation_.num[2] - size_ ,1.0f };
	vertexData_[23].position = vertexData_[20].position;
	//前面
	vertexData_[0].position = vertexData_[19].position;//左上
	vertexData_[1].position = vertexData_[20].position;
	vertexData_[2].position = vertexData_[14].position;//右下
	vertexData_[3].position = vertexData_[19].position;//左上
	vertexData_[4].position = vertexData_[13].position;
	vertexData_[5].position = vertexData_[14].position;//右下
	//背面
	vertexData_[6].position = vertexData_[18].position;
	vertexData_[7].position = vertexData_[22].position;
	vertexData_[8].position = vertexData_[16].position;
	vertexData_[9].position = vertexData_[18].position;
	vertexData_[10].position = vertexData_[12].position;
	vertexData_[11].position = vertexData_[16].position;
	//右側面
	vertexData_[24].position = vertexData_[22].position;
	vertexData_[25].position = vertexData_[16].position;
	vertexData_[26].position = vertexData_[14].position;
	vertexData_[27].position = vertexData_[22].position;
	vertexData_[28].position = vertexData_[20].position;
	vertexData_[29].position = vertexData_[14].position;
	//左側面
	vertexData_[30].position = vertexData_[18].position;
	vertexData_[31].position = vertexData_[12].position;
	vertexData_[32].position = vertexData_[13].position;
	vertexData_[33].position = vertexData_[18].position;
	vertexData_[34].position = vertexData_[19].position;
	vertexData_[35].position = vertexData_[13].position;

	for (uint32_t i = 0; i < vertexCount_; ++i) {
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}

	world_.translation_ = { 0.0f,0.0f,0.0f };
	world_.scale_ = { 1.0f,1.0f,1.0f };
	world_.UpdateMatrix();

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
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, world_.constBuff_->GetGPUVirtualAddress());
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
	dxCommon_->GetCommandList()->DrawIndexedInstanced(vertexCount_, 1, 0, 0, 0);
}

void CreateLine::Finalize() {

}

void CreateLine::SettingVertex() {
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

void CreateLine::SettingColor() {
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void CreateLine::SettingDictionalLight() {
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

void CreateLine::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
	isDirectionalLight_ = isDirectionalLight;
	lightNum_ = lightNum;
}