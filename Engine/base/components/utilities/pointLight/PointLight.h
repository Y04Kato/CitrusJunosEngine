/**
 * @file PointLight.h
 * @brief PointLight用の各種設定を一元管理する
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class PointLights {
public:
	static PointLights* GetInstance();

	void Initialize();

	void Update();

	//今使われているライトの設定をゲームシーンから得る
	void SetTarget(const PointLight& pointLight) { pointLight_ = pointLight; }
	const PointLight& GetPointLight() { return pointLight_; }

private:
	PointLights() = default;
	~PointLights() = default;
	PointLights(const PointLights& obj) = default;
	PointLights& operator=(const PointLights& obj) = default;

	PointLight pointLight_;
};

