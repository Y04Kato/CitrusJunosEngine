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
	circle_ = textureManager_->Load("project/gamedata/resources/transition/baseCircle.png");

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
		boxTransform_[i] = { {0.4f,0.39f,0.4f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

		circleSprite_[i] = std::make_unique <CreateSprite>();
		circleSprite_[i]->Initialize(Vector2{ 100.0f,100.0f }, circle_);
		circleSprite_[i]->SetTextureInitialSize();
		circleSprite_[i]->SetAnchor(Vector2{ 0.5f,0.5f });
		circleTransform_[i] = { {0.4f,0.39f,0.4f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

		pairMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
	}

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
	for (int i = 0; i < transitionSpriteMaxNum_; i++) {
		ImGui::Text("pair %d", i);
		ImGui::DragFloat3("KomaTranslate" + ("%d", i), komaTransform_[i].translate.num, 0.5f);
		ImGui::DragFloat3("BoxScale" + ("%d", i), boxTransform_[i].scale.num, 0.5f);
		ImGui::DragFloat4("PairMaterial" + ("%d", i), pairMaterial_[i].num, 1.0f / 256.0f);
	}
	ImGui::End();

	for (int i = 0; i < transitionSpriteMaxNum_; i++) {
		boxTransform_->translate = komaTransform_->translate;
		boxTransform_->translate.num[0] += (boxTransform_->scale.num[0] - 0.4f) * 24.0f;

		circleTransform_->translate.num[0] = boxSprite_[i]->GetWorldPositionFromPoint(boxTransform_[i], Vector2{0.0f,0.5f}).num[0];
		circleTransform_->translate.num[1] = boxSprite_[i]->GetWorldPositionFromPoint(boxTransform_[i], Vector2{0.0f,0.5f}).num[1];
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

}

void Transition::SceneEnd() {

}