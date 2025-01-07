/**
 * @file Boss.cpp
 * @brief プレイヤーを倒す為に動くボスを管理
 * @author KATO
 * @date 2024/10/11
 */

#include "Boss.h"
#include <cassert>
#define _USE_MATH_DEFINES

#include <math.h>

void Boss::Initialize(Model* model) {
    textureManager_ = TextureManager::GetInstance();

    BaseCharacter::Initialize(model);

    bikeModel_.reset(Model::CreateModel("project/gamedata/resources/bike", "bike.obj"));
    bikeModel_->SetDirectionalLightFlag(true, 3);

    bikeWorld_.Initialize();

    bodyModelData_ = model_->LoadModelFile("project/gamedata/resources/cylinder", "Cylinder.obj");
    bodyTexture_ = textureManager_->Load(bodyModelData_.material.textureFilePath);

    bodyTransform_.translate = { 0.0f, 0.0f, 0.0f };
    bodyTransform_.rotate = { 0.0f, 0.0f, 0.0f };
    bodyTransform_.scale = { 2.0f, 2.0f, 2.0f };

    std::mt19937 randomEngine(seedGenerator());
    std::uniform_real_distribution<float> distColor(0.5f, 1.0f);

    for (int i = 0; i < maxHP_; i++) {
        Vector4 color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
        bodyTransform_.translate.num[1] = bodyTransform_.scale.num[1] * (float)i + bodyTransform_.scale.num[1] / 2.0f;
        SpawnBody(bodyTransform_, color, i);
    }
}

void Boss::Update() {
    worldTransform_.TransferMatrix();
    bikeWorld_.TransferMatrix();

    if (hp_ <= 0) {
        isDead_ = true;
    }

    attackTimer_--;
    if (attackTimer_ <= 0) {
        if (nowAttackPattern_ == 1) {
            Attack1();
        }
        if (nowAttackPattern_ == 2) {
            Attack2();
        }
    }

    ApplyPhysics();

    worldTransform_.UpdateMatrix();
    bikeWorld_.UpdateMatrix();
    for (Body& body : bodys_) {
        body.world.UpdateMatrix();
    }

    if (isHit_) {
        hitTimer_--;
        if (hitTimer_ <= 0) {
            isHit_ = false;
        }
    }

    ImGui::Begin("Boss");
    ImGui::DragFloat3("translate", worldTransform_.translation_.num);
    ImGui::DragFloat3("rotate", worldTransform_.rotation_.num);
    ImGui::DragFloat3("scale", worldTransform_.scale_.num);
    ImGui::DragInt("hp", &hp_);
    ImGui::End();
}

void Boss::Draw(const ViewProjection& viewProjection) {
    for (Body& body : bodys_) {
        if (body.durability >= 1) {
            body.model.Draw(body.world, viewProjection, body.material);
        }
    }
}

void Boss::Finalize() {
    bodys_.clear();
}

void Boss::Attack1() {
    // Attack1の実装
}

void Boss::Attack2() {
    // Attack2の実装
}

void Boss::SpawnBody(EulerTransform transform, Vector4 color, int num) {
    Body body;
    body.model.Initialize(bodyModelData_, bodyTexture_);
    body.model.SetDirectionalLightFlag(true, 3);
    body.world.Initialize();

    body.world.translation_ = transform.translate;
    body.world.rotation_ = transform.rotate;
    body.world.scale_ = transform.scale;

    body.material = color;
    body.num = num;
    body.durability = 1;
    body.velocity = { 0.0f, 0.0f, 0.0f };

    body.isHitOnFloor = true;

    bodys_.push_back(body);
}

void Boss::SetObjectPos(const WorldTransform& worldtransform) {
    objectPos_ = worldtransform;
}

void Boss::SetIsHitOnFloor(const bool isHitOnFloor, Body& b) {
    for (Body& body : bodys_) {
        if (body.num == b.num && !isHit_) {
            body.isHitOnFloor = isHitOnFloor;
        }
    }
}

void Boss::HitBody(Body& b) {
    for (Body& body : bodys_) {
        if (body.num == b.num && !isHit_) {
            body = b;
            isHit_ = true;
            hitTimer_ = coolTime_;
        }
    }
}

void Boss::Reset() {
    hp_ = maxHP_;
    isDead_ = false;

    bodyTransform_.translate = { 0.0f, 0.0f, 0.0f };
    bodyTransform_.rotate = { 0.0f, 0.0f, 0.0f };
    bodyTransform_.scale = { 2.0f, 2.0f, 2.0f };

    std::mt19937 randomEngine(seedGenerator());
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

    bodys_.clear();
    for (int i = 0; i < maxHP_; i++) {
        Vector4 color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
        bodyTransform_.translate.num[1] = bodyTransform_.scale.num[1] * (float)i + bodyTransform_.scale.num[1] / 2.0f;
        SpawnBody(bodyTransform_, color, i);
    }
}

void Boss::ApplyPhysics() {
    for (Body& body : bodys_) {
        if (body.durability <= 0) continue;//耐久値が0以下のBodyは処理しない
        if (body.world.translation_.num[1] < -10.0f) {//Yが-10以上で
            body.durability = 0; // 耐久値を0に設定
            hp_ -= 1; // ボスの体力を減少させる
        }
        if (!body.isHitOnFloor || body.world.GetWorldPos().num[1] < 0.0f) {//地面と当たっていなければ
            body.world.translation_.num[1] += body.velocity.num[1] / 2;
            Vector3 accelerationVector = { 0.0f,-gravityAcceleration_,0.0f };
            body.velocity.num[1] += accelerationVector.num[1];
        }
        else {
            body.world.translation_.num[1] = objectPos_.translation_.num[1] + objectPos_.scale_.num[1];
            body.velocity.num[1] = 0.0f;
        }

        body.world.translation_ += body.velocity;
    }
}




