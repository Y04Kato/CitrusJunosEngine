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

	void SetWorldTransformBase(const WorldTransform& worldTransform) { worldTransformBase_ = worldTransform; }
	void SetWorldTransformFloor(const WorldTransform& worldTransform) { worldTransformFloor_ = worldTransform; }

	void ExplosionFlagTrue();

private:
    Input* input_;
    Model* model_;
    std::vector<Model*> models_;//破片を動的に追加
    std::vector<WorldTransform> worldTransforms_;//各破片のWorldTransform
    std::vector <Vector3> velocities_;

    WorldTransform worldTransformBase_;//破壊発生地点のWorldTransform
    WorldTransform worldTransformFloor_;//地面のWorldTransform
    Vector4 modelMaterial_;

    float acceleration_ = -0.1f;

    Audio* audio_;
    SoundData soundData1_;
};