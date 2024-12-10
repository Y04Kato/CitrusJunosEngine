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

	//NULLポインタチェック
	BaseCharacter::Initialize(model);

	//バイクのモデル追加
	bikeModel_.reset(Model::CreateModel("project/gamedata/resources/bike", "bike.obj"));
	bikeModel_->SetDirectionalLightFlag(true, 3);

	bikeWorld_.Initialize();

	bodyModelData_ = model_->LoadModelFile("project/gamedata/resources/cylinder", "Cylinder.obj");
	bodyTexture_ = textureManager_->Load(bodyModelData_.material.textureFilePath);

	bodyTransform_.translate = { 0.0f,0.0f,0.0f };
	bodyTransform_.rotate = { 0.0f,0.0f,0.0f };
	bodyTransform_.scale = { 2.0f,2.0f,2.0f };

	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

	for (int i = 0; i < maxHP_; i++) {
		Vector4 color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine) ,1.0f };
		bodyTransform_.translate.num[1] = bodyTransform_.scale.num[1] * (float)i + bodyTransform_.scale.num[1] / 2.0f;
		SpawnBody(bodyTransform_, color, i);
	}
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
	for (Body& body : bodys_) {
		body.world.UpdateMatrix();
	}

	if (isHit_ == true) {
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
	//3Dモデルを描画
	//model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	//bikeModel_->Draw(bikeWorld_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });

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

}

void Boss::Attack2() {

}

void Boss::SpawnBody(EulerTransform transform, Vector4 color, int num) {
	Body body
		;
	body.model.Initialize(bodyModelData_, bodyTexture_);
	body.model.SetDirectionalLightFlag(true, 3);
	body.world.Initialize();

	body.world.translation_ = transform.translate;
	body.world.rotation_ = transform.rotate;
	body.world.scale_ = transform.scale;

	body.material = color;

	body.num = num;

	body.durability = 1;

	bodys_.push_back(body);
}

void Boss::HitBody(Body b) {
	for (Body& body : bodys_) {
		if (body.num == b.num && isHit_ == false) {
			body.durability--;
			hp_ -= 1;
			isHit_ = true;
			hitTimer_ = coolTime_;
			for (Body& body2 : bodys_) {
				body2.world.translation_.num[1] -= body2.world.scale_.num[1];
			}
		}
	}
}

void Boss::Reset() {
	hp_ = maxHP_;
	isDead_ = false;

	bodyTransform_.translate = { 0.0f,0.0f,0.0f };
	bodyTransform_.rotate = { 0.0f,0.0f,0.0f };
	bodyTransform_.scale = { 2.0f,2.0f,2.0f };

	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

	for (int i = 0; i < maxHP_; i++) {
		Vector4 color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine) ,1.0f };
		bodyTransform_.translate.num[1] = bodyTransform_.scale.num[1] * (float)i + bodyTransform_.scale.num[1] / 2.0f;
		SpawnBody(bodyTransform_, color, i);
	}
}