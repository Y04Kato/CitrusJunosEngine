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
	bikeModel_.reset(Model::CreateModel("project/gamedata/resources/bike", "bike.gltf"));
	bikeModel_->SetDirectionalLightFlag(true, 3);

	bikeWorld_.Initialize();

	bodyModelData_ = model_->LoadModelFile("project/gamedata/resources/cylinder", "Cylinder.gltf");
	bodyTexture_ = textureManager_->Load(bodyModelData_.material.textureFilePath);

	bodyTransform_.translate = { 0.0f,0.0f,0.0f };
	bodyTransform_.rotate = { 0.0f,0.0f,0.0f };
	bodyTransform_.scale = { 1.0f,1.0f,1.0f };

	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

	for (int i = 0; i < maxHP_ - 1; i++) {
		Vector4 color = { distColor(randomEngine),distColor(randomEngine) ,distColor(randomEngine) ,1.0f };
		SpawnBody(bodyTransform_, color);
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

	ImGui::Begin("Boss");
	ImGui::DragFloat3("translate", worldTransform_.translation_.num);
	ImGui::DragFloat3("rotate", worldTransform_.rotation_.num);
	ImGui::DragFloat3("scale", worldTransform_.scale_.num);
	ImGui::End();
}

void Boss::Draw(const ViewProjection& viewProjection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	bikeModel_->Draw(bikeWorld_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f });

	for (Body& body : bodys_) {
		body.model.Draw(body.world, viewProjection, body.material);
	}
}

void Boss::Attack1() {

}

void Boss::Attack2() {

}

void Boss::SpawnBody(EulerTransform transform, Vector4 color) {
	Body block;
	block.model.Initialize(bodyModelData_, bodyTexture_);
	block.model.SetDirectionalLightFlag(true, 3);
	block.world.Initialize();

	block.world.translation_ = transform.translate;
	block.world.rotation_ = transform.rotate;
	block.world.scale_ = transform.scale;

	block.material = color;

	bodys_.push_back(block);
}