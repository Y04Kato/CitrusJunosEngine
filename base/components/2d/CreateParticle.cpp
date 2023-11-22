#include "CreateParticle.h"

void CreateParticle::Initialize(int kNumInstance, Emitter emitter, AccelerationField accelerationField, uint32_t textureIndex) {
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	modelData_.vertices.push_back({ .position = { 1.0f,1.0f,0.0f,1.0f },.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},  .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//右下
	modelData_.textureIndex = textureIndex;

	kNumMaxInstance_ = kNumInstance;
	numInstance_ = 0;

	instancingResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice().Get(), sizeof(ParticleForGPU) * kNumInstance);

	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	for (int i = 0; i < kNumMaxInstance_; i++) {
		instancingData_[i].matWorld = MakeIdentity4x4();
	}

	bufferIndex_ = LoadBuffer(kNumMaxInstance_);

	SettingVertex();
	SettingColor();
	SettingDictionalLight();

	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();
	
	emitter_ = emitter;
	accelerationField_ = accelerationField;
	
}

void CreateParticle::Update() {
	std::mt19937 randomEngine(seedGenerator());
	ImGui::Begin("Particle");
	ImGui::Checkbox("UseBillBoard", &isBillBoard_);
	if (ImGui::Button("Add Particle")) {
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
	}
	ImGui::DragFloat3("EmitterTranslate", emitter_.transform.translate.num, 0.1f);
	ImGui::End();
}

void CreateParticle::Finalize() {

}

void CreateParticle::Draw(const ViewProjection& viewProjection) {
	std::mt19937 randomEngine(seedGenerator());
	Matrix4x4 billboardMatrix = MakeIdentity4x4();
	if (isBillBoard_) {
		billboardMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>) * Inverse(viewProjection.matView);
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;
	}

	emitter_.frequencyTime += kDeltaTime;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
		emitter_.frequencyTime -= emitter_.frequency;
	}

	for (std::list<Particle>::iterator iterator = particles_.begin(); iterator != particles_.end();) {
		if ((*iterator).lifeTime <= (*iterator).currentTime) {
			iterator = particles_.erase(iterator);
			continue;
		}

		//Fieldの適用
		if (IsCollision(accelerationField_.area, (*iterator).transform.translate)) {
			(*iterator).velocity += accelerationField_.acceleration * kDeltaTime;
		}

		(*iterator).transform.translate += (*iterator).velocity * kDeltaTime;
		(*iterator).currentTime += kDeltaTime;//経過時間を足す

		if (numInstance_ < kNumMaxInstance_) {
			instancingData_[numInstance_].matWorld = MakeScaleMatrix((*iterator).transform.scale) * billboardMatrix * MakeTranslateMatrix((*iterator).transform.translate);

			instancingData_[numInstance_].color = (*iterator).color;
			float alpha = 1.0f - ((*iterator).currentTime / (*iterator).lifeTime);
			instancingData_[numInstance_].color.num[3] = alpha;

			numInstance_++;//生きているParticleの数をカウントする
		}

		iterator++;
	}

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

	dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), numInstance_, 0, 0);

	numInstance_ = 0;
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
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	//SRVを作成するDescripterHeapの場所を決める
	textureManager_->textureSrvHandleGPU_[index] = textureManager_->GetGPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), textureManager_->GetDescriptorSizeSRV(), index);
	textureManager_->textureSrvHandleCPU_[index] = textureManager_->GetCPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), textureManager_->GetDescriptorSizeSRV(), index);

	//先頭はIMGUIが使ってるので、その次を使う
	textureManager_->textureSrvHandleGPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureManager_->textureSrvHandleCPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, textureManager_->textureSrvHandleCPU_[index]);
}

Particle CreateParticle::MakeNewParticle(std::mt19937& randomEngine, const Transform transform) {
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	Particle particles;
	particles.transform.scale = { 1.0f,1.0f,1.0f };
	particles.transform.rotate = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.transform.translate = transform.translate + randomTranslate;
	particles.velocity = { distribution(randomEngine) ,distribution(randomEngine) ,distribution(randomEngine) };
	particles.color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine) ,1.0f };
	particles.lifeTime = distTime(randomEngine);
	particles.currentTime = 0;

	return particles;
}

std::list<Particle> CreateParticle::Emission(const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; count++) {
		particles_.push_back(MakeNewParticle(randomEngine, emitter.transform));
	}
	return particles;
}