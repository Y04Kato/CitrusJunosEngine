#pragma once
#include "IPipelineStateObject.h"
#include "PSOManager/PSOManager.h"

class Standard2D : public IPipelineStateObject {
public:
	void ShaderCompile() override;
	void CreateRootSignature() override;
	void CreateInputLayOut() override;
	void CreateBlendState() override;
	void CreateRasterizarState() override;
	void CreatePipelineStateObject() override;

private:

};