/**
 * @file PSOManager.h
 * @brief 各種パイプラインの生成、その後の管理を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "pipelineState/Standard3D/Standard3D.h"
#include "pipelineState/Standard2D/Standard2D.h"
#include "pipelineState/Particle/Particle.h"
#include "pipelineState/Skinning/Skinning.h"
#include "pipelineState/SkyBox/SkyBox.h"
#include "pipelineState/VAT/VAT.h"
#include "pipelineState/PostProcess/PostProcess.h"
#include "pipelineState/Grayscale/Grayscale.h"
#include "pipelineState/Vignette/Vignette.h"
#include "pipelineState/Smoothing/Smoothing.h"
#include "pipelineState/Gaussian/Gaussian.h"
#include "pipelineState/Outline/Outline.h"
#include "pipelineState/RadialBlur/RadialBlur.h"
#include "pipelineState/MaskTexture/MaskTexture.h"
#include "pipelineState/Random/Random.h"
#include "pipelineState/LensDistortion/LensDistortion.h"
#include "pipelineState/Scanlines/Scanlines.h"
#include "pipelineState/HSV/HSV.h"

#include <map>

class PSOManager{
public:
	static PSOManager* GetInstance();

	void Initialize();

	//現在のルートシグネイチャーを取得する
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(PipelineType Type) { return Pipeline_[Type].rootSignature.Get(); }
	
	//現在のパイプラインステートを取得する
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState(PipelineType Type) { return Pipeline_[Type].graphicsPipelineState.Get(); }

	//パイプラインを追加する
	void AddPipeline(PipelineStateObject Pipeline, PipelineType Type) {
		Pipeline_[Type] = Pipeline;
	}

private:
	std::map<PipelineType, PipelineStateObject> Pipeline_;

};
