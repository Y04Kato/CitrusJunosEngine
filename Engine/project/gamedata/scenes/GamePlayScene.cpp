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

	//Audioの初期化
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/CraftsmansForge.mp3");
	soundData2_ = audio_->SoundLoad("project/gamedata/resources/metal.mp3");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, true);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//モデルの初期化と読み込み
	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());

	enemyModel_.reset(Model::CreateModel("project/gamedata/resources/enemy", "enemy.obj"));
	enemyModel_->SetDirectionalLightFlag(true, 2);

	ground_ = std::make_unique<Ground>();
	groundModel_.reset(Model::CreateModel("project/gamedata/resources/floor", "Floor.obj"));
	groundModel_->SetDirectionalLightFlag(true, 3);
	ground_->Initialize(groundModel_.get(), { 0.0f,0.0f,-5.0f }, { 30.0f,1.0f,30.0f });

	for (int i = 0; i < 5; i++) {
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

	//
	ObjModelData_ = playerModel_->LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	editors_ = Editors::GetInstance();
	editors_->Initialize();
	editors_->SetModels(ObjModelData_, ObjTexture_);
	editors_->SetGroupName((char*)"DemoStage");

	//テクスチャの初期化と読み込み
	background_ = textureManager_->Load("project/gamedata/resources/paper.png");
	move1_ = textureManager_->Load("project/gamedata/resources/move1.png");
	move2_ = textureManager_->Load("project/gamedata/resources/move2.png");
	move3_ = textureManager_->Load("project/gamedata/resources/move3.png");

	//UIの初期化
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

	//パーティクルの初期化
	testEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	testEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	testEmitter_.count = 5;
	testEmitter_.frequency = 0.2f;//0.5秒ごとに発生
	testEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area.max = { 1.0f,1.0f,1.0f };

	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/circle.png");

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, testEmitter_, accelerationField_, particleResourceNum_);
	particle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });

	//PostEffectの読み込み
	postEffect_ = PostEffect::GetInstance();
	noiseTexture_ = textureManager_->Load("project/gamedata/resources/noise0.png");
	maskData_.maskThreshold = 1.0f;
	maskData_.maskColor = { 0.2f,0.2f,0.2f };
	maskData_.edgeColor = { 0.2f,0.2f,0.2f };

	//
	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);

	srand((unsigned int)time(NULL));

	//ライト
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	//ステージ開始用フラグ
	gameStart_ = true;
}

void GamePlayScene::Update() {
	//ステージ開始
	if (gameStart_ == true) {
		player_->SetWorldTransform(Vector3{ 0.0f,0.2f,0.0f });
		player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
		player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });
		directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.5f };
		pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
		debugCamera_->MovingCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);
		maskData_.maskThreshold = 1.0f;
		postEffect_->SetMaskTexture(noiseTexture_);
		for (int i = 0; i < 10; i++) {
			SetEnemy(Vector3{ rand() % 60 - 30 + rand() / (float)RAND_MAX ,2.0f,rand() % 59 - 36 + rand() / (float)RAND_MAX });
		}
		gameStart_ = false;
	}

	//敵を全員倒した時
	if (enemyDethCount_ == 0 && isfadeIn_ == false) {
		isGameclear_ = true;
		isfadeIn_ = true;
	}

	//プレイヤーがゲームオーバーになった時
	if (player_->isGameover() && isfadeIn_ == false) {
		isGameover_ = true;
		isfadeIn_ = true;
	}

	//フェード用
	if (isfadeIn_ == false) {
		fadeAlpha_ -= 4;
		if (fadeAlpha_ <= 0) {
			fadeAlpha_ = 0;
		}
	}
	else if (isfadeIn_ == true && isGameclear_ == true) {//ゲームクリア時
		debugCamera_->MovingCamera(player_->GetWorldTransform().translation_, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);
		fadeAlpha_ += 4;
		if (fadeAlpha_ >= 256) {
			gameStart_ = true;
			isfadeIn_ = false;
			isGameclear_ = false;
			directionalLight_.intensity = 1.0f;
			pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
			sceneNo = CLEAR_SCENE;
		}
	}
	else if (isfadeIn_ == true && isGameover_ == true) {//ゲームオーバー時
		fadeAlpha_ += 4;
		maskData_.maskThreshold -= 0.02f;
		if (fadeAlpha_ >= 256) {
			gameStart_ = true;
			for (Enemy* enemy : enemys_) {
				enemy->SetisDead();
			}
			enemys_.remove_if([&](Enemy* enemy) {
				if (enemy->GetisDead()) {
					delete enemy;
					enemyDethCount_--;
					return true;
				}
				return false;
				});
			isfadeIn_ = false;
			isGameover_ = false;
			directionalLight_.intensity = 1.0f;
			pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
			sceneNo = OVER_SCENE;
		}
	}

	ApplyGlobalVariables();
	//PostEffect更新
	postEffect_->SetMaskData(maskData_);

	//プレイヤー更新
	player_->SetViewProjection(&viewProjection_);
	if (isEditorMode_ == false) {
		player_->Update();
	}
	world_[4].translation_ = player_->GetWorldTransform().translation_;
	for (int i = 0; i < 5; i++) {
		world_[i].UpdateMatrix();
	}

	//
	editors_->Update();
