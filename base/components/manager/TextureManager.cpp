#include "TextureManager.h"

void TextureManager::Initialize(){
	dxCommon_ = DirectXCommon::GetInstance();
	descriptorSizeDSV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	descriptorSizeRTV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureIndex_ = -1;
}

TextureManager* TextureManager::GetInstance(){
	static TextureManager instance;
	return &instance;
}

uint32_t TextureManager::Load(const std::string& filePath){
	textureIndex_++;
	LoadTexture(filePath, textureIndex_);
	return textureIndex_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUHandle(uint32_t textureHandle){
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = textureSrvHandleGPU_[textureHandle];
	return GPUHandle;
}

//テクスチャデータを読み込む
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	//テクスチャファイルを読んでうろグラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	return mipImages;
}
//テクスチャリソースを作る
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr <ID3D12Device> device, const DirectX::TexMetadata& metadata){
	//metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//texturの幅
	resourceDesc.Height = UINT(metadata.height);//texturの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行or配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段は2次元

	//利用するheapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	//Resourceを生成する
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));
	return resource;
}


void TextureManager::LoadTexture(const std::string& filePath, uint32_t index){
	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
	textureResource_[index] = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	intermediateResource_[index] = UploadTextureData(textureResource_[index], mipImage, index);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescripterHeapの場所を決める
	textureSrvHandleGPU_[index] = GetGPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), descriptorSizeSRV, index);
	textureSrvHandleCPU_[index] = GetCPUDescriptorHandle(dxCommon_->GetSrvDescriptiorHeap(), descriptorSizeSRV, index);

	//先頭はIMGUIが使ってるので、その次を使う
	textureSrvHandleCPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_[index].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource_[index].Get(), &srvDesc, textureSrvHandleCPU_[index]);

}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorheap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorheap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorheap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorheap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(Microsoft::WRL::ComPtr <ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, uint32_t index) {
	std::vector<D3D12_SUBRESOURCE_DATA>subresource;
	DirectX::PrepareUpload(dxCommon_->GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);
	uint64_t  intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresource.size()));
	intermediateResource_[index] = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), intermediateSize);
	UpdateSubresources(dxCommon_->GetCommandList().Get(), texture.Get(), intermediateResource_[index].Get(), 0, 0, UINT(subresource.size()), subresource.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource_[index];
}
