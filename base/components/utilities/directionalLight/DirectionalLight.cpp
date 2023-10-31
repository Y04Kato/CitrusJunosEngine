#include "DirectionalLight.h"

DirectionalLights* DirectionalLights::GetInstance() {
	static DirectionalLights instance;

	return &instance;
}

void DirectionalLights::Initialize() {
	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},1.0f };
}

void DirectionalLights::Update() {
	directionalLight_.direction = Normalize(directionalLight_.direction);

	ImGui::Begin("DirectionalLight");
	ImGui::DragFloat3("LightColor", directionalLight_.color.num, 1.0f);
	ImGui::DragFloat3("lightDirection", directionalLight_.direction.num, 0.1f);
	ImGui::End();
}