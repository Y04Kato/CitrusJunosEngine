#include "Gaussian.h"

void Gaussian::ShaderCompile() {
	vertexShaderBlob = ShaderCompiler::GetInstance()->CompileShader(L"project/gamedata/resources/shaders/CopyImage.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	pixelShaderBlob = ShaderCompiler::GetInstance()->CompileShader(L"project/gamedata/resources/shaders/GaussianFilter.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Gaussian::CreateRootSignature() {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_PARAMETER rootParameters[1] = {};

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ
	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	//DescriptorTable
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0番
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&PipelineStateObject_.rootSignature));
	assert(SUCCEEDED(hr));
}

void Gaussian::CreateInputLayOut() {
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;
}

void Gaussian::CreateBlendState() {
	//すべての色要素を書き込む
	//何もなし
	blendDesc[kBlendModeNone].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	//通常ブレンド
	blendDesc[kBlendModeNormal].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[kBlendModeNormal].RenderTarget[0].BlendEnable = TRUE;
	blendDesc[kBlendModeNormal].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[kBlendModeNormal].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeNormal].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc[kBlendModeNormal].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc[kBlendModeNormal].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeNormal].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//加算ブレンド
	blendDesc[kBlendModeAdd].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[kBlendModeAdd].RenderTarget[0].BlendEnable = TRUE;
	blendDesc[kBlendModeAdd].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[kBlendModeAdd].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeAdd].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc[kBlendModeAdd].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc[kBlendModeAdd].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeAdd].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//減算ブレンド
	blendDesc[kBlendModeSubtract].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[kBlendModeSubtract].RenderTarget[0].BlendEnable = TRUE;
	blendDesc[kBlendModeSubtract].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc[kBlendModeSubtract].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc[kBlendModeSubtract].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc[kBlendModeSubtract].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc[kBlendModeSubtract].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeSubtract].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//乗算ブレンド
	blendDesc[kBlendModeMultiply].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[kBlendModeMultiply].RenderTarget[0].BlendEnable = TRUE;
	blendDesc[kBlendModeMultiply].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc[kBlendModeMultiply].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeMultiply].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
	blendDesc[kBlendModeMultiply].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc[kBlendModeMultiply].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeMultiply].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//スクリーンブレンド
	blendDesc[kBlendModeScreen].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[kBlendModeScreen].RenderTarget[0].BlendEnable = TRUE;
	blendDesc[kBlendModeScreen].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc[kBlendModeScreen].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeScreen].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc[kBlendModeScreen].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc[kBlendModeScreen].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc[kBlendModeScreen].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void Gaussian::CreateRasterizarState() {
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void Gaussian::CreatePipelineStateObject() {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = PipelineStateObject_.rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc[kBlendModeNormal];//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//rasterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&PipelineStateObject_.graphicsPipelineState));
	assert(SUCCEEDED(hr));
}
