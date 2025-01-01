/**
 * @file CJEngine.h
 * @brief 各種フレーム処理、描画処理、終了処理をサポート
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "MathCalculation.h"
#include "renderer/Renderer.h"

#include "Input.h"
#include "Audio.h"

class CitrusJunosEngine {
public:
	static CitrusJunosEngine* GetInstance();

	void Initialize(const char* title, int32_t width, int32_t height);

	//フレーム開始、描画準備
	void BeginFrame();

	//フレーム終了、内部コマンドを生成して廟がする
	void EndFrame();

	void Finalize();

	void Update();

	DirectXCommon* GetDirectXCommon() { return dxCommon_; }

	std::unique_ptr<Renderer> renderer_ = nullptr;

	CitrusJunosEngine(const CitrusJunosEngine& obj) = delete;
	CitrusJunosEngine& operator=(const CitrusJunosEngine& obj) = delete;

private:
	CitrusJunosEngine() = default;
	~CitrusJunosEngine() = default;

	static	DirectXCommon* dxCommon_;

	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
};