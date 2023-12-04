#include "ImGuiManager.h"

ImGuiManager* ImGuiManager::GetInstance(){
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice().Get(),
		dxCommon_->GetbackBufferCount(),
		dxCommon_->getRtvDesc().Format,
		dxCommon_->GetSrvDescriptiorHeap().Get(),
		dxCommon_->GetSrvDescriptiorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSrvDescriptiorHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::Begin(){
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetSrvDescriptiorHeap().Get() };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImGuiManager::End(){
	ImGui::Render();

}

void ImGuiManager::Draw(){
#ifdef _DEBUG
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList().Get());
#endif

}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}