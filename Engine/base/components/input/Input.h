#pragma once
#define DIRECTINPUT_VERSION		0x0800  //DirectInputのバージョン指定
#include <dinput.h>
#include <XInput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")
#include "WinApp.h"
#include <array>
#include "math/MathCalculation.h"

#define MOUSE_BOTTON0	0
#define MOUSE_BOTTON1	1
#define MOUSE_BOTTON2	2
#define MOUSE_BOTTON3	3
#define MOUSE_BOTTON4	4
#define MOUSE_BOTTON5	5
#define MOUSE_BOTTON6	6
#define MOUSE_BOTTON7	7

struct MousePosition {
	Vector2 Pos;
	Vector2 Velocity;
	float Scroll;
};

class Input {
public:
	static Input* GetInstance();

	void Initialize();

	void Update();

	bool TriggerKey(BYTE keyNumber) const;
	bool PressKey(BYTE keyNumber)const;
	bool ReleaseKey(BYTE keyNumber)const;

	bool pushMouse(uint32_t Mousebutton);

	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out);

	bool PushLTrigger(XINPUT_STATE& out);
	bool PushRTrigger(XINPUT_STATE& out);
	bool PushLSHOULDER(XINPUT_STATE& out);
	bool PushRSHOULDER(XINPUT_STATE& out);
	bool PushAButton(XINPUT_STATE& out);
	bool PushBButton(XINPUT_STATE& out);
	bool PushXButton(XINPUT_STATE& out);
	bool PushYButton(XINPUT_STATE& out);

	bool TriggerAButton(XINPUT_STATE& out);
	bool TriggerBButton(XINPUT_STATE& out);
	bool TriggerXButton(XINPUT_STATE& out);
	bool TriggerYButton(XINPUT_STATE& out);

	void ViewCursor();
	void HideCursor();
	void ToggleCursor();
	bool GetToggleCursor() { return toggleCursor_; }

	MousePosition GetMousePosition() {
		return m_Position_;
	}

	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;
private:
	Input() = default;
	~Input() = default;

	Microsoft::WRL::ComPtr <IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr <IDirectInputDevice8> keyboardInput_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;

	//マウス
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 preMouse_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseInput_ = nullptr;
	MousePosition m_Position_ = { {0.0f,0.0f},0.0f };
	Vector2 MousePos();
	Vector2 MouseVelocity();
	float MouseScroll();
	bool toggleCursor_ = true;

	//コントローラー
	XINPUT_STATE prevStateA;
	XINPUT_STATE prevStateB;
	XINPUT_STATE prevStateX;
	XINPUT_STATE prevStateY;
};