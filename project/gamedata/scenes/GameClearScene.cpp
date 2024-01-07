#include"GameClearScene.h"

void GameClearScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	pageL_ = textureManager_->Load("project/gamedata/resources/pageLeft.png");
	pageR_ = textureManager_->Load("project/gamedata/resources/pageRight.png");
	pageAll_ = textureManager_->Load("project/gamedata/resources/page.png");
	start_ = textureManager_->Load("project/gamedata/resources/pressSpace.png");
	game_ = textureManager_->Load("project/gamedata/resources/gameclear.png");

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoadWave("project/gamedata/resources/page.wav");

	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	spriteTransform2_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
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
	sprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, pageL_);
	sprite_[1]->SetTextureInitialSize();
	sprite_[1]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[2] = std::make_unique <CreateSprite>();
	sprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, pageR_);
	sprite_[2]->SetTextureInitialSize();
	sprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[3] = std::make_unique <CreateSprite>();
	sprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, start_);
	sprite_[3]->SetTextureInitialSize();
	sprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[4] = std::make_unique <CreateSprite>();
	sprite_[4]->Initialize(Vector2{ 100.0f,100.0f }, game_);
	sprite_[4]->SetTextureInitialSize();
	sprite_[4]->SetAnchor(Vector2{ 0.5f,0.5f });

	count = 0;
}

void GameClearScene::Update() {
	if (input_->TriggerKey(DIK_SPACE) && count < 1) {
		if (pageChange_ == false) {
			pageChange_ = true;
			spriteTransform2_.rotate.num[1] = 0.0f;
			audio_->SoundPlayWave(soundData1_, 0.5f, false);
		}
		else {

		}
	}

	if (pageChange_ == true) {
		spriteTransform2_.rotate.num[1] += 0.05f;
		if (spriteTransform2_.rotate.num[1] >= 3.15f) {
			spriteTransform2_.rotate.num[1] = 3.15f;
			count++;
			pageChange_ = false;
		}
	}

	if (count == 0) {
		if (changeAlpha_ == false) {
			spriteAlpha_ -= 8;
			if (spriteAlpha_ <= 0) {
				changeAlpha_ = true;
			}
		}
		if (changeAlpha_ == true) {
			spriteAlpha_ += 8;
			if (spriteAlpha_ >= 256) {
				changeAlpha_ = false;
			}
		}
	}
	if (count == 1) {
		count = 0;
		sceneNo = TITLE_SCENE;
	}
}

void GameClearScene::Draw() {
#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();
	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	sprite_[1]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	sprite_[2]->Draw(spriteTransform2_, SpriteuvTransform_, spriteMaterial_);
	if (pageChange_ == false) {
		if (count == 0) {
			sprite_[3]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
			sprite_[4]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
		}
	}

#pragma endregion
}

void GameClearScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}