#include "PointLight.h"

PointLights* PointLights::GetInstance() {
	static PointLights instance;

	return &instance;
}

void PointLights::Initialize() {
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
}

void PointLights::Update() {
	ImGui::Begin("PointLight");
	ImGui::DragFloat3("LightColor", pointLight_.color.num, 1.0f);
	ImGui::DragFloat3("lightPosition", pointLight_.position.num, 0.1f);
	ImGui::DragFloat("lightIntensity", &pointLight_.intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("lightRadius", &pointLight_.radius, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("lightDecay", &pointLight_.decay, 0.1f, 0.0f, 10.0f);
	ImGui::End();
}