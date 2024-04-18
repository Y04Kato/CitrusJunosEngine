#pragma once
#include "DirectXCommon.h"
#include <DirectXTex/DirectXTex.h>
#include "Vector.h"

using namespace Microsoft::WRL;

class PostProsess{
public:
	void Initialization();

private:
	ComPtr<ID3D12Resource> CreateRenderTextureResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	DirectXCommon* dxCommon_;

	HRESULT hr_;

};