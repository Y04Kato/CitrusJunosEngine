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
	ImGui::Text("%.02f %.02f %.02f %.02f : Rotation", rotation.x, rotation.y, rotation.z, rotation.w);
	ImGui::Text("RotateMatrix");
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::Text("%.02f %.02f %.02f : RotateByQuaternion", rotateByQuaternion.num[0], rotateByQuaternion.num[1], rotateByQuaternion.num[2]);
	ImGui::Text("%.02f %.02f %.02f : RotateByMatrix", rotateByMatrix.num[0], rotateByMatrix.num[1], rotateByMatrix.num[2]);
	ImGui::End();
}

void GameTitleScene::Draw(){

}

void GameTitleScene::Finalize() {
	
}