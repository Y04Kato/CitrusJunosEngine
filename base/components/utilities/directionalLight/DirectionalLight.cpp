#include "DirectionalLight.h"

DirectionalLights* DirectionalLights::GetInstance() {
	static DirectionalLights instance;

	return &instance;
}

void DirectionalLights::Initialize() {
	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{-0.167f,-0.619f,0.767f},1.0f };
}

void DirectionalLights::Update() {
	directionalLight_.direction = Normalize(directionalLight_.direction);
}