#ifdef _DEBUG
	ImGui::Begin("PlayScene");
	ImGui::Checkbox("isEditorMode", &isEditorMode_);
	ImGui::End();
#endif // _DEBUG

	//ライト更新
	directionalLights_->SetTarget(directionalLight_);
	pointLight_.position.num[0] = player_->GetWorldTransform().GetWorldPos().num[0];
	pointLight_.position.num[2] = player_->GetWorldTransform().GetWorldPos().num[2];
	pointLights_->SetTarget(pointLight_);

	//エネミー更新
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	//エネミーが倒されたら削除する
	enemys_.remove_if([&](Enemy* enemy) {
		if (enemy->GetisDead()) {
			delete enemy;
			enemyDethCount_--;
			return true;
		}
		return false;
		});

	//地面更新
	ground_->Update();
	Obb_.center = ground_->GetWorldTransform().GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(ground_->GetWorldTransform().rotation_), Obb_.orientation);
	Obb_.size = ground_->GetWorldTransform().scale_;

	//Particle更新
	particle_->Update();
	particle_->SetTranslate(player_->GetWorldTransform().translation_);

	//カメラの更新
	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	//プレイヤーの判定取得
	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//プレイヤーと地面の当たり判定
	if (IsCollision(Obb_, pSphere)) {
		player_->isHitOnFloor = true;
		player_->SetObjectPos(ground_->GetWorldTransform());
	}
	else {
		player_->isHitOnFloor = false;
	}

	//エネミーと地面の当たり判定
	for (Enemy* enemy : enemys_) {
		if (IsCollision(Obb_, enemy->GetStructSphere())) {
			enemy->isHitOnFloor = true;
			enemy->SetObjectPos(ground_->GetWorldTransform());
		}
		else {
			enemy->isHitOnFloor = false;
		}
	}

	//プレイヤーとエネミーの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		if (IsCollision(pSphere, eSphere)) {
			//押し戻しの処理
			Vector3 direction = eSphere.center - pSphere.center;
			float distance = Length(direction);
			float overlap = pSphere.radius + eSphere.radius - distance;

			if (overlap > 0.0f) {
				Vector3 correction = Normalize(direction) * (overlap / 2) * 2.0f;
				pSphere.center = pSphere.center - correction;
				eSphere.center = eSphere.center + correction;

				player_->SetWorldTransform(pSphere.center);
				enemy->SetWorldTransform(eSphere.center);
			}

			//反発処理
			std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, player_->GetVelocity(), 1.0f, enemy->GetVelocity(), 0.8f, Normalize(player_->GetWorldTransform().GetWorldPos() - enemy->GetWorldTransform().GetWorldPos()));
			player_->SetVelocity(pair.first);
			enemy->SetVelocity(pair.second);
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
			audio_->SoundPlayWave(soundData2_, 0.1f, false);
		}
	}

	//エネミー同士の当たり判定
	for (auto it1 = enemys_.begin(); it1 != enemys_.end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != enemys_.end(); ++it2) {
			Enemy* enemy1 = *it1;
			Enemy* enemy2 = *it2;

			StructSphere eSphere1 = enemy1->GetStructSphere();
			StructSphere eSphere2 = enemy2->GetStructSphere();

			if (IsCollision(eSphere1, eSphere2)) {
				// 押し戻しの処理
				Vector3 direction = eSphere2.center - eSphere1.center;
				float distance = Length(direction);
				float overlap = eSphere1.radius + eSphere2.radius - distance;

				if (overlap > 0.0f) {
					Vector3 correction = Normalize(direction) * (overlap / 2) * 2.0f;
					eSphere1.center = eSphere1.center - correction;
					eSphere2.center = eSphere2.center + correction;

					enemy1->SetWorldTransform(eSphere1.center);
					enemy2->SetWorldTransform(eSphere2.center);
				}

				//反発処理
				std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, enemy1->GetVelocity(), 1.0f, enemy2->GetVelocity(), 0.8f, Normalize(enemy1->GetWorldTransform().GetWorldPos() - enemy2->GetWorldTransform().GetWorldPos()));
				enemy1->SetVelocity(pair.first);
				enemy2->SetVelocity(pair.second);
				audio_->SoundPlayWave(soundData2_, 0.1f, false);
			}
		}
	}

	//プレイヤーとオブジェクトの当たり判定
	for (Obj obj : editors_->GetObj()) {
		OBB objOBB;
		objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
		if (IsCollision(objOBB,pSphere)) {
			//押し戻し処理
			//Sphere から OBB の最近接点を計算
			Vector3 closestPoint = objOBB.center;
			Vector3 d = pSphere.center - objOBB.center;

			for (int i = 0; i < 3; ++i) {
				float dist = Dot(d, objOBB.orientation[i]);
				dist = std::fmax(-objOBB.size.num[i], std::fmin(dist, objOBB.size.num[i]));
				closestPoint += objOBB.orientation[i] * dist;
			}

			//Sphere の中心と最近接点の距離を計算
			Vector3 direction = pSphere.center - closestPoint;
			float distance = Length(direction);
			float overlap = pSphere.radius - distance;

			if (overlap > 0.0f) {
				Vector3 correction = Normalize(direction) * overlap * 1.5f;
				pSphere.center += correction;

				player_->SetWorldTransform(pSphere.center);
			}

			//反発処理
			Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(pSphere, player_->GetVelocity(), objOBB, 0.8f);
			player_->SetVelocity(velocity);
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
			audio_->SoundPlayWave(soundData2_, 0.1f, false);
		}
	}

	//エネミーとオブジェクトの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		for (Obj obj : editors_->GetObj()) {
			OBB objOBB;
			objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
			if (IsCollision(objOBB, eSphere)) {
				//押し戻し処理
				//Sphere から OBB の最近接点を計算
				Vector3 closestPoint = objOBB.center;
				Vector3 d = eSphere.center - objOBB.center;

				for (int i = 0; i < 3; ++i) {
					float dist = Dot(d, objOBB.orientation[i]);
					dist = std::fmax(-objOBB.size.num[i], std::fmin(dist, objOBB.size.num[i]));
					closestPoint += objOBB.orientation[i] * dist;
				}

				//Sphere の中心と最近接点の距離を計算
				Vector3 direction = eSphere.center - closestPoint;
				float distance = Length(direction);
				float overlap = eSphere.radius - distance;

				if (overlap > 0.0f) {
					Vector3 correction = Normalize(direction) * overlap * 1.5f;
					eSphere.center += correction;

					enemy->SetWorldTransform(eSphere.center);
				}

				//反発処理
				Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), objOBB, 0.8f);
				enemy->SetVelocity(velocity);
				audio_->SoundPlayWave(soundData2_, 0.1f, false);
			}
		}
	}


	//プレイヤー移動時の演出の処理
	if (player_->GetIsMoveFlag() == false) {
		if (input_->TriggerKey(DIK_W)) {
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
		}
		if (input_->TriggerKey(DIK_A)) {
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
		}
		if (input_->TriggerKey(DIK_S)) {
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
		}
		if (input_->TriggerKey(DIK_D)) {
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
		}

		XINPUT_STATE joyState;
		Input::GetInstance()->GetJoystickState(0, joyState);

		if (input_->PushAButton(joyState)) {
			playerFlagRotate_ = Angle(player_->GetVelocity(), { 0.0f,0.0f,1.0f });
		}
	}

	world_[4].rotation_.num[1] = LerpShortAngle(world_[4].rotation_.num[1], -playerFlagRotate_, 0.3f);
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

	editors_->Draw(viewProjection_);

#pragma endregion

#pragma region 3DSkinningオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);
	for (int i = 0; i < 5; i++) {
		flagModel_[i]->SkinningDraw(world_[i], viewProjection_, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GamePlayScene::DrawUI() {
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

void GamePlayScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::Vignette);
	if (isGameover_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::MaskTexture);
	}
	if (isGameclear_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::RadialBlur);
	}
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	audio_->SoundUnload(&soundData2_);

	editors_->Finalize();
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}

void GamePlayScene::SetEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_.get());
	enemy->SetWorldTransform(pos);
	enemys_.push_back(enemy);
	enemyDethCount_++;
}