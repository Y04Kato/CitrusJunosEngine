/**
 * @file Explosion.cpp
 * @brief ゲームシーンでの破壊演出を管理
 * @author KATO
 * @date 2024/10/12
 */

#include "Explosion.h"

void Explosion::Initialize(ModelData ObjModelData, uint32_t ObjTexture) {
    input_ = Input::GetInstance();
    ObjModelData_ = ObjModelData;
    ObjTexture_ = ObjTexture;
    worldTransformBase_.Initialize();
    audio_ = Audio::GetInstance();
    soundData1_ = audio_->SoundLoad("project/gamedata/resources/break.mp3");
}

void Explosion::Update() {
    auto currentTime = std::chrono::steady_clock::now();//現在の時間を取得

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
        else {//それ以外(現状想定してない)
            worldTransforms_[i].translation_.num[1] = 1.0f;
        }

        //回転の更新
        worldTransforms_[i].rotation_.num[0] += angularVelocities_[i].num[0];
        worldTransforms_[i].rotation_.num[1] += angularVelocities_[i].num[1];
        worldTransforms_[i].rotation_.num[2] += angularVelocities_[i].num[2];

        worldTransforms_[i].UpdateMatrix();

        //速度を減衰させる
        velocities_[i].num[0] *= decelerationFactor_;
        velocities_[i].num[1] += gravityAcceleration_;
        velocities_[i].num[2] *= decelerationFactor_;

        //回転速度の減衰
        angularVelocities_[i].num[0] *= rotationalDamping_;
        angularVelocities_[i].num[1] *= rotationalDamping_;
        angularVelocities_[i].num[2] *= rotationalDamping_;

        //Y座標が-10.0f未満の場合、破片を削除
        if (worldTransforms_[i].translation_.num[1] < dropSite_) {
            models_.erase(models_.begin() + i);
            worldTransforms_.erase(worldTransforms_.begin() + i);
            velocities_.erase(velocities_.begin() + i);
            angularVelocities_.erase(angularVelocities_.begin() + i);
            creationTimes_.erase(creationTimes_.begin() + i);
            continue;//削除したのでインデックスを増やさない
        }

        //指定秒数経過したかどうかのチェック
        LONGLONG elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - creationTimes_[i]).count();

        //経過していたら吸い込まれるアニメーションを開始
        if (elapsed >= deleteTime_ - 5 && elapsed < deleteTime_) {
            //徐々に縮小させる
            worldTransforms_[i].scale_.num[0] *= sizeReduction_;
            worldTransforms_[i].scale_.num[1] *= sizeReduction_;
            worldTransforms_[i].scale_.num[2] *= sizeReduction_;

            //徐々に地面に近づける
            worldTransforms_[i].translation_.num[1] -= amountApproaching_;
        }

        //経過時間が指定秒数以上の場合は削除
        if (elapsed >= deleteTime_) {
            models_.erase(models_.begin() + i);
            worldTransforms_.erase(worldTransforms_.begin() + i);
            velocities_.erase(velocities_.begin() + i);
            modelMaterials_.erase(modelMaterials_.begin() + i);
            angularVelocities_.erase(angularVelocities_.begin() + i);
            creationTimes_.erase(creationTimes_.begin() + i);
            continue;//削除したのでインデックスを増やさない
        }
    }
}


void Explosion::Draw(const ViewProjection& viewProjection) {
    for (size_t i = 0; i < models_.size(); i++) {
        models_[i]->Draw(worldTransforms_[i], viewProjection, modelMaterials_[i]);
    }
}

void Explosion::Finalize() {
    models_.clear();
    worldTransforms_.clear();
    velocities_.clear();
    modelMaterials_.clear();
    angularVelocities_.clear();
    creationTimes_.clear();
}

void Explosion::ExplosionFlagTrue(Vector4 material) {
    audio_->SoundPlayWave(soundData1_, 0.1f, false);

    //新しい破片を生成
    for (int i = 0; i < 10; i++) {
        Model* newModel = Model::CreateModel(ObjModelData_, ObjTexture_);
        newModel->SetDirectionalLightFlag(true, 3);
        models_.push_back(newModel);

        WorldTransform newWorldTransform;
        newWorldTransform.Initialize();
        newWorldTransform.scale_ = { 0.2f, 0.2f, 0.2f };
        newWorldTransform.translation_ = worldTransformBase_.translation_;
        worldTransforms_.push_back(newWorldTransform);

        //初期速度を設定
        Vector3 initialVelocity;
        initialVelocity.num[0] = (rand() % 2 == 0 ? 1 : -1) * (rand() / (float)RAND_MAX) * 0.1f;
        initialVelocity.num[1] = 1.2f;
        initialVelocity.num[2] = (rand() % 2 == 0 ? 1 : -1) * (rand() / (float)RAND_MAX) * 0.1f;
        velocities_.push_back(initialVelocity);

        //ランダムな回転速度を設定
        Vector3 initialAngularVelocity;
        initialAngularVelocity = ComputeRotationFromVelocity(initialVelocity, 0.5f);
        angularVelocities_.push_back(initialAngularVelocity);

        //現在の時間を記録
        creationTimes_.push_back(std::chrono::steady_clock::now());

        //マテリアルカラーを設定
        modelMaterials_.push_back(material);
    }
}