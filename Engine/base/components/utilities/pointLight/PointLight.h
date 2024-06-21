#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class PointLights {
public:
	static PointLights* GetInstance();

	void Initialize();

	void Update();

	void SetTarget(const PointLight& pointLight) { pointLight_ = pointLight; }
	const PointLight& GetPointLight() { return pointLight_; }

private:
	PointLights() = default;
	~PointLights() = default;
	PointLights(const PointLights& obj) = default;
	PointLights& operator=(const PointLights& obj) = default;

	PointLight pointLight_;
};

