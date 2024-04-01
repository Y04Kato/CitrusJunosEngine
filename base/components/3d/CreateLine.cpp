#include "CreateLine.h"
#include "CJEngine.h"

void CreateLine::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CJEngine_ = CitrusJunosEngine::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	world_.Initialize();

	SettingVertex();
	SettingColor();
	SettingDictionalLight();
}

void CreateLine::Draw(const WorldTransform& worldTransform1, const WorldTransform& worldTransform2, const ViewProjection& viewProjection, const Vector4& material) {
	//前面
	vertexData_[0].position = { -1.0f,-1.0f,1.0f ,1.0f };//左上
	vertexData_[1].position = { 1.0f,-1.0f,1.0f ,1.0f };
	vertexData_[2].position = { 1.0f,1.0f,1.0f ,1.0f };//右下
	vertexData_[3].position = { -1.0f,-1.0f,1.0f ,1.0f };//左上
	vertexData_[4].position = { -1.0f,1.0f,1.0f ,1.0f };
	vertexData_[5].position = { 1.0f,1.0f,1.0f ,1.0f };//右下
	//背面
	vertexData_[6].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[7].position = { 1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[8].position = { 1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[9].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[10].position = { -1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[11].position = { 1.0f,1.0f,-1.0f ,1.0f };
	//上面
	vertexData_[12].position = { -1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[13].position = { -1.0f,1.0f,1.0f ,1.0f };
	vertexData_[14].position = { 1.0f,1.0f,1.0f ,1.0f };
	vertexData_[15].position = { -1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[16].position = { 1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[17].position = { 1.0f,1.0f,1.0f ,1.0f };
	//底面
	vertexData_[18].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[19].position = { -1.0f,-1.0f,1.0f ,1.0f };
	vertexData_[20].position = { 1.0f,-1.0f,1.0f ,1.0f };
	vertexData_[21].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[22].position = { 1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[23].position = { 1.0f,-1.0f,1.0f ,1.0f };
	//右側面
	vertexData_[24].position = { 1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[25].position = { 1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[26].position = { 1.0f,1.0f,1.0f ,1.0f };
	vertexData_[27].position = { 1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[28].position = { 1.0f,-1.0f,1.0f ,1.0f };
	vertexData_[29].position = { 1.0f,1.0f,1.0f ,1.0f };
	//左側面
	vertexData_[30].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[31].position = { -1.0f,1.0f,-1.0f ,1.0f };
	vertexData_[32].position = { -1.0f,1.0f,1.0f ,1.0f };
	vertexData_[33].position = { -1.0f,-1.0f,-1.0f ,1.0f };
	vertexData_[34].position = { -1.0f,-1.0f,1.0f ,1.0f };
	vertexData_[35].position = { -1.0f,1.0f,1.0f ,1.0f };

	for (uint32_t i = 0; i < vertexCount_; ++i) {
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}

	world_.translation_.num[0] = (worldTransform1.translation_.num[0] + worldTransform2.translation_.num[0]) / 2.0f;
	world_.translation_.num[1] = (worldTransform1.translation_.num[1] + worldTransform2.translation_.num[1]) / 2.0f;
	world_.translation_.num[2] = (worldTransform1.translation_.num[2] + worldTransform2.translation_.num[2]) / 2.0f;
	world_.scale_.num[1] = Distance(worldTransform1.translation_, worldTransform2.translation_) / 2.0f;
	world_.rotation_ = CalculateAngle(worldTransform1.translation_, worldTransform2.translation_);
	world_.UpdateMatrix();

	ImGui::Begin("line");
	ImGui::DragFloat3("Translate", world_.translation_.num, 0.05f);
	ImGui::DragFloat3("Rotate", world_.rotation_.num, 0.05f);
	ImGui::DragFloat3("Scale", world_.scale_.num, 0.05f);
	ImGui::End();

	Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
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
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewLine_);

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
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(0));

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

	indexResourceLine_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * vertexCount_);

	indexBufferViewLine_.BufferLocation = indexResourceLine_->GetGPUVirtualAddress();

	indexBufferViewLine_.SizeInBytes = sizeof(uint32_t) * vertexCount_;

	indexBufferViewLine_.Format = DXGI_FORMAT_R32_UINT;

	indexResourceLine_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataLine_));

	for (uint32_t i = 0; i < vertexCount_; i++) {
		indexDataLine_[i] = i;
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