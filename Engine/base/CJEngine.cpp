#include "CJEngine.h"
#include <assert.h>

void CitrusJunosEngine::Initialize(const char* title, int32_t width, int32_t height) {
	std::string titleWithEnginename = std::string(title) + " with CitrusJunosEngine ";
	auto&& titleString = ConvertString(titleWithEnginename);

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialization(titleString.c_str(), width, height);

	renderer_ = std::make_unique<Renderer>();
	renderer_->Initialize();
}

void CitrusJunosEngine::BeginFrame() {
	dxCommon_->PreDraw();
}

void CitrusJunosEngine::EndFrame() {
	//内部コマンドを生成する
	dxCommon_->PostDraw();
}

void CitrusJunosEngine::Finalize() {
	dxCommon_->Finalize();
}

void CitrusJunosEngine::Update() {

}

CitrusJunosEngine* CitrusJunosEngine::GetInstance() {
	static CitrusJunosEngine instance;
	return &instance;
}

DirectXCommon* CitrusJunosEngine::dxCommon_;