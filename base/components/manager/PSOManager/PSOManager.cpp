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
	std::unique_ptr<VAT> vat = std::make_unique<VAT>();
	std::unique_ptr<PostProcess> postProcess = std::make_unique<PostProcess>();
	std::unique_ptr<Grayscale> grayscale = std::make_unique<Grayscale>();
	std::unique_ptr<Vignette> vignette = std::make_unique<Vignette>();


	//Initializeの宣言
	standard3D->Initialize();
	standard2D->Initialize();
	particle->Initialize();
	vat->Initialize();
	postProcess->Initialize();
	grayscale->Initialize();
	vignette->Initialize();


	//パイプラインを追加する
	AddPipeline(standard3D->GetPSO(), PipelineType::Standard3D);
	AddPipeline(standard2D->GetPSO(), PipelineType::Standard2D);
	AddPipeline(particle->GetPSO(), PipelineType::Particle);
	AddPipeline(vat->GetPSO(), PipelineType::VertexAnimationTexture);
	AddPipeline(postProcess->GetPSO(), PipelineType::PostProcess);
	AddPipeline(grayscale->GetPSO(), PipelineType::Grayscale);
	AddPipeline(vignette->GetPSO(), PipelineType::Vignette);
}
