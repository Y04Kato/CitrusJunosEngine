#include "GamePlayScene.h"
#include "components/utilities/globalVariables/GlobalVariables.h"
#include <stdlib.h>
#include <time.h>

void GamePlayScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/CraftsmansForge.mp3");
	soundData2_ = audio_->SoundLoad("project/gamedata/resources/metal.mp3");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, true);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	viewProjection_.Initialize();

	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());

	ground_ = std::make_unique<Ground>();
	groundModel_.reset(Model::CreateModel("project/gamedata/resources/floor", "Floor.obj"));
	groundModel_->SetDirectionalLightFlag(true, 3);
	ground_->Initialize(groundModel_.get(), { 0.0f,0.0f,-5.0f }, { 30.0f,1.0f,30.0f });

	for (int i = 0; i < 4; i++) {
		flagModel_[i].reset(Model::CreateSkinningModel("project/gamedata/resources/flag", "flag.gltf"));
		flagModel_[i]->SetDirectionalLightFlag(true, 3);
		world_[i].Initialize();
		world_[i].scale_ = { 1.5f,1.5f,1.5f };
		world_[i].rotation_.num[1] = 30.0f;
	}
	world_[0].translation_ = { -30.0f,0.0f,25.0f };
	world_[1].translation_ = { -30.0f,0.0f,-35.0f };
	world_[2].translation_ = { 30.0f,0.0f,25.0f };
	world_[3].translation_ = { 30.0f,0.0f,-35.0f };

	background_ = textureManager_->Load("project/gamedata/resources/paper.png");
	move1_ = textureManager_->Load("project/gamedata/resources/move1.png");
	move2_ = textureManager_->Load("project/gamedata/resources/move2.png");
	move3_ = textureManager_->Load("project/gamedata/resources/move3.png");

	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	SpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	sprite_[0] = std::make_unique <CreateSprite>();
	sprite_[0]->Initialize(Vector2{ 100.0f,100.0f }, background_);
	sprite_[0]->SetTextureInitialSize();
	sprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[1] = std::make_unique <CreateSprite>();
	sprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, move1_);
	sprite_[1]->SetTextureInitialSize();
	sprite_[1]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[2] = std::make_unique <CreateSprite>();
	sprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, move2_);
	sprite_[2]->SetTextureInitialSize();
	sprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[3] = std::make_unique <CreateSprite>();
	sprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, move3_);
	sprite_[3]->SetTextureInitialSize();
	sprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[4] = std::make_unique <CreateSprite>();
	sprite_[4]->Initialize(Vector2{ 100.0f,100.0f }, background_);
	sprite_[4]->SetTextureInitialSize();
	sprite_[4]->SetAnchor(Vector2{ 0.5f,0.5f });

	//パーティクル
	testEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	testEmitter_.count = 5;
	testEmitter_.frequency = 0.2f;//0.5秒ごとに発生
	testEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/circle.png");

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, testEmitter_, accelerationField, particleResourceNum_);
	particle_->SetColor(test);


	enemyModel_.reset(Model::CreateModel("project/gamedata/resources/enemy", "enemy.obj"));
	enemyModel_->SetDirectionalLightFlag(true, 2);

	//CollisionManager
	collisionManager_ = CollisionManager::GetInstance();

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);

	srand((unsigned int)time(NULL));

	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();
}

