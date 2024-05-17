#include "PSOManager.h"

PSOManager* PSOManager::GetInstance(){
	static PSOManager instance;
	return &instance;
}

void PSOManager::Initialize(){
	ShaderCompiler* shaderCompiler = ShaderCompiler::GetInstance();
	shaderCompiler->Initialize();

	//パイプラインの生成
	std::unique_ptr<Standard3D> standard3D = std::make_unique<Standard3D>();
	std::unique_ptr<Standard2D> standard2D = std::make_unique<Standard2D>();
	std::unique_ptr<ParticlePSO> particle = std::make_unique<ParticlePSO>();
	std::unique_ptr<Skinning> skininng = std::make_unique<Skinning>();
	std::unique_ptr<VAT> vat = std::make_unique<VAT>();
	std::unique_ptr<PostProcess> postProcess = std::make_unique<PostProcess>();


	//Initializeの宣言
	standard3D->Initialize();
	standard2D->Initialize();
	particle->Initialize();
	skininng->Initialize();
	vat->Initialize();
	postProcess->Initialize();


	//パイプラインを追加する
	AddPipeline(standard3D->GetPSO(), PipelineType::Standard3D);
	AddPipeline(standard2D->GetPSO(), PipelineType::Standard2D);
	AddPipeline(particle->GetPSO(), PipelineType::Particle);
	AddPipeline(skininng->GetPSO(), PipelineType::Skinning);
	AddPipeline(vat->GetPSO(), PipelineType::VertexAnimationTexture);
	AddPipeline(postProcess->GetPSO(), PipelineType::PostProcess);
}
