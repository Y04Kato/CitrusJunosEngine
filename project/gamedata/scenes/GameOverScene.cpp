#include"GameOverScene.h"

void GameOverScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	pageAll_ = textureManager_->Load("project/gamedata/resources/paper.png");
	start_ = textureManager_->Load("project/gamedata/resources/pressSpace.png");
	game_ = textureManager_->Load("project/gamedata/resources/gameover.png");

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoadWave("project/gamedata/resources/page.wav");

	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	SpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	sprite_[0] = std::make_unique <CreateSprite>();
	sprite_[0]->Initialize(Vector2{ 100.0f,100.0f }, pageAll_);
	sprite_[0]->SetTextureInitialSize();
	sprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[1] = std::make_unique <CreateSprite>();
	sprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, start_);
	sprite_[1]->SetTextureInitialSize();
	sprite_[1]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[2] = std::make_unique <CreateSprite>();
	sprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, game_);
	sprite_[2]->SetTextureInitialSize();
	sprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[3] = std::make_unique <CreateSprite>();
	sprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, pageAll_);
	sprite_[3]->SetTextureInitialSize();
	sprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	count = 0;
}

void GameOverScene::Update() {
	if (input_->TriggerKey(DIK_SPACE) && count < 1) {
		if (pageChange_ == false) {
			pageChange_ = true;
			audio_->SoundPlayWave(soundData1_, 0.5f, false);
		}
		else {

		}
	}

	if (pageChange_ == true) {
		count++;
		pageChange_ = false;
	}

	if (changeAlpha_ == false) {
		spriteAlpha_ -= 8;
		if (spriteAlpha_ <= 0) {
			changeAlpha_ = true;
		}
	}
	else if (changeAlpha_ == true) {
		spriteAlpha_ += 8;
		if (spriteAlpha_ >= 256) {
			changeAlpha_ = false;
		}
	}

	if (count == 0) {
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	if (count == 1) {
		fadeAlpha_ += 4;
		if (fadeAlpha_ >= 256) {
			count = 0;
			fadeAlpha_ = 256;
			sceneNo = TITLE_SCENE;
		}
	}
}

void GameOverScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->PreDraw2D();
	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();
	if (pageChange_ == false) {
		sprite_[1]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
		if (count == 0) {
			sprite_[2]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
		}
	}
	sprite_[3]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 0.0f,0.0f,0.0f,fadeAlpha_ / 256.0f });
#pragma endregion
}

void GameOverScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}