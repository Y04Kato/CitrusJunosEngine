/**
 * @file IPipeLineStateObject.h
 * @brief パイプラインの基本的な機能をまとめた継承部分
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "ShaderCompiler.h"
#include <array>

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

class IPipelineStateObject{
public:
	~IPipelineStateObject() {};

	void Initialize() {
		ShaderCompile();
		CreateRootSignature();
		CreateInputLayOut();
		CreateBlendState();
		CreateRasterizarState();
		CreatePipelineStateObject();
	}

	virtual void ShaderCompile() = 0;//シェーダーを読み込む
	virtual void CreateRootSignature() = 0;//ルートシグネイチャーの設定
	virtual void CreateInputLayOut() = 0;//インプットレイアウトの設定
	virtual void CreateBlendState() = 0;//ブレンドの設定
	virtual void CreateRasterizarState() = 0;//何処まで描画するかの設定
	virtual void CreatePipelineStateObject() = 0;//パイプラインステートオブジェクトを生成する

	PipelineStateObject GetPSO() { return PipelineStateObject_; }

protected:
	HRESULT hr;

	IDxcBlob* vertexShaderBlob = nullptr;
	IDxcBlob* pixelShaderBlob = nullptr;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs5[5] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs3[3] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs2[2] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	D3D12_BLEND_DESC blendDesc[kCountOfBlendMode];

	D3D12_RASTERIZER_DESC rasterizerDesc{};

	PipelineStateObject PipelineStateObject_;

};