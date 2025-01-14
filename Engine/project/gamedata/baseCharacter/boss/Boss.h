/**
 * @file Boss.h
 * @brief プレイヤーを倒す為に動くボスを管理
 * @author KATO
 * @date 2024/10/11
 */

#pragma once
#include "baseCharacter/BaseCharacter.h"
#include "CreateParticle.h"

struct Body {
	Model model;
	WorldTransform world;
	Vector4 material;
	int num;
	int durability;
	Vector3 velocity;
    bool isHitOnFloor;
};

class Boss : public BaseCharacter {
public:
    void Initialize(Model* model) override;

    void Update() override;

    void Draw(const ViewProjection& viewProjection) override;

    void Finalize();

    // バイクの玩具で引く攻撃
    void Attack1();

    // 独感を撃ち出す攻撃
    void Attack2();

    // 生死フラグ
    bool GetisDead() { return isDead_; }
    void SetisDead(const bool flag) { isDead_ = flag; }

    WorldTransform GetWorldTransform() { return worldTransform_; }
    void SetWorldTransform(const Vector3 translation) { worldTransform_.translation_ = translation; }

    void SpawnBody(EulerTransform transform, Vector4 color, int num);

    //座標の設定
    void SetObjectPos(const WorldTransform& worldtransform);

    //接地しているか否かの設定
    void SetIsHitOnFloor(const bool isHitOnFloor, Body& b);

    // Bodyのリストを得る
    std::list<Body>& GetBody() { return bodys_; }

    // 現在のグループ内のオブジェクトが接触した際の処理
    void HitBody(Body& b);

    // ゲーム再スタート処理
    void Reset();

private:
    void ApplyPhysics(); // 重力と物理情報を適用

    TextureManager* textureManager_;

    const int maxHP_ = 1; // 最大体力
    const int faseChangePoint_ = maxHP_ / 2; // 体力がこの値以下になったらフェイズ2へ
    int hp_ = maxHP_; // 現在体力

    std::unique_ptr<Model> bikeModel_;
    WorldTransform bikeWorld_;

    std::list<Body> bodys_;
    ModelData bodyModelData_;
    uint32_t bodyTexture_;
    EulerTransform bodyTransform_;

    // 生死フラグ
    bool isDead_ = false;

    //接地しているか否か
    WorldTransform objectPos_;

    // 被弾関係
    bool isHit_ = false;
    const int coolTime_ = 30; // 被弾のクールタイム
    int hitTimer_ = coolTime_; // 現在のクールタイム

    // 攻撃に関するモノ
    const int resetTime_ = 180; // 攻撃のクールタイム
    int attackTimer_ = resetTime_; // 現在のクールタイム
    int nowAttackPattern_ = 1; // 次の攻撃内容。各Attack関数の末尾の数字

    //減衰量
    const float movingAttenuation_ = 0.01f;
    const float gravityAcceleration_ = 0.05f;

    // Other
    std::random_device seedGenerator;
};