/**
 * @file GamePlayScene.cpp
 * @brief ゲームシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "GamePlayScene.h"
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
	//音声再生(BGM)
	audio_->SoundPlayWave(soundData1_, 0.1f, true);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//Playerモデルの初期化と読み込み
	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());

	//フォローカメラ初期化
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransformPlayer());//カメラをPlayerにセット

	//Bossモデルの初期化と読み込み
	boss_ = std::make_unique<Boss>();
	bossModel_.reset(Model::CreateModel("project/gamedata/resources/hand", "hand.gltf"));
	bossModel_->SetDirectionalLightFlag(true, 3);
	boss_->Initialize(bossModel_.get());

	//Enemyモデルの初期化と読み込み
	enemyModel_.reset(Model::CreateModel("project/gamedata/resources/enemy", "enemy.obj"));
	enemyModel_->SetDirectionalLightFlag(true, 3);

	//Skyboxモデルの初期化と読み込み
	skyboxTex_ = textureManager_->Load("project/gamedata/resources/vz_empty_space_cubemap_ue.dds");
	skyBox_ = std::make_unique <CreateSkyBox>();
	skyBox_->Initialize();
	worldTransformSkyBox_.Initialize();
	worldTransformSkyBox_.scale_ = { 1000.0f,1000.0f,1000.0f };
	skyBoxMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	skyBox_->SetDirectionalLightFlag(true, 3);

	//groundモデルの初期化と読み込み
	groundModel_.reset(Model::CreateModel("project/gamedata/resources/floor", "Floor.obj"));
	groundModel_->SetDirectionalLightFlag(true, 4);
	groundModel_->SetEnvironmentTexture(skyboxTex_);
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(groundModel_.get(), { 0.0f,0.0f,-5.0f }, { 30.0f,1.0f,30.0f });

	//旗モデルの初期化と読み込み
	for (int i = 0; i < 4; i++) {
		flagModel_[i].reset(Model::CreateSkinningModel("project/gamedata/resources/flag", "flag.gltf"));
		flagModel_[i]->SetDirectionalLightFlag(true, 3);
		worldModels_[i].Initialize();
		worldModels_[i].scale_ = { 1.5f,1.5f,1.5f };
		worldModels_[i].rotation_.num[1] = 30.0f;
	}
	//マップ端に旗を設置(自動化予定)
	worldModels_[0].translation_ = { -30.0f,0.0f,25.0f };
	worldModels_[1].translation_ = { -30.0f,0.0f,-35.0f };
	worldModels_[2].translation_ = { 30.0f,0.0f,25.0f };
	worldModels_[3].translation_ = { 30.0f,0.0f,-35.0f };

	//Blockモデルの読み込み
	ObjModelData_ = playerModel_->LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	//エディターの初期化
	editors_ = Editors::GetInstance();
	editors_->Initialize();
	editors_->SetModels(ObjModelData_, ObjTexture_);
	editors_->AddGroupName((char*)"DemoStage");

	//テクスチャの初期化と読み込み
	background_ = textureManager_->Load("project/gamedata/resources/paper.png");
	move1_ = textureManager_->Load("project/gamedata/resources/ui/move1.png");
	move2_ = textureManager_->Load("project/gamedata/resources/ui/move2.png");
	move3_ = textureManager_->Load("project/gamedata/resources/ui/move3.png");
	purpose_ = textureManager_->Load("project/gamedata/resources/ui/purpose.png");
	pause_ = textureManager_->Load("project/gamedata/resources/ui/pause.png");

	//Spriteの初期化
	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	spriteTransform4_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,-2.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
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
	sprite_[4]->Initialize(Vector2{ 100.0f,100.0f }, purpose_);
	sprite_[4]->SetTextureInitialSize();
	sprite_[4]->SetAnchor(Vector2{ 0.5f,0.5f });

	sprite_[5] = std::make_unique <CreateSprite>();
	sprite_[5]->Initialize(Vector2{ 100.0f,100.0f }, pause_);
	sprite_[5]->SetTextureInitialSize();
	sprite_[5]->SetAnchor(Vector2{ 0.5f,0.5f });

	//Playerパーティクルの初期化
	playerEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	playerEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	playerEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	playerEmitter_.count = 5;
	playerEmitter_.frequency = 0.2f;//0.5秒ごとに発生
	playerEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	playerAccelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	playerAccelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	playerAccelerationField_.area.max = { 1.0f,1.0f,1.0f };

	playerParticleResource_ = textureManager_->Load("project/gamedata/resources/circle.png");

	playerParticle_ = std::make_unique <CreateParticle>();
	playerParticle_->Initialize(100, playerEmitter_, playerAccelerationField_, playerParticleResource_);
	playerParticle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });

	//接触時パーティションの初期化
	collisionEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	collisionEmitter_.count = 0;
	collisionEmitter_.frequency = 0.0f;//0.0秒ごとに発生
	collisionEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	collisionAccelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	collisionAccelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	collisionAccelerationField_.area.max = { 1.0f,1.0f,1.0f };

	collisionParticleResource_ = textureManager_->Load("project/gamedata/resources/circle.png");

	collisionParticle_ = std::make_unique <CreateParticle>();
	collisionParticle_->Initialize(100, collisionEmitter_, collisionAccelerationField_, collisionParticleResource_);
	collisionParticle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
	collisionParticle_->SetisVelocity(true, 10.0f);
	collisionParticle_->SetLifeTime(3.0f);

	//PostEffectの読み込み
	postEffect_ = PostEffect::GetInstance();
	noiseTexture_ = textureManager_->Load("project/gamedata/resources/noise0.png");
	maskData_.maskThreshold = 1.0f;
	maskData_.maskColor = { 0.2f,0.2f,0.2f };
	maskData_.edgeColor = { 0.2f,0.2f,0.2f };

	//Explosion
	explosion_ = new Explosion();
	explosion_->Initialize(ObjModelData_, ObjTexture_);
	explosion_->SetWorldTransformFloor(ground_->GetWorldTransform());

	srand((unsigned int)time(NULL));

	//シーン遷移
	transition_ = Transition::GetInstance();

	//ライト
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	//ステージ開始用フラグ
	isGameStart_ = true;
}

void GamePlayScene::Update() {
	if (isGamePause_ == true) {//ポーズ
		GamePauseProcessing();
	}
	else {
		if (isGameStart_ == true) {//ステージ初期設定
			GameStartProcessing();
		}
		else if (isGameEntry_ == true) {//ステージ開始演出
			GameEntryProcessing();
		}
		else {//ゲームプレイ
			GameClearProcessing();
			GameOverProcessing();

			GameProcessing();

			//ゲームをポーズ
			if (input_->TriggerKey(DIK_Q)) {
				isGamePause_ = true;
			}
		}

		//PostEffect更新
		postEffect_->SetMaskData(maskData_);

		//プレイヤー更新
		player_->SetViewProjection(&viewProjection_);
		player_->Update();

		//ボス更新
		boss_->Update();

		//旗座標更新
		for (int i = 0; i < 4; i++) {
			worldModels_[i].UpdateMatrix();
		}

		//Edirots更新
		editors_->Update();

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
				enemyAliveCount_--;
				return true;
			}
			return false;
			});

		//Ground更新
		ground_->Update();
		groundObb_.center = ground_->GetWorldTransform().GetWorldPos();
		GetOrientations(MakeRotateXYZMatrix(ground_->GetWorldTransform().rotation_), groundObb_.orientation);
		groundObb_.size = ground_->GetWorldTransform().scale_;

		//SkyBox更新
		worldTransformSkyBox_.UpdateMatrix();

		//Particle更新
		playerParticle_->Update();
		playerParticle_->SetTranslate(player_->GetWorldTransform().translation_);

		collisionParticle_->Update();
		collisionParticle_->SetAccelerationField(collisionAccelerationField_);

		//Explosion更新
		explosion_->Update();

		//当たり判定処理
		CollisionConclusion();
	}

	//Transition更新
	transition_->Update();

	//
	ImGui::Begin("PlayScene");
	ImGui::Checkbox("isEditorMode", &isEditorMode_);
	ImGui::DragFloat3("", player_->GetVelocity().num);
	ImGui::End();
}

void GamePlayScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	sprite_[0]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);

#pragma endregion

#pragma region SkyBox描画
	CJEngine_->renderer_->Draw(PipelineType::SkyBox);

	skyBox_->Draw(worldTransformSkyBox_, viewProjection_, skyBoxMaterial_, skyboxTex_);
#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	player_->Draw(viewProjection_);
	boss_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	explosion_->Draw(viewProjection_);
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}

	editors_->Draw(viewProjection_);

#pragma endregion

#pragma region 3DSkinningオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);
	for (int i = 0; i < 4; i++) {
		flagModel_[i]->SkinningDraw(worldModels_[i], viewProjection_, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	playerParticle_->Draw(viewProjection_);
	collisionParticle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GamePlayScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	if (isGameEntry_ == true && entryCount_ >= 1 || isGameEntry_ == false) {
		if (isGamePause_ == false) {
			if (player_->GetMoveMode() == 0) {
				sprite_[1]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
			}
			if (player_->GetMoveMode() == 1) {
				sprite_[2]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
			}
			if (player_->GetMoveMode() == 2) {
				sprite_[3]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
			}

			sprite_[4]->Draw(spriteTransform4_, SpriteuvTransform_, spriteMaterial_);
		}
	}

	if (isGamePause_ == true) {
		sprite_[5]->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}

	//
	transition_->Draw();

#pragma endregion
}

void GamePlayScene::DrawPostEffect() {
	if (isGamePause_ == true) {//ポーズ
		CJEngine_->renderer_->Draw(PipelineType::Gaussian);
	}
	else {//ゲームプレイ
		CJEngine_->renderer_->Draw(PipelineType::Vignette);
	}

	if (isGameover_ == true) {//ゲームオーバーなら
		CJEngine_->renderer_->Draw(PipelineType::MaskTexture);
	}
	if (isGameclear_ == true) {//ゲームクリアなら
		CJEngine_->renderer_->Draw(PipelineType::RadialBlur);
	}
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	audio_->SoundUnload(&soundData2_);

	enemys_.clear();
	explosion_->Finalize();
	boss_->Finalize();
	editors_->Finalize();
}

void GamePlayScene::GameStartProcessing() {
	//Player初期化
	player_->SetWorldTransform(Vector3{ 0.0f,0.2f,-20.0f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });

	//ライト初期化
	directionalLight_ = { { 1.0f, 0.8f, 0.6f, 1.0f },{0.0f,-1.0f,0.0f},0.5f };
	pointLight_ = { {1.0f,0.8f,0.6f,1.0f},{0.0f,0.0f,0.0f},0.5f ,10.0f,7.0f };

	//
	maskData_.maskThreshold = 1.0f;
	postEffect_->SetMaskTexture(noiseTexture_);

	//カメラ初期化
	followCamera_->SetTarget(&ground_->GetWorldTransformRotate());//カメラをGroundにセット
	followCamera_->SetOffset({ 0.0f,20.0f,-100.0f });
	followCamera_->Update();

	debugCamera_->SetCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_);
	debugCamera_->Update();
	isBirdseyeMode_ = false;
	isEditorMode_ = false;
	isGamePause_ = false;

	//エディター、ステージ読み込み
	editors_->SetModels(ObjModelData_, ObjTexture_);
	editors_->SetGroupName((char*)"DemoStage");

	//エネミーの配置
	for (int i = 0; i < enemyMaxCount_; i++) {
		Vector3 pos = FindValidPosition();
		SetEnemy(pos);
	}

	boss_->Reset();

	//シーン遷移
	transition_->SceneStart();

	//Sprite
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f - 300.0f,0.0f} };
	spriteTransform4_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,-1.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

	//開始時フラグを無効化
	isGameStart_ = false;

	//開始演出フラグを有効化
	isGameEntry_ = true;
	entryCount_ = 0;
}

void GamePlayScene::GameEntryProcessing() {
	//Playerを行動不能に
	player_->SetIsMove(false);

	//全体を見渡して回る
	if (entryCount_ == 0) {
		startCameraChangeTimer_++;
		if (startCameraChangeTimer_ >= 120) {
			startCameraChangeTimer_ = 0;
			entryCount_ = 1;

		}

		followCamera_->Update();
		viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
	//目標を提示しつつ回る
	if (entryCount_ == 1) {
		//Spriteを回しつつ拡大する
		spriteTransform4_.scale.num[0] += 0.02f;
		spriteTransform4_.scale.num[1] += 0.02f;
		spriteTransform4_.scale.num[2] += 0.02f;

		spriteTransform4_.rotate.num[2] += 0.02f;

		//最大値になったら
		if (spriteTransform4_.scale.num[0] >= 1.0f) {
			spriteTransform4_.scale = { 1.0f,1.0f,1.0f };
			spriteTransform4_.rotate.num[2] = 0.0f;

			startCameraChangeTimer_++;
			if (startCameraChangeTimer_ >= 60) {
				startCameraChangeTimer_ = 0;
				entryCount_ = 2;

				followCamera_->SetTarget(&player_->GetWorldTransformPlayer());
				followCamera_->SetOffset({ 0.0f,3.5f,-20.0f });
				followCamera_->Update();

				debugCamera_->MovingCamera({ 0.0f,4.9f,-40.0f }, followCamera_->GetViewProjection().rotation_, 0.01f);
			}
			else {
				debugCamera_->SetCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_);
				debugCamera_->Update();
			}
		}

		followCamera_->Update();
		viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
	//自機に近づく
	if (entryCount_ == 2) {
		startCameraChangeTimer_++;
		if (startCameraChangeTimer_ >= 120) {
			startCameraChangeTimer_ = 0;
			entryCount_ = 0;
			isGameEntry_ = false;

			//Playerを動けるように
			player_->SetIsMove(true);
		}

		spriteTransform_.translate = Lerp(spriteTransform_.translate, Vector3{ 1280.0f / 2.0f,720.0f / 2.0f,0.0f }, 0.1f);
		spriteTransform4_.translate = Lerp(spriteTransform4_.translate, Vector3{ 1063.0f,62.0f,0.0f }, 0.1f);
		spriteTransform4_.scale = Lerp(spriteTransform4_.scale, Vector3{ 0.38f,0.38f,1.0f }, 0.1f);

		//カメラの更新
		followCamera_->Update();

		debugCamera_->Update();
		viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
		viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
		viewProjection_.UpdateMatrix();
	}
}

void GamePlayScene::GameProcessing() {
	//カメラ切り替え処理
	if (input_->TriggerKey(DIK_E)) {
		if (isBirdseyeMode_ == false) {//Player視点 → 俯瞰視点
			debugCamera_->SetCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_);
			debugCamera_->MovingCamera(Vector3{ 0.0f,54.0f,-62.0f }, Vector3{ 0.8f,0.0f,0.0f }, cameraMoveSpeed_);
			isBirdseyeMode_ = true;

			//Playerを動けないように
			player_->SetIsMove(false);
		}
		else {//俯瞰視点 → Player視点
			debugCamera_->MovingCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_, cameraMoveSpeed_);

			cameraChange_ = true;

			//Playerを動けるように
			player_->SetIsMove(true);
		}
	}

	//カメラ切り替え処理開始
	if (cameraChange_ == true) {
		cameraChangeTimer_++;
	}

	//カメラ切り替え処理終了
	if (cameraChangeTimer_ >= cameraChangeMaxTimer_) {
		debugCamera_->SetCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_);
		cameraChangeTimer_ = 0;
		cameraChange_ = false;
		isBirdseyeMode_ = false;
	}

	//カメラの更新
	debugCamera_->Update();
	followCamera_->Update();
	if (isBirdseyeMode_ == false) {//俯瞰モードでなければ
		viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
	else {//俯瞰モードなら
		viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
		viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
		viewProjection_.UpdateMatrix();
	}
}

void GamePlayScene::GamePauseProcessing() {
	//ゲームポーズを解除
	if (input_->TriggerKey(DIK_Q)) {
		isGamePause_ = false;
	}

	//Eを押したとき
	if (input_->TriggerKey(DIK_E) && isGameEnd_ == false) {
		transition_->SceneEnd();
		isGameEnd_ = true;
	}

	if (isGameEnd_ == true) {
		//遷移終わりにゲーム終了処理
		if (transition_->GetIsSceneEnd_() == false) {
			//各種初期化処理
			StageReset();
			sceneNo = TITLE_SCENE;
		}
	}
}

void GamePlayScene::GameClearProcessing() {
	//ボスのHPが0になった時
	if (boss_->GetisDead() == true) {
		if (isGameclear_ == false) {
			transition_->SceneEnd();
		}
		isGameclear_ = true;
	}

	if (isGameclear_ == true) {
		//遷移終わりにゲームクリア処理
		if (transition_->GetIsSceneEnd_() == false) {
			//各種初期化処理
			StageReset();
			sceneNo = CLEAR_SCENE;
		}
	}
}

void GamePlayScene::GameOverProcessing() {
	//プレイヤーがゲームオーバーになった時
	if (player_->isGameover()) {
		if (maskData_.maskThreshold <= 0.1f && maskData_.maskThreshold >= 0.05f) {
			transition_->SceneEnd();
		}
		isGameover_ = true;
	}

	if (isGameover_ == true) {
		maskData_.maskThreshold -= 0.02f;
		//遷移終わりにゲームオーバー処理
		if (transition_->GetIsSceneEnd_() == false && maskData_.maskThreshold <= 0.0f) {
			//各種初期化処理
			StageReset();
			sceneNo = OVER_SCENE;
		}
	}
}

void GamePlayScene::StageReset() {
	//生き残っている敵を全て削除
	enemys_.clear();

	//各種初期化処理
	isGameStart_ = true;
	isGameover_ = false;
	isGameclear_ = false;
	isGameEnd_ = false;
	isGamePause_ = false;
	isEditorMode_ = false;
	directionalLight_.intensity = 1.0f;
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
	boss_->Finalize();
	editors_->Finalize();
	explosion_->Finalize();
}

void GamePlayScene::CollisionConclusion() {
	//プレイヤーの判定取得
	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//プレイヤーと地面の当たり判定
	if (IsCollision(groundObb_, pSphere)) {
		player_->isHitOnFloor = true;
		player_->SetObjectPos(ground_->GetWorldTransform());
	}
	else {
		player_->isHitOnFloor = false;
	}

	//エネミーと地面の当たり判定
	for (Enemy* enemy : enemys_) {
		if (IsCollision(groundObb_, enemy->GetStructSphere())) {
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
				Vector3 correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
				pSphere.center = pSphere.center - correction;
				eSphere.center = eSphere.center + correction;

				player_->SetWorldTransform(pSphere.center);
				enemy->SetWorldTransform(eSphere.center);
			}

			//衝突音を再生
			ContactVolume(player_->GetVelocity());

			//反発処理
			std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, player_->GetVelocity(), 1.0f, enemy->GetVelocity(), repulsionCoefficient_, Normalize(player_->GetWorldTransform().GetWorldPos() - enemy->GetWorldTransform().GetWorldPos()));
			player_->SetVelocity(pair.first);
			enemy->SetVelocity(pair.second);

			collisionParticle_->SetTranslate(MidPoint(player_->GetWorldTransform().translation_, enemy->GetWorldTransform().translation_));
			collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
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
				//押し戻しの処理
				Vector3 direction = eSphere2.center - eSphere1.center;
				float distance = Length(direction);
				float overlap = eSphere1.radius + eSphere2.radius - distance;

				if (overlap > 0.0f) {
					Vector3 correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
					eSphere1.center = eSphere1.center - correction;
					eSphere2.center = eSphere2.center + correction;

					enemy1->SetWorldTransform(eSphere1.center);
					enemy2->SetWorldTransform(eSphere2.center);
				}

				//衝突音を再生
				ContactVolume(enemy1->GetVelocity());

				//反発処理
				std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, enemy1->GetVelocity(), 1.0f, enemy2->GetVelocity(), repulsionCoefficient_, Normalize(enemy1->GetWorldTransform().GetWorldPos() - enemy2->GetWorldTransform().GetWorldPos()));
				enemy1->SetVelocity(pair.first);
				enemy2->SetVelocity(pair.second);

				collisionParticle_->SetTranslate(MidPoint(enemy1->GetWorldTransform().translation_, enemy2->GetWorldTransform().translation_));
				collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
			}
		}
	}

	//プレイヤーとオブジェクトの当たり判定
	for (Obj obj : editors_->GetObj()) {
		if (obj.durability > 0) {
			if (obj.type == "Wall") {
				OBB objOBB;
				objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
				if (IsCollision(objOBB, pSphere)) {
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
						Vector3 correction = Normalize(direction) * overlap * pushbackMultiplierObj_;
						pSphere.center += correction;

						player_->SetWorldTransform(pSphere.center);
					}

					//衝突音を再生
					ContactVolume(player_->GetVelocity());

					//反発処理
					Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(pSphere, player_->GetVelocity(), objOBB, repulsionCoefficient_);
					player_->SetVelocity(velocity);

					editors_->Hitobj(obj);
					if (obj.durability <= 1) {
						explosion_->SetWorldTransformBase(obj.world);
						isExplosion_ = true;
						explosion_->ExplosionFlagTrue(obj.material);
						explosionTimer_ = 10;
					}

					collisionParticle_->SetTranslate(closestPoint);
					collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
				}
			}
		}
	}

	//エネミーとオブジェクトの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		for (Obj obj : editors_->GetObj()) {
			if (obj.durability > 0) {
				if (obj.type == "Wall") {
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
							Vector3 correction = Normalize(direction) * overlap * pushbackMultiplierObj_;
							eSphere.center += correction;

							enemy->SetWorldTransform(eSphere.center);
						}

						//衝突音を再生
						ContactVolume(enemy->GetVelocity());

						//反発処理
						Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), objOBB, repulsionCoefficient_);
						enemy->SetVelocity(velocity);

						editors_->Hitobj(obj);
						if (obj.durability <= 1) {
							explosion_->SetWorldTransformBase(obj.world);
							isExplosion_ = true;
							explosion_->ExplosionFlagTrue(obj.material);
							explosionTimer_ = 10;
						}

						collisionParticle_->SetTranslate(closestPoint);
						collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
					}
				}
			}
		}
	}

	//プレイヤーと破片の当たり判定
	for (size_t i = 0; i < explosion_->GetFragments().size(); ++i) {
		OBB fragmentOBB;
		WorldTransform fragmentTransform = explosion_->GetFragmentTransform(i);
		fragmentOBB = CreateOBBFromEulerTransform(EulerTransform(
			fragmentTransform.scale_, fragmentTransform.rotation_, fragmentTransform.translation_
		));

		if (IsCollision(fragmentOBB, pSphere)) {
			//破片の反発処理
			Vector3 velocity = ComputeVelocitiesAfterCollisionWithOBB(pSphere, player_->GetVelocity(), 1.0f, fragmentOBB, explosion_->GetFragmentVelocity(i), 1.0f, repulsionCoefficient_);
			velocity.num[1] = 1.0f;//上方向への反発を調整
			explosion_->SetFragmentVelocity(i, velocity * 0.5f);
			explosion_->SetAngularVelocitie(i, ComputeRotationFromVelocity(velocity * 0.5, 0.5f));
		}
	}

	//エネミーと破片の当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();

		for (size_t i = 0; i < explosion_->GetFragments().size(); ++i) {
			OBB fragmentOBB;
			WorldTransform fragmentTransform = explosion_->GetFragmentTransform(i);
			fragmentOBB = CreateOBBFromEulerTransform(EulerTransform(
				fragmentTransform.scale_, fragmentTransform.rotation_, fragmentTransform.translation_
			));

			if (IsCollision(fragmentOBB, eSphere)) {
				//破片の反発処理
				Vector3 velocity = ComputeVelocitiesAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), 1.0f, fragmentOBB, explosion_->GetFragmentVelocity(i), 1.0f, repulsionCoefficient_);
				velocity.num[1] = 1.0f;//上方向への反発を調整
				explosion_->SetFragmentVelocity(i, velocity * 0.5f);
				explosion_->SetAngularVelocitie(i, ComputeRotationFromVelocity(velocity * 0.5, 0.5f));
			}
		}
	}

	//プレイヤーと敵Bodyの当たり判定
	for (Body body : boss_->GetBody()) {
		StructCylinder bCylinder;
		//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
		bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

		//ローカル空間での円柱の上端と下端の位置
		Vector3 localTopCenter{ 0, 0.5f, 0 };
		Vector3 localBottomCenter{ 0, -0.5f, 0 };

		//スケールを適用（Y軸方向のスケールを高さに反映）
		localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
		localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

		//平行移動を適用
		bCylinder.topCenter = localTopCenter + body.world.translation_;
		bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

		if (IsCollision(pSphere, bCylinder)) {
			//円柱の高さ方向の単位ベクトルを計算
			Vector3 cylinderAxis = {
				bCylinder.topCenter.num[0] - bCylinder.bottomCenter.num[0],
				bCylinder.topCenter.num[1] - bCylinder.bottomCenter.num[1],
				bCylinder.topCenter.num[2] - bCylinder.bottomCenter.num[2]
			};
			float heightSquared = DistanceSquared(bCylinder.topCenter, bCylinder.bottomCenter);
			float height = std::sqrt(heightSquared);
			Vector3 unitAxis = { cylinderAxis.num[0] / height, cylinderAxis.num[1] / height, cylinderAxis.num[2] / height };

			//球の中心を円柱の高さ方向に投影して最も近い点を取得
			Vector3 sphereToCylinderBase = {
				pSphere.center.num[0] - bCylinder.bottomCenter.num[0],
				pSphere.center.num[1] - bCylinder.bottomCenter.num[1],
				pSphere.center.num[2] - bCylinder.bottomCenter.num[2]
			};
			float projLength = sphereToCylinderBase.num[0] * unitAxis.num[0] +
				sphereToCylinderBase.num[1] * unitAxis.num[1] +
				sphereToCylinderBase.num[2] * unitAxis.num[2];

			//円柱の高さ範囲に限定
			projLength = std::fmax(0.0f, std::fmin(projLength, height));

			//円柱上で球に最も近い点
			Vector3 closestPointOnCylinder = {
				bCylinder.bottomCenter.num[0] + projLength * unitAxis.num[0],
				bCylinder.bottomCenter.num[1] + projLength * unitAxis.num[1],
				bCylinder.bottomCenter.num[2] + projLength * unitAxis.num[2]
			};

			//球の中心と円柱の最も近い点の距離ベクトル
			Vector3 direction = {
				pSphere.center.num[0] - closestPointOnCylinder.num[0],
				pSphere.center.num[1] - closestPointOnCylinder.num[1],
				pSphere.center.num[2] - closestPointOnCylinder.num[2]
			};
			float distance = Length(direction);

			//球が円柱と重なっているかを確認
			float overlap = pSphere.radius + bCylinder.radius - distance;

			if (overlap > 0.0f) {
				//押し戻しの量を計算
				Vector3 correction = Normalize(direction);
				correction.num[0] *= overlap * pushbackMultiplier_;
				correction.num[1] *= overlap * pushbackMultiplier_;
				correction.num[2] *= overlap * pushbackMultiplier_;

				//球を押し戻し
				pSphere.center.num[0] += correction.num[0];
				pSphere.center.num[1] += correction.num[1];
				pSphere.center.num[2] += correction.num[2];

				player_->SetWorldTransform(pSphere.center);
			}

			//衝突音を再生
			ContactVolume(player_->GetVelocity());

			//反発処理
			Vector3 velocity = ComputeSphereVelocityAfterCollisionWithCylinder(pSphere, player_->GetVelocity(), bCylinder, repulsionCoefficient_);
			player_->SetVelocity(velocity);

			collisionParticle_->SetTranslate(closestPointOnCylinder);
			collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
		}
	}

	//エネミーと敵Bodyの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		for (Body body : boss_->GetBody()) {
			StructCylinder bCylinder;
			//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
			bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

			//ローカル空間での円柱の上端と下端の位置
			Vector3 localTopCenter{ 0, 0.5f, 0 };
			Vector3 localBottomCenter{ 0, -0.5f, 0 };

			//スケールを適用（Y軸方向のスケールを高さに反映）
			localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
			localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

			//平行移動を適用
			bCylinder.topCenter = localTopCenter + body.world.translation_;
			bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

			if (IsCollision(eSphere, bCylinder)) {
				//円柱の高さ方向の単位ベクトルを計算
				Vector3 cylinderAxis = {
					bCylinder.topCenter.num[0] - bCylinder.bottomCenter.num[0],
					bCylinder.topCenter.num[1] - bCylinder.bottomCenter.num[1],
					bCylinder.topCenter.num[2] - bCylinder.bottomCenter.num[2]
				};
				float heightSquared = DistanceSquared(bCylinder.topCenter, bCylinder.bottomCenter);
				float height = std::sqrt(heightSquared);
				Vector3 unitAxis = { cylinderAxis.num[0] / height, cylinderAxis.num[1] / height, cylinderAxis.num[2] / height };

				//球の中心を円柱の高さ方向に投影して最も近い点を取得
				Vector3 sphereToCylinderBase = {
					eSphere.center.num[0] - bCylinder.bottomCenter.num[0],
					eSphere.center.num[1] - bCylinder.bottomCenter.num[1],
					eSphere.center.num[2] - bCylinder.bottomCenter.num[2]
				};
				float projLength = sphereToCylinderBase.num[0] * unitAxis.num[0] +
					sphereToCylinderBase.num[1] * unitAxis.num[1] +
					sphereToCylinderBase.num[2] * unitAxis.num[2];

				//円柱の高さ範囲に限定
				projLength = std::fmax(0.0f, std::fmin(projLength, height));

				//円柱上で球に最も近い点
				Vector3 closestPointOnCylinder = {
					bCylinder.bottomCenter.num[0] + projLength * unitAxis.num[0],
					bCylinder.bottomCenter.num[1] + projLength * unitAxis.num[1],
					bCylinder.bottomCenter.num[2] + projLength * unitAxis.num[2]
				};

				//球の中心と円柱の最も近い点の距離ベクトル
				Vector3 direction = {
					eSphere.center.num[0] - closestPointOnCylinder.num[0],
					eSphere.center.num[1] - closestPointOnCylinder.num[1],
					eSphere.center.num[2] - closestPointOnCylinder.num[2]
				};
				float distance = Length(direction);

				//球が円柱と重なっているかを確認
				float overlap = eSphere.radius + bCylinder.radius - distance;

				if (overlap > 0.0f) {
					//押し戻しの量を計算
					Vector3 correction = Normalize(direction);
					correction.num[0] *= overlap * pushbackMultiplier_;
					correction.num[1] *= overlap * pushbackMultiplier_;
					correction.num[2] *= overlap * pushbackMultiplier_;

					//球を押し戻し
					eSphere.center.num[0] += correction.num[0];
					eSphere.center.num[1] += correction.num[1];
					eSphere.center.num[2] += correction.num[2];

					enemy->SetWorldTransform(eSphere.center);
				}

				//衝突音を再生
				ContactVolume(enemy->GetVelocity());

				bool isBreakCylinder = false;
				float combinedSpeed = std::abs(enemy->GetVelocity().num[0]) + std::abs(enemy->GetVelocity().num[2]);
				if (combinedSpeed >= bodyBreakSpeed_) {
					isBreakCylinder = true;
				}


				//反発処理
				Vector3 velocity = ComputeSphereVelocityAfterCollisionWithCylinder(eSphere, enemy->GetVelocity(), bCylinder, repulsionCoefficient_);
				enemy->SetVelocity(velocity);

				if (isBreakCylinder == true) {
					explosion_->SetWorldTransformBase(body.world);
					isExplosion_ = true;
					explosion_->ExplosionFlagTrue(body.material);
					explosionTimer_ = 10;

					boss_->HitBody(body);
				}

				collisionParticle_->SetTranslate(closestPointOnCylinder);
				collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
			}
		}
	}
}

void GamePlayScene::ContactVolume(Vector3 velocity) {
	//接触速度に基づく音量の調整
	float collisionSpeed = Length(velocity);
	float maxSpeed = 0.95f;//最大速度の仮定
	float minVolume = 0.0005f;//最小音量
	float maxVolume = 0.12f;//最大音量

	//速度に基づく音量の計算 (速度が最大時には maxVolume、速度が0の時には minVolume)
	float volume = minVolume + (maxVolume - minVolume) * (collisionSpeed / maxSpeed);
	volume = std::clamp(volume, minVolume, maxVolume);//音量の範囲を制限

	audio_->SoundPlayWave(soundData2_, volume, false);
}

void GamePlayScene::SetEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_.get());
	enemy->SetWorldTransform(pos);
	enemys_.push_back(enemy);
	enemyAliveCount_++;
}

Vector3 GamePlayScene::GenerateRandomPosition() {
	return Vector3{ rand() % 60 - 30 + rand() / (float)RAND_MAX, 2.0f, rand() % 59 - 36 + rand() / (float)RAND_MAX };
}

bool GamePlayScene::IsValidPosition(const Vector3 pos) {
	StructSphere sphere;
	sphere.center = pos;
	sphere.radius = 1.5f;

	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//各種オブジェクトと衝突しているか確認
	if (IsCollision(pSphere, sphere)) {
		return false;
	}
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		if (IsCollision(eSphere, sphere)) {
			return false;
		}
	}
	for (Obj obj : editors_->GetObj()) {
		OBB objOBB;
		objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
		if (IsCollision(objOBB, sphere)) {
			return false;
		}
	}
	for (Body body : boss_->GetBody()) {
		StructCylinder bCylinder;
		//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
		bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

		//ローカル空間での円柱の上端と下端の位置
		Vector3 localTopCenter{ 0, 0.5f, 0 };
		Vector3 localBottomCenter{ 0, -0.5f, 0 };

		//スケールを適用（Y軸方向のスケールを高さに反映）
		localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
		localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

		//平行移動を適用
		bCylinder.topCenter = localTopCenter + body.world.translation_;
		bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

		if (IsCollision(sphere, bCylinder)) {
			return false;
		}
	}

	//衝突していなければTrueを返す
	return true;
}

Vector3 GamePlayScene::FindValidPosition() {
	const int maxAttempts = 100;
	for (int i = 0; i < maxAttempts; ++i) {
		Vector3 pos = GenerateRandomPosition();
		if (IsValidPosition(pos)) {
			return pos;
		}
	}
	return Vector3{ 0.0f,0.0f,0.0f };
}