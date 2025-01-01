#include "SceneNumber.h"

SceneNumber* SceneNumber::GetInstance() {
	static SceneNumber instance;

	return &instance;
}

void SceneNumber::Initialize(const int nowSceneNo) {
	sceneNo_ = nowSceneNo;
}

void SceneNumber::SetSceneNumber(const int nextSceneNo) {
	sceneNo_ = nextSceneNo;
}