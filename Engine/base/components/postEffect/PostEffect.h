#pragma once
#include "CJEngine.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Vector.h"
#include "SRVManager/SRVManager.h"

#include <DirectXTex/DirectXTex.h>

using namespace Microsoft::WRL;

class PostEffect{
public:
	static PostEffect* GetInstance();

	void Initialize();
	void Draw();
	void ALLCreate();
	void PreDraw();
	void PreCopy();
	void PostCopy();

	//MaskTexture割り当て用
	void SetMaskTexture(uint32_t maskTexture) { maskTexture_ = maskTexture; }
	//MaskData割り当て用
	void SetMaskData(const MaskData& maskData) {*maskData_ = maskData; }
	//ScanlineData割り当て用
	void SetScanlineData(const ScanlineData& scanlineData) { *scanlineData_ = scanlineData; }
	//HSVMaterial割り当て用
	void SetHSVMaterial(const HSVMaterial& hsvMaterial) { *hsvMaterial_ = hsvMaterial; }

	PostEffect(const PostEffect& obj) = delete;
	PostEffect& operator=(const PostEffect& obj) = delete;

private:
	void CreateRTV();
	void CreateSRV();
	void CreateDepth();
	void CreateViewport();
	void CreateScissor();

private:
	PostEffect() = default;
	~PostEffect() = default;

	ComPtr<ID3D12Resource> CreateRenderTextureResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	CitrusJunosEngine* CJEngine_;

	DirectXCommon* dxCommon_;

	TextureManager* textureManager_;

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

	//Mask用Texture
	uint32_t maskTexture_;
	//Mask用の閾値
	Microsoft::WRL::ComPtr<ID3D12Resource> thresholdResource_;
	MaskData* maskData_;

	//Random用
	Microsoft::WRL::ComPtr<ID3D12Resource> randomResource_;
	RandomData* randomData_;

	//Random用
	Microsoft::WRL::ComPtr<ID3D12Resource> scanlineResource_;
	ScanlineData* scanlineData_;

	//HSV用
	Microsoft::WRL::ComPtr<ID3D12Resource> hsvResource_;
	HSVMaterial* hsvMaterial_;
};

