#include "DirectXCommon.h"
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

void DirectXCommon::Initialization(const wchar_t* title, int32_t backBufferWidth, int32_t backBufferHeight) {
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	//FPS固定初期化
	InitializeFixFPS();

	WinApp::GetInstance()->CreateWindowView(title, backBufferWidth_, backBufferHeight_);

	//DXGIデバイス初期化
	InitializeDXGIDevice();

	//コマンド関連初期化
	InitializeCommand();

	//スワップチェーンの生成
	CreateSwapChain();

	//レンダーターゲット生成
	CreateFinalRenderTargets();

	//フェンス生成
	CreateFence();

	//深度設定
	CreateDepthStensil();

	//テキスト関係
	InitializeTextFactory();
	CreateTextRenderTargets();
	CreateTextVertex();
}

void DirectXCommon::InitializeDXGIDevice() {
	//DXGIファクトリーの生成
	dxgiFactory_ = nullptr;
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr_));

	//使用するアダプタ用の変数
	useAdapter_ = nullptr;
	//順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//アダプター情報の取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));

		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用アダプタの情報をログに出力
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;//ソフトウェアアダプタの場合は無視
	}
	//適切なアダプタがないため起動しない
	assert(useAdapter_ != nullptr);

	//D3D12Deviceの生成
	device_ = nullptr;
	//機能レベルとログ出力
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelString[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか確認
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//採用したアダプターでデバイス生成
		hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルのデバイス生成に成功したか確認
		if (SUCCEEDED(hr_)) {
			//生成できたのでログ出力をしてループ抜け
			Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
			break;
		}
	}

	//デバイス生成失敗の為起動しない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!\n");//初期化完了ログ

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバイエラーで止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラーで止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//解放
		infoQueue->Release();
	}
#endif // _DEBUG
}

void DirectXCommon::InitializeCommand() {
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成失敗の為、起動しない
	assert(SUCCEEDED(hr_));

	//コマンドアロケータの生成
	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr_));

	//コマンドリストを生成する
	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリスト生成失敗の為、起動しない
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateSwapChain() {
	//スワップチェーン
	swapChain_ = nullptr;
	swapChainDesc_.Width = WinApp::kClientWidth;//画面の幅
	swapChainDesc_.Height = WinApp::kClientHeight;//画面の高さ
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc_.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用
	swapChainDesc_.BufferCount = 2;//ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに移したら中身を破棄

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), WinApp::GetInstance()->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr_));

	//RTV用ディスクリプタヒープの生成
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SwapChainからResourceを引っ張ってくる
	swapChainResources_[0] = { nullptr };
	swapChainResources_[1] = { nullptr };
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	//取得できなければ起動しない
	assert(SUCCEEDED(hr_));

	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));

	//SRV用ディスクリプタヒープの生成
	srvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
}

Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptionHeapDesc{};
	descriptionHeapDesc.Type = heapType;//レンダーターゲットビュー用
	descriptionHeapDesc.NumDescriptors = numDescriptors;//ダブルバッファ用に二つ。多くても別にかまわない
	descriptionHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptionHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが生成失敗の為、起動しない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::CreateFinalRenderTargets() {
	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	//まず一つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

void DirectXCommon::CreateFence() {
	//初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//Fenceのsignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::PreDraw() {
	//書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//今回のbarrierはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにする
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier対象のリソース、バックばっがに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//遷移前のresourcestate
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のresourcestate
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色、RGBA順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	dsvhandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvhandle_);
	commandList_->ClearDepthStencilView(dsvhandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::PostDraw() {
	hr_;
	//画面描画処理の終わり、状態を遷移
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
	//コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseする
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	//GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);
	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまで辿り着いた時、Fenceの値を指定した値に代入するようにsignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//FPS固定
	UpdateFixFPS();

	//次のフレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::ClearRenderTarget() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);
}

Microsoft::WRL::ComPtr <ID3D12Resource> DirectXCommon::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes;//リソースサイズ
	//バッファの場合はこれらは１にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;

	Microsoft::WRL::ComPtr <ID3D12Resource> Resource = nullptr;
	//実際に頂点リソースを作る
	hr = device->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}

Microsoft::WRL::ComPtr <ID3D12Resource> DirectXCommon::CreateDepthStenciltextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
	//生成するresourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//textureの幅
	resourceDesc.Height = height;//textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行きor配列textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//resourceの設定
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定、特になし
		&resourceDesc,//resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込める状態にしておく
		&depthClearValue,//Clear最適値
		IID_PPV_ARGS(&resource));//作成するresourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

void DirectXCommon::CreateDepthStensil() {
	depthStencilResource_ = CreateDepthStenciltextureResource(device_, WinApp::kClientWidth, WinApp::kClientHeight);
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc{};
	dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format、基本的にresourceに合わせる
	dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture

	device_->CreateDepthStencilView(depthStencilResource_.Get(),
		&dsvdesc,
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::Finalize() {
	CloseHandle(fenceEvent_);
#ifdef DEBUG
	winApp_->GetdebugController()->Release();
#endif // DEBUG

	CloseWindow(WinApp::GetInstance()->GetHwnd());
}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void::DirectXCommon::InitializeFixFPS() {
	reference_ = std::chrono::steady_clock::now();
}

void::DirectXCommon::UpdateFixFPS() {
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得するn
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinTime) {
		//1/60秒経過するまでスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//現在の時間を取得する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::InitializeTextFactory() {
	//ID2D1Factoryインターフェイスの作成
	hr_ = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		pD2DFactory_.GetAddressOf()
	);

	//IDWriteFactoryインターフェイスの作成
	if (SUCCEEDED(hr_)) {
		hr_ = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(pDWriteFactory_.GetAddressOf())
		);
	}

	//テキスト文字列の初期化及び、長さの格納
	wszText_ = L"Hello World using  DirectWrite!";
	cTextLength_ = (UINT32)wcslen(wszText_);

	//IDWriteTextFormatインターフェイスオブジェクトの作成
	//使用されるフォント、太さ、ストレッチ、スタイル、ロケールを指定する
	if (SUCCEEDED(hr_)) {
		hr_ = pDWriteFactory_->CreateTextFormat(
			L"Gabriola",//Font Name
			NULL,//Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			L"en-us",
			&pTextFormat_
		);
	}

	//テキストを水平、垂直方向に中央揃え
	if (SUCCEEDED(hr_)) {
		hr_ = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr_)) {
		hr_ = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

}

void DirectXCommon::CreateTextRenderTargets() {
	RECT rc;
	GetClientRect(WinApp::GetInstance()->GetHwnd(), &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	if (!pRT_) {
		//Direct2D RenderTargetの生成
		hr_ = pD2DFactory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				WinApp::GetInstance()->GetHwnd(),
				size
			),
			&pRT_
		);

		//ブラシ設定、今回は純色で塗りつぶし
		if (SUCCEEDED(hr_)) {
			hr_ = pRT_->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&pBlackBrush_
			);
		}
	}
}

void DirectXCommon::CreateTextVertex() {
	/*D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / dpiScaleX_,
		static_cast<FLOAT>(rc.top) / dpiScaleY_,
		static_cast<FLOAT>(rc.right - rc.left) / dpiScaleX_,
		static_cast<FLOAT>(rc.bottom - rc.top) / dpiScaleY_
	);*/
}

void DirectXCommon::InitializeDXGIDevice11() {
	//D3D11On12Deviceの生成
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

	d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));

	/*if (m_useWarpDevice){
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_));
	}
	else{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		D3D12CreateDevice(hardwareAdapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device_));
	}*/


}