/**
 * @file Transition.cpp
 * @brief トランジションを行うクラス
 * @author KATO
 * @date 2024/10/12
 */

#include "transition.h"

Transition* Transition::GetInstance() {
	static Transition instance;

	return &instance;
}

void Transition::Initialize() {
	//テクスチャマネージャーの初期化
	textureManager_ = TextureManager::GetInstance();

	//テクスチャ読み込み
	koma_ = textureManager_->Load("project/gamedata/resources/transition/koma.png");
	box_ = textureManager_->Load("project/gamedata/resources/transition/baseBox.png");
	circle_ = textureManager_->Load("project/gamedata/resources/transition/baseHalfCircle.png");

	for (int i = 0; i < transitionSpriteMaxNum_; i++) {
		komaSprite_[i] = std::make_unique <CreateSprite>();
		komaSprite_[i]->Initialize(Vector2{ 100.0f,100.0f }, koma_);
		komaSprite_[i]->SetTextureInitialSize();
		komaSprite_[i]->SetAnchor(Vector2{ 0.5f,0.5f });
		komaTransform_[i] = { {0.4f,0.4f,0.4f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

		boxSprite_[i] = std::make_unique <CreateSprite>();
		boxSprite_[i]->Initialize(Vector2{ 100.0f,100.0f }, box_);
		boxSprite_[i]->SetTextureInitialSize();
		boxSprite_[i]->SetAnchor(Vector2{ 1.0f,0.5f });
		boxTransform_[i] = { {1.6f,0.39f,0.4f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

		circleSprite_[i] = std::make_unique <CreateSprite>();
		circleSprite_[i]->Initialize(Vector2{ 100.0f,100.0f }, circle_);
		circleSprite_[i]->SetTextureInitialSize();
		circleSprite_[i]->SetAnchor(Vector2{ 0.5f,0.5f });
		circleTransform_[i] = { {0.4f,0.39f,0.4f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

		pairMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
	}

	komaTransform_[0].translate = { -356.0f,553.0f,0.0f };
	komaTransform_[1].translate = { -230.0f,360.0f,0.0f };
	komaTransform_[2].translate = { -430.0f,719.0f,0.0f };
	komaTransform_[3].translate = { -410.0f,16.0f,0.0f };
	komaTransform_[4].translate = { -150.0f,202.0f,0.0f };
	komaTransform_[5].translate = { -530.0f,300.0f,0.0f };

	boxTransform_[0].scale = { 1.6f,0.39f,0.4f };
	boxTransform_[1].scale = { 1.6f,0.39f,0.4f };
	boxTransform_[2].scale = { 1.5f,0.39f,0.4f };
	boxTransform_[3].scale = { 1.6f,0.39f,0.4f };
	boxTransform_[4].scale = { 1.8f,0.39f,0.4f };
	boxTransform_[5].scale = { 1.6f,0.39f,0.4f };

	pairMaterial_[0] = { 1.0f,0.75f,0.75f,1.0f };
	pairMaterial_[1] = { 0.5f,1.0f,0.5f,1.0f };
	pairMaterial_[2] = { 0.75f,0.75f,1.0f,1.0f };
	pairMaterial_[3] = { 1.0f,0.75f,0.75f,1.0f };
	pairMaterial_[4] = { 0.5f,0.5f,1.0f,1.0f };
	pairMaterial_[5] = { 0.75f,0.75f,1.0f,1.0f };

	allSpriteTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	spriteUVTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
}

void Transition::Update() {
	ImGui::Begin("Transition");
	ImGui::DragFloat3("AllTranslate", &allSpriteTransform_.translate.num[0], 0.5f);
	ImGui::End();

	for (int i = 0; i < transitionSpriteMaxNum_; i++) {
		komaTransform_[i].rotate.num[2] += 0.1f;
		komaTransform_[i].translate += allSpriteTransform_.translate;
		boxTransform_[i].translate = komaTransform_[i].translate;
		boxTransform_[i].translate.num[0] += (boxTransform_[i].scale.num[0] - 0.4f) * 24.0f;

		circleTransform_[i].translate.num[0] = boxSprite_[i]->GetLocalPosition(Vector2{ 0.0f,0.5f }, boxTransform_[i]).num[0];
		circleTransform_[i].translate.num[1] = boxSprite_[i]->GetLocalPosition(Vector2{ 0.0f,0.5f }, boxTransform_[i]).num[1];
	}

	if (isSceneStart_ == true) {
		allSpriteTransform_.translate.num[0] = transitionSpeed_;
		if (3000.0f <= komaTransform_[4].translate.num[0]) {
			allSpriteTransform_.translate = { 0.0f,0.0f,0.0f };
			isSceneStart_ = false;
			isSceneEnd_ = false;
		}
	}

	if (isSceneEnd_ == true) {
		allSpriteTransform_.translate.num[0] = transitionSpeed_;
		if (1430.0f <= komaTransform_[4].translate.num[0]) {
			allSpriteTransform_.translate = { 0.0f,0.0f,0.0f };
			isSceneEnd_ = false;
			isSceneStart_ = false;

			komaTransform_[0].translate = { 1224.0f,553.0f,0.0f };
			komaTransform_[1].translate = { 1350.0f,360.0f,0.0f };
			komaTransform_[2].translate = { 1150.0f,719.0f,0.0f };
			komaTransform_[3].translate = { 1170.0f,16.0f,0.0f };
			komaTransform_[4].translate = { 1430.0f,202.0f,0.0f };
			komaTransform_[5].translate = { 1050.0f,300.0f,0.0f };
		}
	}
}

void Transition::Draw() {
	for (int i = 0; i < transitionSpriteMaxNum_; i++) {
		circleSprite_[i]->Draw(circleTransform_[i], spriteUVTransform_, pairMaterial_[i]);
		boxSprite_[i]->Draw(boxTransform_[i], spriteUVTransform_, pairMaterial_[i]);
		komaSprite_[i]->Draw(komaTransform_[i], spriteUVTransform_, pairMaterial_[i]);
	}
}

void Transition::SceneStart() {
	isSceneStart_ = true;
	isSceneEnd_ = false;

	komaTransform_[0].translate = { 1224.0f,553.0f,0.0f };
	komaTransform_[1].translate = { 1350.0f,360.0f,0.0f };
	komaTransform_[2].translate = { 1150.0f,719.0f,0.0f };
	komaTransform_[3].translate = { 1170.0f,16.0f,0.0f };
	komaTransform_[4].translate = { 1430.0f,202.0f,0.0f };
	komaTransform_[5].translate = { 1050.0f,300.0f,0.0f }; 
}

void Transition::SceneEnd() {
	isSceneEnd_ = true;
	isSceneStart_ = false;

	komaTransform_[0].translate = { -356.0f,553.0f,0.0f };
	komaTransform_[1].translate = { -230.0f,360.0f,0.0f };
	komaTransform_[2].translate = { -430.0f,719.0f,0.0f };
	komaTransform_[3].translate = { -410.0f,16.0f,0.0f };
	komaTransform_[4].translate = { -150.0f,202.0f,0.0f };
	komaTransform_[5].translate = { -530.0f,300.0f,0.0f };
}