#pragma once
#include "DirectXCommon.h"
#include "Vector.h"
#include "SRVManager/SRVManager.h"
#include "PostEffectList.h"

#include <DirectXTex/DirectXTex.h>

using namespace Microsoft::WRL;

class PostEffect{
public:
	void Initialize();
	void Draw();
	void ALLCreate();
	void PreDraw();
	void PreCopy();
	void PostCopy();

private:
	void CreateRTV();
	void CreateSRV();
	void CreateDepth();
	void CreateViewport();
	void CreateScissor();

private:
	ComPtr<ID3D12Resource> CreateRenderTextureResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	DirectXCommon* dxCommon_;

	HRESULT hr_;

	SRVManager* SRVManager_ = nullptr;

	//結果が入る、S_OKなら成功
	HRESULT hr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	//RTV
	ComPtr<ID3D12Resource> renderTextureResource = nullptr;
	//SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	DESCRIPTERHANDLE SRVhandle;
	//DSV関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>depthStencilResource = nullptr;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	D3D12_RESOURCE_BARRIER barrier{};

	//ListFlags
	PostEffectList* postEffectList_;
	PostEffectLists* postEffectLists_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12Resource> postEffectListResource_ = nullptr;
};