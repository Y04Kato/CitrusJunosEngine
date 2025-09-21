#include "DeltaTimer.h"
#include "components/manager/ImGuiManager/ImGuiManager.h"

DeltaTimer::DeltaTimer() {}

DeltaTimer::~DeltaTimer() {}

void DeltaTimer::Initialize() {
	now_ = std::chrono::steady_clock::now();
	lastTime_ = std::chrono::steady_clock::now();

	duration_ = now_ - lastTime_;
	deltaTime_ = duration_.count();

}

void DeltaTimer::Update() {
	now_ = std::chrono::steady_clock::now();

	duration_ = now_ - lastTime_;
	deltaTime_ = duration_.count();

	lastTime_ = now_;

}

void DeltaTimer::Debug() {
	ImGui::Begin("deltaTimer");
	ImGui::Text("deltaTime: %f", deltaTime_);
	ImGui::End();

}
