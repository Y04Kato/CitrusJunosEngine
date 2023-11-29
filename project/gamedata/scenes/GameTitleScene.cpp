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

	Matrix4x4 rotateMatrix0 = DirectionToDirection(Normalize(Vector3{ 1.0f,0.0f,0.0f }), Normalize(Vector3{ -1.0f,0.0f,0.0f }));
	Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);

	ImGui::Begin("RotateMatrix");
	ImGui::Text("rotateMatrix0");
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix0.m[0][0],rotateMatrix0.m[0][1], rotateMatrix0.m[0][2], rotateMatrix0.m[0][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix0.m[1][0],rotateMatrix0.m[1][1], rotateMatrix0.m[1][2], rotateMatrix0.m[1][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix0.m[2][0],rotateMatrix0.m[2][1], rotateMatrix0.m[2][2], rotateMatrix0.m[2][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix0.m[3][0],rotateMatrix0.m[3][1], rotateMatrix0.m[3][2], rotateMatrix0.m[3][3]);
	ImGui::Text("rotateMatrix1");
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix1.m[0][0], rotateMatrix1.m[0][1], rotateMatrix1.m[0][2], rotateMatrix1.m[0][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix1.m[1][0], rotateMatrix1.m[1][1], rotateMatrix1.m[1][2], rotateMatrix1.m[1][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix1.m[2][0], rotateMatrix1.m[2][1], rotateMatrix1.m[2][2], rotateMatrix1.m[2][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix1.m[3][0], rotateMatrix1.m[3][1], rotateMatrix1.m[3][2], rotateMatrix1.m[3][3]);
	ImGui::Text("rotateMatrix2");
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix2.m[0][0], rotateMatrix2.m[0][1], rotateMatrix2.m[0][2], rotateMatrix2.m[0][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix2.m[1][0], rotateMatrix2.m[1][1], rotateMatrix2.m[1][2], rotateMatrix2.m[1][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix2.m[2][0], rotateMatrix2.m[2][1], rotateMatrix2.m[2][2], rotateMatrix2.m[2][3]);
	ImGui::Text("%.03f %.03f %.03f %.03f", rotateMatrix2.m[3][0], rotateMatrix2.m[3][1], rotateMatrix2.m[3][2], rotateMatrix2.m[3][3]);
	ImGui::End();
}

void GameTitleScene::Draw(){

}

void GameTitleScene::Finalize() {
	
}