void GamePlayScene::Update() {
	if (gameStart == true) {
		player_->SetWorldTransform(Vector3{ 0.0f,0.2f,0.0f });
		player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });
		directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.5f };
		pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
		debugCamera_->MovingCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);
		for (int i = 0; i < 10; i++) {
			SetEnemy(Vector3{ rand() % 60 - 30 + rand() / (float)RAND_MAX ,2.0f,rand() % 59 - 36 + rand() / (float)RAND_MAX });
		}
		gameStart = false;
	}

	if (enemyDethCount == 0 && isfadeIn == false) {
		gameclear = true;
		isfadeIn = true;
	}

	if (player_->isGameover() && isfadeIn == false) {
		gameover = true;
		isfadeIn = true;
	}


	if (isfadeIn == false) {
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	else if (isfadeIn == true && gameclear == true) {
		debugCamera_->MovingCamera(player_->GetWorldTransform().translation_, Vector3{0.8f,0.0f,0.0f}, 0.05f);
		fadeAlpha_ += 4;
		if (fadeAlpha_ >= 256) {
			gameStart = true;
			isfadeIn = false;
			gameclear = false;
			directionalLight_.intensity = 1.0f;
			pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
			sceneNo = CLEAR_SCENE;
		}
	}
	else if (isfadeIn == true && gameover == true) {
		fadeAlpha_ += 4;
		if (fadeAlpha_ >= 256) {
			gameStart = true;
			for (Enemy* enemy : enemys_) {
				enemy->SetisDead();
			}
			enemys_.remove_if([&](Enemy* enemy) {
				if (enemy->isDead()) {
					delete enemy;
					enemyDethCount--;
					return true;
				}
				return false;
				});
			isfadeIn = false;
			gameover = false;
			directionalLight_.intensity = 1.0f;
			pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
			sceneNo = OVER_SCENE;
		}
	}

	ApplyGlobalVariables();

	player_->Update();
	for (int i = 0; i < 4; i++) {
		world_[i].UpdateMatrix();
	}
	directionalLights_->SetTarget(directionalLight_);
	pointLight_.position.num[0] = player_->GetWorldTransform().GetWorldPos().num[0];
	pointLight_.position.num[2] = player_->GetWorldTransform().GetWorldPos().num[2];
	pointLights_->SetTarget(pointLight_);

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	ground_->Update();
	Obb_.center = ground_->GetWorldTransform().GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(ground_->GetWorldTransform().rotation_), Obb_.orientation);
	Obb_.size = ground_->GetWorldTransform().scale_;

	if (IsCollision(Obb_, player_->GetStructSphere())) {
		player_->isHit_ = true;
		player_->SetObjectPos(ground_->GetWorldTransform());
	}
	else {
		player_->isHit_ = false;
	}

	for (Enemy* enemy : enemys_) {
		if (IsCollision(Obb_, enemy->GetStructSphere())) {
			enemy->isHit_ = true;
			enemy->SetObjectPos(ground_->GetWorldTransform());
		}
		else {
			enemy->isHit_ = false;
		}
		if (enemy->isDead() == true) {

		}
	}

	player_->SetViewProjection(&viewProjection_);

	particle_->Update();
	particle_->SetTranslate(player_->GetWorldTransform().translation_);
	ImGui::Begin("Particle");
	ImGui::ColorEdit4("Color", test.num, 0);
	ImGui::End();
	particle_->SetColor(test);

	enemys_.remove_if([&](Enemy* enemy) {
		if (enemy->isDead()) {
			delete enemy;
			enemyDethCount--;
			return true;
		}
		return false;
		});

	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_.get());
	for (Enemy* enemy : enemys_) {
		collisionManager_->AddCollider(enemy);
		if (enemy->isCollision_ == true) {
			if (enemy->isHit == false) {
				std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, player_->GetVelocity(), 1.0f, enemy->GetVelocity(), 0.8f, Normalize(player_->GetWorldTransform().GetWorldPos() - enemy->GetWorldTransform().GetWorldPos()));
				player_->SetVelocity(pair.first);
				enemy->SetVelocity(pair.second);
				audio_->SoundPlayWave(soundData2_, 0.1f, false);
			}
			enemy->isCollision_ = false;
		}
	}
	collisionManager_->CheckAllCollision();

	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

}

void GamePlayScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	player_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
#pragma endregion

#pragma region 3DSkinningオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);
	for (int i = 0; i < 4; i++) {
		flagModel_[i]->SkinningDraw(world_[i], viewProjection_, Vector4{1.0f,1.0f,1.0f,1.0f});
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	if (player_->GetMoveMode() == 0) {
		sprite_[1]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}
	if (player_->GetMoveMode() == 1) {
		sprite_[2]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}
	if (player_->GetMoveMode() == 2) {
		sprite_[3]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}

	sprite_[4]->Draw(spriteTransform_, SpriteuvTransform_, Vector4{ 0.0f,0.0f,0.0f,fadeAlpha_ / 256.0f });

#pragma endregion
}

void GamePlayScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GamePlayScene::DrawPostEffect() {
	if (player_->GetIsHit() == true) {
		CJEngine_->renderer_->Draw(PipelineType::Grayscale);
	}
	else {
		CJEngine_->renderer_->Draw(PipelineType::Vignette);
	}
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	audio_->SoundUnload(&soundData2_);
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}

void GamePlayScene::SetEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	enemy->SetWorldTransform(pos);
	enemy->Initialize(enemyModel_.get());
	enemys_.push_back(enemy);
	enemyDethCount++;
}