#pragma once
#include "WinApp.h"
#include "ConvertString.h"
#include "PipeLineList.h"

#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <thread>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "DirectXTex/d3dx12.h"

#include <dwrite.h>
#pragma comment(lib, "Dwrite.lib")

struct PipelineStateObject {
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
};

class DirectXCommon {
public:
	static DirectXCommon* GetInstance();

	void Initialization(const wchar_t* title, int32_t backBufferWidth = WinApp::GetInstance()->GetClientWidth(), int32_t backBufferHeight = WinApp::GetInstance()->GetClientHeight());

	void PreDraw();
	void PostDraw();

	void Finalize();

	//Getter&Setter
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }

	WinApp* GetWin() { return WinApp::GetInstance(); }
	HRESULT GetHr() { return  hr_; }
	void SetHr(HRESULT a) { this->hr_ = a; }

	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc()const { return rtvDesc_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetrtvHandles()const { return rtvHandles_[1]; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptiorHeap() { return srvDescriptorHeap_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptiorHeap() { return dsvDescriptorHeap_; }

	Microsoft::WRL::ComPtr <IDXGISwapChain4> GetSwapChain() { return swapChain_; }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const { return swapChainDesc_; }

	//Create
	static Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);
	
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStenciltextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);


	DirectXCommon(const DirectXCommon& obj) = delete;
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;

private:
	DirectXCommon() = default;
	~DirectXCommon() = default;

	//DXGIファクトリーの生成
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory_;

	//使用するアダプタ用の変数
	Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_;

	//D3D12Deviceの生成
	Microsoft::WRL::ComPtr <ID3D12Device> device_;

	//コマンドキュー生成
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue_;

	//コマンドアロケータの生成
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator_;

	//コマンドリストを生成する
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;

	//スワップチェーン
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap_;//rtv用
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap_;//srv用

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap_;//dsv用
	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle_;

	Microsoft::WRL::ComPtr <ID3D12Resource> swapChainResources_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;//RTVハンドル先頭
	uint32_t descriptorSizeRTV_;//RTVのサイズ

	//Fence
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;

	D3D12_RESOURCE_BARRIER barrier_{};

	HRESULT hr_;

	D3D12_VIEWPORT viewport_{};

	D3D12_RECT scissorRect_{};

	std::chrono::steady_clock::time_point reference_;

	int32_t backBufferWidth_;
	int32_t backBufferHeight_;

	//テキスト関連
	Microsoft::WRL::ComPtr <IDWriteFactory> pDWriteFactory_;
	Microsoft::WRL::ComPtr <IDWriteTextFormat> pTextFormat_;

	const wchar_t* wszText_;
	UINT32 cTextLength_;

	Microsoft::WRL::ComPtr <ID2D1Factory> pD2DFactory_;
	Microsoft::WRL::ComPtr <ID2D1HwndRenderTarget> pRT_;
	Microsoft::WRL::ComPtr <ID2D1SolidColorBrush> pBlackBrush_;

private:
	void CreateDXGIDevice();

	void CreateSwapChain();

	void CreateCommand();

	void CreateFinalRenderTargets();

	void CreateFence();

	void CreateDepthStensil();

	void CreateViewport();
	void CreateScissor();

	//FPS固定初期化&更新
	void InitializeFixFPS();
	void UpdateFixFPS();

	void InitializeTextFactory();
	void CreateTextRenderTargets();
	void CreateTextVertex();

	void InitializeDXGIDevice11();
};

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		////リソースリークチェック
		Microsoft::WRL::ComPtr <IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};
