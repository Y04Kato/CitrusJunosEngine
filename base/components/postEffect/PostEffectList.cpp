#include "PostEffectList.h"

PostEffectLists* PostEffectLists::GetInstance() {
	static PostEffectLists instance;

	return &instance;
}

void PostEffectLists::Initialize() {
	postEffectList_.isGrayScale = false;
	postEffectList_.isVignetting = false;
	postEffectList_.isSmoothing = false;
	postEffectList_.isGaussian = false;
	postEffectList_.isOutline = false;
}

void PostEffectLists::Update() {
	ImGui::Begin("PostEffectList");
	if (ImGui::Button("DrawGrayScale")) {
		if (postEffectList_.isGrayScale == false) {
			postEffectList_.isGrayScale = true;
		}
		else {
			postEffectList_.isGrayScale = false;
		}
	}
	if (ImGui::Button("DrawVignette")) {
		if (postEffectList_.isVignetting == false) {
			postEffectList_.isVignetting = true;
		}
		else {
			postEffectList_.isVignetting = false;
		}
	}
	if (ImGui::Button("DrawSmoothing")) {
		if (postEffectList_.isSmoothing == false) {
			postEffectList_.isSmoothing = true;
		}
		else {
			postEffectList_.isSmoothing = false;
		}
	}
	if (ImGui::Button("DrawGaussian")) {
		if (postEffectList_.isGaussian == false) {
			postEffectList_.isGaussian = true;
		}
		else {
			postEffectList_.isGaussian = false;
		}
	}
	if (ImGui::Button("DrawOutline")) {
		if (postEffectList_.isOutline == false) {
			postEffectList_.isOutline = true;
		}
		else {
			postEffectList_.isOutline = false;
		}
	}
	ImGui::End();
}