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

	void ExplosionFlagTrue(Vector4 material);

private:
    Input* input_;
    Model* model_;
    std::vector<Model*> models_;//破片を動的に追加
    std::vector<WorldTransform> worldTransforms_;//各破片のWorldTransform
    std::vector <Vector3> velocities_;
    std::vector <Vector3> angularVelocities_;
    std::vector <Vector4> modelMaterials_;
    std::vector <std::chrono::steady_clock::time_point> creationTimes_;

    WorldTransform worldTransformBase_;//破壊発生地点のWorldTransform
    WorldTransform worldTransformFloor_;//地面のWorldTransform

    //Other
    float acceleration_ = -0.1f;//重力加速度
    const LONGLONG deleteTime_ = 30;//一度出現したブロックが消えるまでの秒数

    Audio* audio_;
    SoundData soundData1_;
};