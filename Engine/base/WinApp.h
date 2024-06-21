#pragma once
#include <Windows.h>
#include <cstdint>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d11on12.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include<wrl.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class WinApp {
public:
	bool Procesmessage();
	void Finalize();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Microsoft::WRL::ComPtr <ID3D12Debug1> GetdebugController() { return debugController_; }

	inline HWND GetHwnd() { return hwnd_; }
	HINSTANCE GetWc() { return wc_.hInstance; }
	LPCSTR GetName() { return (LPCSTR)wc_.lpszClassName; }

	void CreateWindowView(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	int32_t GetClientWidth() { return clientWidth_; }
	int32_t GetClientHeight() { return clientHeight_; }

	/// <summary>
	/// desable copy constructor
	/// </summary>

	WinApp(const WinApp& obj) = delete;
	/// <summary>
	///  disable operator overload
	/// </summary>	
	WinApp& operator=(const WinApp& obj) = delete;

	/// <summary>
	/// GetInstance
	/// </summary>
	static WinApp* GetInstance();

private:
	UINT windowStyle_;

	//クライアント領域サイズ
	int32_t clientWidth_ = 1280;
	int32_t clientHeight_ = 720;

	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_;

	static inline WNDCLASS wc_{};//ウィンドウクラス

	static HWND hwnd_;

	/// <summary>
	/// constructor
	/// </summary>
	WinApp() = default;

	/// <summary>
	/// destructor
	/// </summary>
	~WinApp() = default;
};