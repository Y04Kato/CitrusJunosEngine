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


}

void Boss::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();



	worldTransform_.UpdateMatrix();
}

void Boss::Draw(const ViewProjection& viewProjection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
}