#pragma once
#include "WinApp.h"
#include "ConvertString.h"

#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include<wrl.h>
#include<chrono>

class DirectXCommon {
public:
	static DirectXCommon* GetInstance();

	void Initialization(const wchar_t* title, int32_t backBufferWidth = WinApp::kClientWidth, int32_t backBufferHeight = WinApp::kClientHeight);

	void ImGuiInitialize();

	void PreDraw();
	void PostDraw();

	void ClearRenderTarget();
	void Finalize();

	static Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

	WinApp* GetWin() { return WinApp::GetInstance(); }

	HRESULT GetHr() { return  hr_; }
	void SetHr(HRESULT a) { this->hr_ = a; }
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptiorHeap() { return srvDescriptorHeap_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptiorHeap() { return dsvDescriptorHeap_; }
	D3D12_RENDER_TARGET_VIEW_DESC getRtvDesc() { return rtvDesc_; }

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
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap_;//srv用

	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	Microsoft::WRL::ComPtr <ID3D12Resource> swapChainResources_[2];

	//Fence
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;

	int32_t backBufferWidth_;
	int32_t backBufferHeight_;

	D3D12_RESOURCE_BARRIER barrier_{};

	HRESULT hr_;

	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle_;

	std::chrono::steady_clock::time_point reference_;

private:
	void InitializeDXGIDevice();

	void CreateSwapChain();

	void InitializeCommand();

	void CreateFinalRenderTargets();

	void CreateFence();

	Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStenciltextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	void CreateDepthStensil();

	void InitializeFixFPS();

	void UpdateFixFPS();
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
