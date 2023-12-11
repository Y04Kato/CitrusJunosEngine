#pragma once
#include "DirectXCommon.h"
#include "Vector.h"
#include "CreateTriangle.h"
#include "MathCalculation.h"

#include "DirectXTex/d3dx12.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include<vector>
#include<wrl.h>

enum BlendMode {
	//ブレンドなし
	kBlendModeNone,
	//通常のαブレンド
	kBlendModeNormal,
	//加算ブレンド
	kBlendModeAdd,
	//減産ブレンド
	kBlendModeSubtract,
	//乗算ブレンド
	kBlendModeMultiply,
	//スクリーンブレンド
	kBlendModeScreen,
	//計算用、利用しない
	kCountOfBlendMode,
};

class CitrusJunosEngine {
public:
	static CitrusJunosEngine* GetInstance();

	void Initialize(const char* title, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Finalize();

	void Update();

	void PreDraw3D();
	void PreDraw2D();
	void PreDrawParticle();

	DirectXCommon* GetDirectXCommon() { return dxCommon_; }

	CitrusJunosEngine(const CitrusJunosEngine& obj) = delete;
	CitrusJunosEngine& operator=(const CitrusJunosEngine& obj) = delete;

private:
	CitrusJunosEngine() = default;
	~CitrusJunosEngine() = default;

	static	DirectXCommon* dxCommon_;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;

	IDxcIncludeHandler* includeHandler_;

	D3D12_BLEND_DESC blendDesc_[kCountOfBlendMode];

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	//頂点リソースにデータを書き込む
	Vector4* vertexData_;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};


	IDxcBlob* CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePath,
		//Compielerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	void InitializeDxcCompiler();
	void BlendState();
	void ViewPort();
	void ScissorRect();
	void SettingDepth();

	//3D用
	void CreateRootSignature3D();
	void CreateInputlayOut3D();
	void RasterizerState3D();
	void InitializePSO3D();
	Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob3D_;
	Microsoft::WRL::ComPtr <ID3DBlob> errorBlob3D_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature3D_;

	Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlob3D_;
	Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlob3D_;

	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState3D_;
	D3D12_RASTERIZER_DESC rasterizerDesc3D_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs3D_[3];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc3D_{};

	//2D用
	void CreateRootSignature2D();
	void CreateInputlayOut2D();
	void RasterizerState2D();
	void InitializePSO2D();
	void SettingDepth2D();
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob2D_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob2D_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature2D_;

	IDxcBlob* vertexShaderBlob2D_;
	IDxcBlob* pixelShaderBlob2D_;

	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState2D_;
	D3D12_RASTERIZER_DESC rasterizerDesc2D_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs2D_[2];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc2D_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc2D_{};

	//Particle用
	void CreateRootSignatureParticle();
	void CreateInputlayOutParticle();
	void RasterizerStateParticle();
	void InitializePSOParticle();
	void SettingDepthParticle();
	Microsoft::WRL::ComPtr <ID3DBlob> signatureBlobParticle_;
	Microsoft::WRL::ComPtr <ID3DBlob> errorBlobParticle_;
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureParticle_;

	Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlobParticle_;
	Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlobParticle_;

	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineStateParticle_;
	D3D12_RASTERIZER_DESC rasterizerDescParticle_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescsParticle_[3];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDescParticle_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDescParticle_{};
};