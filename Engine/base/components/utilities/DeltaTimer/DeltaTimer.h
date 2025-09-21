#pragma once
#include <chrono>

class DeltaTimer{
public:
	DeltaTimer();
	~DeltaTimer();

	//初期化
	void Initialize();
	//更新
	void Update();
	//デルタタイム取得
	float* GetDeltaTime() { return &deltaTime_; }
	//デバッグ
	void Debug();

private:

	//現在
	std::chrono::time_point<std::chrono::steady_clock> now_;
	//一フレーム前の時間
	std::chrono::time_point<std::chrono::steady_clock> lastTime_;
	//差分
	std::chrono::duration<float> duration_;
	//デルタタイム
	float deltaTime_;

};

