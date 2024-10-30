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
	//NULLポインタチェック
	BaseCharacter::Initialize(model);

	//バイクのモデル追加
	bikeModel_.reset(Model::CreateModel("project/gamedata/resources/floor", "Floor.obj"));
	bikeModel_->SetDirectionalLightFlag(true, 3);

	bikeWorld_.Initialize();
}

void Boss::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	bikeWorld_.TransferMatrix();

	//HPが0になったら死亡フラグをTrueにする
	if (hp_ <= 0) {
		isDead_ = true;
	}

	//クールタイムが0になったら攻撃を行う
	attackTimer_--;
	if (attackTimer_ <= 0) {
		if (nowAttackPattern_ == 1) {
			Attack1();
		}

		if (nowAttackPattern_ == 2) {
			Attack2();
		}
	}

	worldTransform_.UpdateMatrix();
	bikeWorld_.UpdateMatrix();
}

void Boss::Draw(const ViewProjection& viewProjection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	bikeModel_->Draw(bikeWorld_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}

void Boss::Attack1() {

}

void Boss::Attack2() {

}