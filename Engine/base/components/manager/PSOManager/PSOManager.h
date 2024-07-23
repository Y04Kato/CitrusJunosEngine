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

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(PipelineType Type) { return Pipeline_[Type].rootSignature.Get(); }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState(PipelineType Type) { return Pipeline_[Type].graphicsPipelineState.Get(); }

	void AddPipeline(PipelineStateObject Pipeline, PipelineType Type) {
		Pipeline_[Type] = Pipeline;
	}

private:
	std::map<PipelineType, PipelineStateObject> Pipeline_;

};
