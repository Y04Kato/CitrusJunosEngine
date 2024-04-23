#include"GameTitleScene.h"

void GameTitleScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	pageAll_ = textureManager_->Load("project/gamedata/resources/paper.png");
	start_ = textureManager_->Load("project/gamedata/resources/pressSpace.png");
	title_ = textureManager_->Load("project/gamedata/resources/title.png");
	tutorial_ = textureManager_->Load("project/gamedata/resources/tutorial.png");

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/page.mp3");

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
	sprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, title_);
	sprite_[2]->SetTextureInitialSize();
	sprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[3] = std::make_unique <CreateSprite>();
	sprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, tutorial_);
	sprite_[3]->SetTextureInitialSize();
	sprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[4] = std::make_unique <CreateSprite>();
	sprite_[4]->Initialize(Vector2{ 100.0f,100.0f }, pageAll_);
	sprite_[4]->SetTextureInitialSize();
	sprite_[4]->SetAnchor(Vector2{ 0.5f,0.5f });

	count = 0;

	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModelFromObj("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 2);
	player_->Initialize(playerModel_.get());
	player_->SetWorldTransform(Vector3{ 0.0f,0.0f,0.0f });

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();
}

void GameTitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE) && count < 2) {
		if (pageChange_ == false) {
			pageChange_ = true;
			audio_->SoundPlayWave(soundData1_, 0.5f, false);
		}
		else {

		}
	}

	player_->UpdateView();
	player_->SetViewProjection(&viewProjection_);

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
		player_->SetWorldTransform(Vector3{ 0.0f,0.0f,0.0f });
		debugCamera_->SetCamera(Vector3{ 0.0f,11.0f,-8.0f }, Vector3{ 0.8f,0.0f,0.0f });
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	if (count == 1) {
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	if (count == 2) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);
		fadeAlpha_ += 4;
		if (fadeAlpha_ >= 256) {
			count = 0;
			fadeAlpha_ = 256;
			sceneNo = GAME_SCENE;
		}
	}

	debugCamera_->Update();
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();
}

void GameTitleScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->PreDraw2D();
	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->PreDraw3D();

	player_->Draw(viewProjection_);
#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();
	if (pageChange_ == false) {
		sprite_[1]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
		if (count == 0) {
			sprite_[2]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
		}
		if (count == 1) {
			sprite_[3]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
		}
	}
	sprite_[4]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 0.0f,0.0f,0.0f,fadeAlpha_ / 256.0f });

#pragma endregion
}

void GameTitleScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}