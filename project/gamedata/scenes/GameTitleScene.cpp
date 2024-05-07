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
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/system.mp3");

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
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());
	player_->SetScale(Vector3{ 5.0f,5.0f,5.0f });

	//パーティクル
	testEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	testEmitter_.count = 5;
	testEmitter_.frequency = 0.2f;//0.5秒ごとに発生
	testEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 10.0f,15.0f,10.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/circle.png");

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, testEmitter_, accelerationField, particleResourceNum_);
	particle_->SetColor({ 0.2f,0.2f,0.2f,1.0f });

	stage_[0].reset(Model::CreateModelFromObj("project/gamedata/resources/GarageMain", "GarageMain.obj"));
	stage_[1].reset(Model::CreateModelFromObj("project/gamedata/resources/GarageDoor", "GarageDoor.gltf"));
	stage_[0]->SetDirectionalLightFlag(true, 3);
	stage_[1]->SetDirectionalLightFlag(true, 3);
	stage_[1]->SetAnimationTime(testTimer_);
	for (int i = 0; i < 3; i++) {
		world_[i].Initialize();
	}

	world_[1].translation_.num[1] = 12.0f;
	world_[1].translation_.num[2] = 150.0f;

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();

	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();
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

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.2f };
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,4.6f,0.0f},0.2f ,10.0f,1.0f };
	directionalLights_->SetTarget(directionalLight_);
	pointLights_->SetTarget(pointLight_);

	player_->UpdateView();
	player_->SetViewProjection(&viewProjection_);
	particle_->Update();
	particle_->SetTranslate(player_->GetWorldTransform().translation_);

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
		debugCamera_->SetCamera(Vector3{ 26.7f,10.7f,-28.8f }, Vector3{ 0.0f,-0.3f,0.0f });
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	if (count == 1) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,-29.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	if (count == 2) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,20.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
		fadeAlpha_ += 4;
		player_->SetVelocity({ 0.0f,0.0f,5.0f });
		if (fadeAlpha_ >= 256) {
			count = 0;
			fadeAlpha_ = 256;
			testTimer_ = 1.0f;
			sceneNo = GAME_SCENE;
		}
	}

	if (count >= 1) {
		testTimer_ += 1.0f;
	}
	stage_[1]->SetAnimationTime(testTimer_);

	for (int i = 0; i < 3; i++) {
		world_[i].UpdateMatrix();
	}

	debugCamera_->Update();
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();
}

void GameTitleScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	player_->Draw(viewProjection_);
	stage_[0]->Draw(world_[0], viewProjection_, Vector4{1.0f,1.0f,1.0f,1.0f});
	stage_[1]->Draw(world_[1], viewProjection_, Vector4{1.0f,1.0f,1.0f,1.0f});
#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);
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