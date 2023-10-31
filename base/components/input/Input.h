#pragma once
#define DIRECTINPUT_VERSION		0x0800  //DirectInputのバージョン指定
#include <dinput.h>
#include <XInput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")
#include "WinApp.h"
#include <array>

class Input{
public:
	static Input* GetInstance();

	void Initialize();

	void Update();

	bool TriggerKey(BYTE keyNumber) const;
	bool PressKey(BYTE keyNumber)const;
	bool ReleaseKey(BYTE keyNumber)const;

	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out);

	bool PushLTrigger(XINPUT_STATE& out);
	bool PushRTrigger(XINPUT_STATE& out);
	bool PushLSHOULDER(XINPUT_STATE& out);
	bool PushRSHOULDER(XINPUT_STATE& out);
	bool PushAButton(XINPUT_STATE& out);
	bool PushBButton(XINPUT_STATE& out);
	bool PushXButton(XINPUT_STATE& out);
	bool PushYButton(XINPUT_STATE& out);

	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;

private:
	Input() = default;
	~Input() = default;

	Microsoft::WRL::ComPtr <IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr <IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;
};

