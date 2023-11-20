#include "CreateParticle.h"

void CreateParticle::Initialize(int kNumInstance) {
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	modelData_.vertices.push_back({ .position = { 1.0f,1.0f,0.0f,1.0f },.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},  .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//右下
	modelData_.material.textureFilePath = "project/gamedata/resources/uvChecker.png";

	uint32_t texture = textureManager_->Load("project/gamedata/resources/uvChecker.png");
	modelData_.textureIndex = texture;

	kNumInstance_ = kNumInstance;

	instancingResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice().Get(), sizeof(ConstBufferDataWorldTransform) * kNumInstance);

	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	for (int i = 0; i < kNumInstance_; i++) {
		instancingData_[i].matWorld = MakeIdentity4x4();
	}

	bufferIndex_ = LoadBuffer(kNumInstance_);

	SettingVertex();
	SettingColor();
	SettingDictionalLight();

	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();

	for (int i = 0; i < kNumInstance_; i++) {
		WorldTransform w;
		transform_.push_back(w);
		transform_[i].scale_ = { 1.0f,1.0f,1.0f };
		transform_[i].rotation_ = { 0.0f,0.0f,0.0f };
		transform_[i].translation_ = { i * 0.1f,i * 0.1f,i * 0.1f };

		instancingData_[i].matWorld = MakeAffineMatrix(transform_[i].scale_, transform_[i].rotation_, transform_[i].translation_);
	}
}

void CreateParticle::Update() {
	for (int i = 0; i < kNumInstance_; i++) {
		instancingData_[i].matWorld = MakeAffineMatrix(transform_[i].scale_, transform_[i].rotation_, transform_[i].translation_);
	}
}

void CreateParticle::Finalize() {

}

void CreateParticle::Draw(const ViewProjection& viewProjection) {
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//ViewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	//色とUvTransform
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//テクスチャ
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, textureManager_->textureSrvHandleGPU_[bufferIndex_]);
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(modelData_.textureIndex));

	dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), kNumInstance_, 0, 0);
}

void CreateParticle::SettingVertex() {
	//CreateVertexResource
	vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込むためのアドレスを取得
	vertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void CreateParticle::SettingColor() {
	//マテリアル用のリソースを作る　今回はcolor1つ分
	materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void CreateParticle::SettingDictionalLight() {
	directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource_->Map(0, NULL, reinterpret_cast<void**>(&directionalLight_));
}

uint32_t CreateParticle::LoadBuffer(int kNumInstance) {
	uint32_t index = textureManager_->GetTextureIndex();
	index++;
	LoadBuffer(index, kNumInstance);
	textureManager_->SetTextureIndex(index);
	return index;
}

void CreateParticle::LoadBuffer(uint32_t index, int kNumInstance) {
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = kNumInstance;
	srvDesc.Buffer.StructureByteStride = sizeof(ConstBufferDataWorldTransform);

	//SRVを作成するDescripterHeapの場所を決める
	textureManager_->textureSrvHandleGPU_[index] = textureManager_->GetGPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), textureManager_->GetDescriptorSizeSRV(), index);
	textureManager_->textureSrvHandleCPU_[index] = textureManager_->GetCPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), textureManager_->GetDescriptorSizeSRV(), index);

	//先頭はIMGUIが使ってるので、その次を使う
	textureManager_->textureSrvHandleGPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureManager_->textureSrvHandleCPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, textureManager_->textureSrvHandleCPU_[index]);
}