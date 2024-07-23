#include "PostEffect.h"

PostEffect* PostEffect::GetInstance() {
	static PostEffect instance;
	return &instance;
}

void PostEffect::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	SRVManager_ = SRVManager::GetInstance();

	//一応初期設定でデータ割り当て
	//Mask
	maskTexture_ = textureManager_->Load("project/gamedata/resources/noise0.png");
	thresholdResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(MaskData));
	thresholdResource_->Map(0, NULL, reinterpret_cast<void**>(&maskData_));

	maskData_->maskThreshold = 0.5f;
	maskData_->maskColor = { 0.0f, 1.0f, 0.0f };
	maskData_->edgeColor = { 1.0f,0.4f,0.3f };

	//Random
	randomResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(RandomData));
	randomResource_->Map(0, NULL, reinterpret_cast<void**>(&randomData_));

	randomData_->time = 0.0f;

	//ScanLines
	scanlineResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(ScanlineData));
	scanlineResource_->Map(0, NULL, reinterpret_cast<void**>(&scanlineData_));
	scanlineData_->scanlineIntensity = 0.2f;
	scanlineData_->scanlineFrequency = 1000.0f;
	scanlineData_->time = 0.0f;

	//HSV
	hsvResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(HSVMaterial));
	hsvResource_->Map(0, NULL, reinterpret_cast<void**>(&hsvMaterial_));
	hsvMaterial_->hue = 0.0f;
	hsvMaterial_->saturation = 0.0f;
	hsvMaterial_->value = 0.0f;
}

void PostEffect::Draw(){
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//テクスチャ
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, SRVhandle.GPU);

	//Outlineの時のみ使用
	if (CJEngine_->renderer_->GetNowPipeLineType() == PipelineType::Outline) {

	}

	//MaskTextureの時のみ使用
	if (CJEngine_->renderer_->GetNowPipeLineType() == PipelineType::MaskTexture) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, textureManager_->GetGPUHandle(maskTexture_));
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, thresholdResource_->GetGPUVirtualAddress());
	}

	//Randomの時のみ使用
	randomData_->time++;
	if (CJEngine_->renderer_->GetNowPipeLineType() == PipelineType::Random) {
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, randomResource_->GetGPUVirtualAddress());
	}

	//Scanlineの時のみ使用
	scanlineData_->time++;
	if (CJEngine_->renderer_->GetNowPipeLineType() == PipelineType::Scanlines) {
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, scanlineResource_->GetGPUVirtualAddress());
	}

	//HSVの時のみ使用
	if (CJEngine_->renderer_->GetNowPipeLineType() == PipelineType::HSV) {
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, hsvResource_->GetGPUVirtualAddress());
	}

	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void PostEffect::ALLCreate(){
	CreateRTV();
	CreateSRV();
	CreateDepth();
	CreateViewport();
	CreateScissor();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(renderTextureResource.Get(), &renderTextureSrvDesc, SRVhandle.CPU);

}

void PostEffect::PreDraw(){
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	//指定した色で画面全体をクリア
	float clearColor[] = { 1.0f,0.0f,0.0f,1.0f };//分かりやすく赤にする
	dxCommon_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	//指定した深度で画面全体をクリアする
	dxCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetSrvDescriptiorHeap().Get()};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostEffect::PreCopy(){
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを貼る対象のリソース
	barrier.Transition.pResource = renderTextureResource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
}

void PostEffect::PostCopy(){
	//RenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
}

ComPtr<ID3D12Resource> PostEffect::CreateRenderTextureResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor){
	//利用するheapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	//Resource設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの幅
	resourceDesc.Height = height;//Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行きor配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント　1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2D
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;//DepthStencilとして使う通知

	//クリアの色と設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.num[0];
	clearValue.Color[1] = clearColor.num[1];
	clearValue.Color[2] = clearColor.num[2];
	clearValue.Color[3] = clearColor.num[3];

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr_ = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,// 描画することを前提としたTextureなのでRenderTargetとして使う事から始める
		&clearValue,//Clear最適値
		IID_PPV_ARGS(&resource)//作成するResourceポインタへのポインタ
	);

	return resource;
}


void PostEffect::CreateRTV(){
	//RTVを作る
	const Vector4 kRenderTargetClearValue{ 1.0f,0.0f,0.0f,1.0f };//分かりやすく赤
	renderTextureResource = CreateRenderTextureResource(dxCommon_->GetDevice(), WinApp::GetInstance()->GetClientWidth(), WinApp::GetInstance()->GetClientHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	uint32_t descriptorSizeRTV = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	rtvHandle.ptr = dxCommon_->GetrtvHandles().ptr + descriptorSizeRTV;
	dxCommon_->GetDevice()->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandle);
}

void PostEffect::CreateSRV(){
	//SRVを作る
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	SRVhandle = SRVManager_->GetDescriptorHandle();
	SRVhandle.CPU.ptr += SRVManager_->descriptorSizeSRV;
	SRVhandle.GPU.ptr += SRVManager_->descriptorSizeSRV;
}

void PostEffect::CreateDepth(){
	//DSVDescriptorHeap
	depthStencilResource = dxCommon_->CreateDepthStenciltextureResource(dxCommon_->GetDevice(), WinApp::GetInstance()->GetClientWidth(), WinApp::GetInstance()->GetClientHeight());
	//DSV用のヒープでディスクリプタの数は1　DSVはShader内で触るものではないのでShaderVisibleはfalse
	dsvDescriptorHeap = dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(),D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format 基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	dxCommon_->GetDevice()->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::CreateViewport(){
	viewport.Width = (float)WinApp::GetInstance()->GetClientWidth();
	viewport.Height = (float)WinApp::GetInstance()->GetClientHeight();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void PostEffect::CreateScissor(){
	scissorRect.left = 0;
	scissorRect.right = WinApp::WinApp::GetInstance()->GetClientWidth();
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::GetInstance()->GetClientHeight();
}
