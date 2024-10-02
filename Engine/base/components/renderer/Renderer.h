/**
 * @file Renderer.h
 * @brief 各種パイプラインの描画を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "PSOManager/PSOManager.h"

class Renderer{
public:
	void Initialize();
	void Draw(PipelineType type);

	PipelineType GetNowPipeLineType() { return nowPipeLineType_; }

private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList = nullptr;
	std::unique_ptr<PSOManager> PSOManager_ = nullptr;

	PipelineType nowPipeLineType_ = PipelineType::Standard3D;
};
