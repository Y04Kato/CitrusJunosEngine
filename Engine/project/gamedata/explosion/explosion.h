/**
 * @file Explosion.h
 * @brief ゲームシーンでの破壊演出を管理
 * @author KATO
 * @date 2024/10/12
 */

#pragma once
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "components/3d/Model.h"
#include "components/audio/Audio.h"

class Explosion {
public:
	void Initialize(ModelData ObjModelData, uint32_t ObjTexture);
	void Update();
	void Draw(const ViewProjection& viewProjection);
    void Finalize();

    //発生地点と床の座標を設定する
	void SetWorldTransformBase(const WorldTransform& worldTransform) { worldTransformBase_ = worldTransform; }
	void SetWorldTransformFloor(const WorldTransform& worldTransform) { worldTransformFloor_ = worldTransform; }

    //設定を基に爆発させる
	void ExplosionFlagTrue(Vector4 material);

    //破片にアクセスするためのゲッター
    const std::vector<WorldTransform>& GetFragments() const { return worldTransforms_; }
    const WorldTransform& GetFragmentTransform(size_t index) const { return worldTransforms_[index]; }
    const Vector3& GetFragmentVelocity(size_t index) const { return velocities_[index]; }

    //破片の位置と速度を設定するセッター
    void SetFragmentPosition(size_t index, const Vector3& position) { worldTransforms_[index].translation_ = position; }
    void SetFragmentVelocity(size_t index, const Vector3& velocity) { velocities_[index] = velocity; }
    void SetAngularVelocitie(size_t index, const Vector3& velocity) { angularVelocities_[index] = velocity; }

private:
    Input* input_;
    ModelData ObjModelData_;
    uint32_t ObjTexture_;

    std::vector<Model*> models_;//破片を動的に追加
    std::vector<WorldTransform> worldTransforms_;//各破片のWorldTransform
    std::vector <Vector3> velocities_;
    std::vector <Vector3> angularVelocities_;
    std::vector <Vector4> modelMaterials_;
    std::vector <std::chrono::steady_clock::time_point> creationTimes_;

    WorldTransform worldTransformBase_;//破壊発生地点のWorldTransform
    WorldTransform worldTransformFloor_;//地面のWorldTransform

    //加速、減速
    const float decelerationFactor_ = 0.98f;//横方向
    const float gravityAcceleration_ = -0.1f;//縦方向
    const float rotationalDamping_ = 0.95f;//回転用
    const float sizeReduction_ = 0.95f;//サイズ縮小
    const float amountApproaching_ = 0.02f;//サイズ縮小時、地面とのズレを修正する量

    //何処まで落ちたら消えるか
    const float dropSite_ = -10.0f;

    //Other
    const LONGLONG deleteTime_ = 30;//一度出現したブロックが消えるまでの秒数

    Audio* audio_;
    SoundData soundData1_;
};