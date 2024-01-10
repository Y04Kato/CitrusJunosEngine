#include"GameTitleScene.h"

void GameTitleScene::Initialize(){
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();
	
	//Input
	input_ = Input::GetInstance();
}

void GameTitleScene::Update(){
	if (input_->TriggerKey(DIK_N)) {
		sceneNo = GAME_SCENE;
	}

	ImGui::Begin("debug");
	ImGui::Text("GameTitleScene");
	ImGui::Text("nextScene:pressKey N");
	ImGui::End();

	ImGui::Begin("Quaternion");
	ImGui::Text("%.02f %.02f %.02f %.02f : interpolate0, Slerp(q0,q1,0.0f)", interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : interpolate1, Slerp(q0,q1,0.3f)", interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : interpolate2, Slerp(q0,q1,0.5f)", interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : interpolate3, Slerp(q0,q1,0.7f)", interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : interpolate4, Slerp(q0,q1,1.0f)", interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);
	ImGui::End();
}

void GameTitleScene::Draw(){

}

void GameTitleScene::Finalize() {
	
}