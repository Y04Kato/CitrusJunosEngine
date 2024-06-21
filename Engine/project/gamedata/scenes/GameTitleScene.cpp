#include"GameTitleScene.h"

void GameTitleScene::Initialize(){
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();
	
	//Input
	input_ = Input::GetInstance();
}

void GameTitleScene::Update(){
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	ImGui::Begin("debug");
	ImGui::Text("GameTitleScene");
	ImGui::Text("DemoScene:N key or A button");
	ImGui::End();

	if (input_->TriggerKey(DIK_N)) {
		sceneNo = DEMO_SCENE;
	}

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (input_->PushAButton(joyState)) {
		sceneNo = DEMO_SCENE;
	}
}

void GameTitleScene::Draw(){
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GameTitleScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GameTitleScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void GameTitleScene::Finalize() {
	
}