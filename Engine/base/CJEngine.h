#pragma once
#include "DirectXCommon.h"
#include "MathCalculation.h"
#include "renderer/Renderer.h"

class CitrusJunosEngine {
public:
	static CitrusJunosEngine* GetInstance();

	void Initialize(const char* title, int32_t width, int32_t height);

	void BeginFrame();

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
};