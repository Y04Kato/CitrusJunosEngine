/**
 * @file CJEngine.cpp
 * @brief 各種フレーム処理、描画処理、終了処理をサポート
 * @author KATO
 * @date 未記録
 */

#include "CJEngine.h"
#include <assert.h>

void CitrusJunosEngine::Initialize(const char* title, int32_t width, int32_t height) {
	//指定されたタイトルにエンジン名表記を追加
	std::string titleWithEnginename = std::string(title) + " with CitrusJunosEngine ";
	auto&& titleString = ConvertString(titleWithEnginename);

	//DirectXCommon
	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialization(titleString.c_str(), width, height);

	//Renderer
	renderer_ = std::make_unique<Renderer>();
	renderer_->Initialize();

	//Audio
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//Input
	input_ = Input::GetInstance();
	input_->Initialize();
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
	audio_->Finalize();
}

void CitrusJunosEngine::Update() {
	input_->Update();
}

CitrusJunosEngine* CitrusJunosEngine::GetInstance() {
	static CitrusJunosEngine instance;
	return &instance;
}

DirectXCommon* CitrusJunosEngine::dxCommon_;