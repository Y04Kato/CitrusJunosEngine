#include "GamePlayScene.h"
#include "components/utilities/globalVariables/GlobalVariables.h"

void GamePlayScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoadWave("project/gamedata/resources/conjurer.wav");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, false);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();

	//CollisionManager
	collisionManager_ = std::make_unique<CollisionManager>();

	//テクスチャ
	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/circle.png");

	//パーティクル
	testEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	testEmitter_.count = 5;
	testEmitter_.frequency = 0.2f;//0.2秒ごとに発生
	testEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, testEmitter_, accelerationField, particleResourceNum_);

	playerModelBody_.reset(Model::CreateModelFromObj("project/gamedata/resources/float_Body", "float_Body.obj"));
	playerModelHead_.reset(Model::CreateModelFromObj("project/gamedata/resources/float_Head", "float_Head.obj"));
	playerModelL_Arm_.reset(Model::CreateModelFromObj("project/gamedata/resources/float_L_arm", "float_L_arm.obj"));
	playerModelR_Arm_.reset(Model::CreateModelFromObj("project/gamedata/resources/float_R_arm", "float_R_arm.obj"));
	playerModelWeapon_.reset(Model::CreateModelFromObj("project/gamedata/resources/weapon", "weapon.obj"));
	player_ = std::make_unique<Player>();
	std::vector<Model*> playerModels = {
		playerModelBody_.get(), playerModelHead_.get(), playerModelL_Arm_.get(),
		playerModelR_Arm_.get(), playerModelWeapon_.get() };
	player_->Initialize(playerModels);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransformBody());
	player_->SetViewProjection(&followCamera_->GetViewProjection());

	skyDomeModel_.reset(Model::CreateModelFromObj("project/gamedata/resources/skydome", "Skydome.obj"));
	skyDome_ = std::make_unique<Skydome>();
	skyDome_->Initialize(skyDomeModel_.get());

	groundmanager_ = std::make_unique<GroundManager>();
	groundmanager_->Initialize();

	goal_ = std::make_unique<Goal>();
	goal_->Initialize({ 0.0f,2.0f,62.0f }, { 1.0f,1.0f,1.0f });

	enemyModelBody.reset(Model::CreateModelFromObj("project/gamedata/resources/float_Body", "float_Body.obj"));
	enemyModelHead.reset(Model::CreateModelFromObj("project/gamedata/resources/float_Head", "float_Head.obj"));
	enemyModelL_arm.reset(Model::CreateModelFromObj("project/gamedata/resources/float_L_arm", "float_L_arm.obj"));
	enemyModelR_arm.reset(Model::CreateModelFromObj("project/gamedata/resources/float_R_arm", "float_R_arm.obj"));
	enemyModelWeapon_.reset(Model::CreateModelFromObj("project/gamedata/resources/weapon", "weapon.obj"));

	SetEnemy(Vector3{ 0.0f,2.0f,50.0f });
	SetEnemy(Vector3{ 15.0f,2.0f,0.0f });
	SetEnemy(Vector3{ 10.0f,2.0f,-10.0f });
	SetEnemy(Vector3{ -15.0f,2.0f,-10.0f });
	SetEnemy(Vector3{ -10.0f,2.0f,0.0f });

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	player_->SetLockOn(lockOn_.get());
	followCamera_->SetlockOn(lockOn_.get());
}

void GamePlayScene::Update() {
	count_++;
	ApplyGlobalVariables();

	collisionManager_->ClearColliders();
	collisionManager_->CheckAllCollision();

	debugCamera_->Update();

	viewProjection_.UpdateMatrix();
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	lockOn_->Update(enemys_, viewProjection_);

	groundmanager_->Update();
	player_->Update();
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}
	if (player_->isGameover() == true) {
		player_->SetWorldTransform(Vector3{ 0.0f,0.2f,0.0f });
	}
	player_->isHit_ = false;

	goal_->Update();

	for (int i = 0; i < 2; i++) {
		if (IsCollision(groundmanager_->GetOBB(i), player_->GetStructSphere())) {
			player_->isHit_ = true;
			player_->SetObjectPos(groundmanager_->GetGround(i)->GetWorldTransform());
		}
	}
	if (count_ >= 5) {
		if (IsCollision(groundmanager_->GetOBB(2), player_->GetStructSphere())) {
			player_->isHit_ = true;
			player_->IsCollision(groundmanager_->GetMoveGround()->GetWorldTransform());
		}
		else {
			player_->DeleteParent();
		}
	}

	if (player_->GetIsAttack() == true) {
		for (Enemy* enemy : enemys_) {
			if (IsCollision(player_->GetOBB(), enemy->GetStructSphere())) {
				enemy->SetPlayerComboData(player_->GetVelocity());
				if (player_->GetComboIndex() == 2) {
					enemy->SetIsFly(true);
				}
			}
		}
	}

	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(goal_.get());
	for (Enemy* enemy : enemys_) {
		collisionManager_->AddCollider(enemy);
	}
	if (count_ >= 5) {
		collisionManager_->CheckAllCollision();
	}

	particle_->Update(player_->GetWorldTransformBody().translation_);

	ImGui::Begin("debug");
	ImGui::Text("GamePlayScene");
	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void GamePlayScene::Draw() {
#pragma region 3Dオブジェクト描画
	CJEngine_->PreDraw3D();

	skyDome_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	groundmanager_->Draw(viewProjection_);
	goal_->Draw(viewProjection_);
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
#pragma endregion

#pragma region パーティクル描画
	CJEngine_->PreDrawParticle();

	particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();
	lockOn_->Draw();
#pragma endregion
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}

void GamePlayScene::SetEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	std::vector<Model*>enemyModels = {
enemyModelBody.get(),enemyModelHead.get(),
enemyModelL_arm.get(),enemyModelR_arm.get(),enemyModelWeapon_.get() };
	enemy->SetPos(pos);
	enemy->Initialize(enemyModels);
	enemys_.push_back(enemy);
}