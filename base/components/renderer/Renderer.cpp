#include "Renderer.h"

void Renderer::Initialize(){
	commandList = DirectXCommon::GetInstance()->GetCommandList();
	PSOManager_ = std::make_unique<PSOManager>();
	PSOManager_->Initialize();

}

void Renderer::Draw(PipelineType Type){
	//RootSignatureを設定。PS0とは別途設定が必要
	commandList->SetGraphicsRootSignature(PSOManager_->GetRootSignature(Type).Get());
	//PS0を設定
	commandList->SetPipelineState(PSOManager_->GetPipelineState(Type).Get());
}