#pragma once
#include "DirectXCommon.h"
#include "pipelineState/Standard3D/Standard3D.h"
#include "pipelineState/Standard2D/Standard2D.h"
#include "pipelineState/Particle/Particle.h"
#include "pipelineState/VAT/VAT.h"
#include "pipelineState/PostProcess/PostProcess.h"

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