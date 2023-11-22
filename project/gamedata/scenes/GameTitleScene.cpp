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

	Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis_, angle_);

	ImGui::Begin("RotateMatrix");
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[0][0],rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[1][0],rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[2][0],rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[3][0],rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::End();
}

void GameTitleScene::Draw(){

}

void GameTitleScene::Finalize() {
	
}