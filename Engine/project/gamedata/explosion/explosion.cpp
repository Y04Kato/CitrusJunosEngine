#include "explosion.h"

void Explosion::Initialize() {
    input_ = Input::GetInstance();
    model_ = Model::CreateModel("project/gamedata/resources/block", "block.obj");
    modelMaterial_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    worldTransformBase_.Initialize();
    audio_ = Audio::GetInstance();
    soundData1_ = audio_->SoundLoad("project/gamedata/resources/break.mp3");
}

void Explosion::Update() {
    for (size_t i = 0; i < models_.size(); i++) {
        //XとZ座標の更新
        worldTransforms_[i].translation_.num[0] += velocities_[i].num[0];
        worldTransforms_[i].translation_.num[2] += velocities_[i].num[2];

        //地面の範囲内かどうかを判定
        bool isWithinFloor =
            worldTransforms_[i].translation_.num[0] >= worldTransformFloor_.translation_.num[0] - worldTransformFloor_.scale_.num[0] &&
            worldTransforms_[i].translation_.num[0] <= worldTransformFloor_.translation_.num[0] + worldTransformFloor_.scale_.num[0] &&
            worldTransforms_[i].translation_.num[2] >= worldTransformFloor_.translation_.num[2] - worldTransformFloor_.scale_.num[2] &&
            worldTransforms_[i].translation_.num[2] <= worldTransformFloor_.translation_.num[2] + worldTransformFloor_.scale_.num[2];

        //地面上のオブジェクトは地面の上で止める
        if (isWithinFloor && worldTransforms_[i].translation_.num[1] > worldTransformFloor_.translation_.num[1] + worldTransformFloor_.scale_.num[1]) {
            worldTransforms_[i].translation_.num[1] += velocities_[i].num[1];
        }
        else if (!isWithinFloor) {
            //範囲外のオブジェクトはY座標が1.0fで止まらず落下を続ける
            worldTransforms_[i].translation_.num[1] += velocities_[i].num[1];
        }
        else {
            worldTransforms_[i].translation_.num[1] = 1.0f;
        }

        worldTransforms_[i].UpdateMatrix();

        //速度を減衰させる
        velocities_[i].num[0] *= 0.98f;//速度の減衰（摩擦を想定）
        velocities_[i].num[1] += acceleration_;//Y方向の重力加速度
        velocities_[i].num[2] *= 0.98f;//速度の減衰（摩擦を想定）

        //Y座標が-10.0f未満の場合、破片を削除
        if (worldTransforms_[i].translation_.num[1] < -10.0f) {
            models_.erase(models_.begin() + i);
            worldTransforms_.erase(worldTransforms_.begin() + i);
            velocities_.erase(velocities_.begin() + i);
            continue;//削除したのでインデックスを増やさない
        }
    }

}

void Explosion::Draw(const ViewProjection& viewProjection) {
    for (size_t i = 0; i < models_.size(); i++) {
        models_[i]->Draw(worldTransforms_[i], viewProjection, modelMaterial_);
    }
}

void Explosion::ExplosionFlagTrue() {
    audio_->SoundPlayWave(soundData1_, 0.1f, false);

    //新しい破片を生成
    for (int i = 0; i < 10; i++) {
        models_.push_back(model_);

        WorldTransform newWorldTransform;
        newWorldTransform.Initialize();
        newWorldTransform.scale_ = { 0.2f, 0.2f, 0.2f };
        newWorldTransform.translation_ = worldTransformBase_.translation_;
        worldTransforms_.push_back(newWorldTransform);

        //初期速度を設定
        Vector3 initialVelocity;
        initialVelocity.num[0] = (rand() % 2 == 0 ? 1 : -1) * (rand() / (float)RAND_MAX) * 0.1f;//ランダムなX方向速度
        initialVelocity.num[1] = 1.2f;//初期の上方向への速度
        initialVelocity.num[2] = (rand() % 2 == 0 ? 1 : -1) * (rand() / (float)RAND_MAX) * 0.1f;//ランダムなZ方向速度
        velocities_.push_back(initialVelocity);
    }
}