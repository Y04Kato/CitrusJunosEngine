/**
 * @file DirectionalLigh.h
 * @brief DirectionalLight用の各種設定を一元管理する
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class DirectionalLights {
public:
	static DirectionalLights* GetInstance();

	void Initialize();

	void Update();

	//今使われているライトの設定をゲームシーンから得る
	void SetTarget(const DirectionalLight& directionalLight) { directionalLight_ = directionalLight; }
	const DirectionalLight& GetDirectionalLight() { return directionalLight_; }

private:
	DirectionalLights() = default;
	~DirectionalLights() = default;
	DirectionalLights(const DirectionalLights& obj) = default;
	DirectionalLights& operator=(const DirectionalLights& obj) = default;

	DirectionalLight directionalLight_;
};
