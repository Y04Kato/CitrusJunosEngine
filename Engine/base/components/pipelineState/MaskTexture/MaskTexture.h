/**
 * @file MaskTexture.h
 * @brief MaskTextureを行う為のパイプライン
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "IPipelineStateObject.h"
#include "PSOManager/PSOManager.h"

class MaskTexture : public IPipelineStateObject {
public:
	void ShaderCompile() override;
	void CreateRootSignature() override;
	void CreateInputLayOut() override;
	void CreateBlendState() override;
	void CreateRasterizarState() override;
	void CreatePipelineStateObject() override;

private:

};