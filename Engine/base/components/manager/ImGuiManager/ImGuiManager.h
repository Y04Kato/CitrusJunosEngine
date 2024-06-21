#pragma once
#include "DirectXCommon.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>

class ImGuiManager {
public:
	static ImGuiManager* GetInstance();
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void Begin();
	void End();
	void Draw();
	void Finalize();
private:
	DirectXCommon* dxCommon_;
};