#pragma once
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "components/3d/Model.h"
#include "components/audio/Audio.h"

class Explosion {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	void SetWorldTransform(const WorldTransform& worldTransform) { worldTransformBase_ = worldTransform; }

	void ExplosionFlagTrue();

	static const int Max = 10;
private:
	Input* input_;
	Model* model_[Max];
	EulerTransform transform_[Max];
	WorldTransform worldTransformBase_;
	WorldTransform worldTransform_[Max];
	Vector4 modelMaterial_;

	bool startFlag = false;
	bool testFlag = true;
	bool drawFlag = false;
	float time_ = 0.0f;
	float bounceSpeed_ = 0.0f;
	float acceleration_ = -0.1f;

	Audio* audio_;
	SoundData soundData1_;
};