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
	ImGui::Text("%.02f %.02f %.02f %.02f : Identity", identity.x, identity.y, identity.z, identity.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : Conjugate", conj.x, conj.y, conj.z, conj.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : Inverse", inv.x, inv.y, inv.z, inv.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : Normalize",normal.x,normal.y,normal.z,normal.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : Multiply(q1,q2)",mul1.x,mul1.y,mul1.z,mul1.w);
	ImGui::Text("%.02f %.02f %.02f %.02f : Multiply(q2,q1)",mul2.x,mul2.y,mul2.z,mul2.w);
	ImGui::Text("%.02f                   : Norm",norm);
	ImGui::End();
}

void GameTitleScene::Draw(){

}

void GameTitleScene::Finalize() {
	
}