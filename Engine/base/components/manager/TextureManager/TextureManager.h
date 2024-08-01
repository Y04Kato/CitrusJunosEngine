#pragma once
#include "DirectXCommon.h"
#include "SRVManager/SRVManager.h"
#include <DirectXTex/DirectXTex.h>
#include <wrl.h>
#include <array>

class TextureManager {
public:
	void Initialize();
	static TextureManager* GetInstance();
	static const int maxtex = 256;
	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);

	//テクスチャの読み込み、0番はwhiteがInitialize時に入っているので1番から割り当て開始
	uint32_t Load(const std::string& filePath); 

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_[maxtex];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[maxtex];

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorheap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorheap, uint32_t descriptorSize, uint32_t index);

	Microsoft::WRL::ComPtr <ID3D12Resource> GetTextureBuffer(uint32_t index)const { return textureResource_[index].Get(); }

	uint32_t GetTextureIndex() { return textureIndex_; }
	void SetTextureIndex(uint32_t textureIndex);

	uint32_t GetDescriptorSizeSRV() { return descriptorSizeSRV; }
	uint32_t GetDescriptorSizeRTV() { return descriptorSizeRTV; }
	uint32_t GetDescriptorSizeDSV() { return descriptorSizeDSV; }

	uint32_t white;
	uint32_t ddsSample;

private:
	DirectXCommon* dxCommon_;
	SRVManager* SRVManager_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>intermediateResource_[maxtex];
	Microsoft::WRL::ComPtr<ID3D12Resource>textureResource_[maxtex];
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	uint32_t textureIndex_;

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr <ID3D12Device> device, const DirectX::TexMetadata& metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr <ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, uint32_t index);
	DirectX::ScratchImage  LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& filePath, uint32_t index);
};
