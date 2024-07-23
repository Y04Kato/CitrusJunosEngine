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
	std::unique_ptr<SkyBox> skyBox = std::make_unique<SkyBox>();
	std::unique_ptr<VAT> vat = std::make_unique<VAT>();
	std::unique_ptr<PostProcess> postProcess = std::make_unique<PostProcess>();
	std::unique_ptr<Grayscale> grayscale = std::make_unique<Grayscale>();
	std::unique_ptr<Vignette> vignette = std::make_unique<Vignette>();
	std::unique_ptr<Smoothing> smoothing = std::make_unique<Smoothing>();
	std::unique_ptr<Gaussian> gaussian = std::make_unique<Gaussian>();
	std::unique_ptr<Outline> outline = std::make_unique<Outline>();
	std::unique_ptr<RadialBlur> radialBlur = std::make_unique<RadialBlur>();
	std::unique_ptr<MaskTexture> maskTexture = std::make_unique<MaskTexture>();
	std::unique_ptr<Random> random = std::make_unique<Random>();
	std::unique_ptr<LensDistortion> lensDistortion = std::make_unique<LensDistortion>();
	std::unique_ptr<Scanlines> scanlines = std::make_unique<Scanlines>();
	std::unique_ptr<HSV> hsv = std::make_unique<HSV>();


	//Initializeの宣言
	standard3D->Initialize();
	standard2D->Initialize();
	particle->Initialize();
	skininng->Initialize();
	skyBox->Initialize();
	vat->Initialize();
	postProcess->Initialize();
	grayscale->Initialize();
	vignette->Initialize();
	smoothing->Initialize();
	gaussian->Initialize();
	outline->Initialize();
	radialBlur->Initialize();
	maskTexture->Initialize();
	random->Initialize();
	lensDistortion->Initialize();
	scanlines->Initialize();
	hsv->Initialize();


	//パイプラインを追加する
	AddPipeline(standard3D->GetPSO(), PipelineType::Standard3D);
	AddPipeline(standard2D->GetPSO(), PipelineType::Standard2D);
	AddPipeline(particle->GetPSO(), PipelineType::Particle);
	AddPipeline(skininng->GetPSO(), PipelineType::Skinning);
	AddPipeline(skyBox->GetPSO(), PipelineType::SkyBox);
	AddPipeline(vat->GetPSO(), PipelineType::VertexAnimationTexture);
	AddPipeline(postProcess->GetPSO(), PipelineType::PostProcess);
	AddPipeline(grayscale->GetPSO(), PipelineType::Grayscale);
	AddPipeline(vignette->GetPSO(), PipelineType::Vignette);
	AddPipeline(smoothing->GetPSO(), PipelineType::Smoothing);
	AddPipeline(gaussian->GetPSO(), PipelineType::Gaussian);
	AddPipeline(outline->GetPSO(), PipelineType::Outline);
	AddPipeline(radialBlur->GetPSO(), PipelineType::RadialBlur);
	AddPipeline(maskTexture->GetPSO(), PipelineType::MaskTexture);
	AddPipeline(random->GetPSO(), PipelineType::Random);
	AddPipeline(lensDistortion->GetPSO(), PipelineType::LensDistortion);
	AddPipeline(scanlines->GetPSO(), PipelineType::Scanlines);
	AddPipeline(hsv->GetPSO(), PipelineType::HSV);
}
