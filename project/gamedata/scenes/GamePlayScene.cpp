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
	enemy_ = std::make_unique<Enemy>();
	std::vector<Model*>enemyModels = {
	enemyModelBody.get(),enemyModelHead.get(),
	enemyModelL_arm.get(),enemyModelR_arm.get(),enemyModelWeapon_.get() };
	enemy_->Initialize(enemyModels);

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
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

	groundmanager_->Update();
	player_->Update();
	enemy_->Update();
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
		if (IsCollision(player_->GetOBB(), enemy_->GetStructSphere())) {
			enemy_->isDead = true;
		}
	}

	collisionManager_->ClearColliders();
	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(goal_.get());
	collisionManager_->AddCollider(enemy_.get());
	if (count_ >= 5) {
		collisionManager_->CheckAllCollision();
	}

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
	enemy_->Draw(viewProjection_);
#pragma endregion

#pragma region パーティクル描画
	CJEngine_->PreDrawParticle();


#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();


#pragma endregion
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}