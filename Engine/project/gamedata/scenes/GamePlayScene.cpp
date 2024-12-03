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
	followCamera_->SetTarget(&player_->GetWorldTransformCameraPlayer());//カメラをPlayerにセット

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
	for (int i = 0; i < flagCount_; i++) {
		flagModel_[i].reset(Model::CreateSkinningModel("project/gamedata/resources/flag", "flag.gltf"));
		flagModel_[i]->SetDirectionalLightFlag(true, 3);
		flagWorldTransforms_[i].Initialize();
		flagWorldTransforms_[i].scale_ = { 1.5f,1.5f,1.5f };
		flagWorldTransforms_[i].rotation_.num[1] = 30.0f;
	}
	//マップ端に旗を設置(自動化予定)
	flagWorldTransforms_[0].translation_ = { -30.0f,0.0f,25.0f };
	flagWorldTransforms_[1].translation_ = { -30.0f,0.0f,-35.0f };
	flagWorldTransforms_[2].translation_ = { 30.0f,0.0f,25.0f };
	flagWorldTransforms_[3].translation_ = { 30.0f,0.0f,-35.0f };

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
	uiSpriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	uiSpriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	uiSpriteTransform4_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,-2.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	uiSpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	uiSprite_[0] = std::make_unique <CreateSprite>();
	uiSprite_[0]->Initialize(Vector2{ 100.0f,100.0f }, background_);
	uiSprite_[0]->SetTextureInitialSize();
	uiSprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[1] = std::make_unique <CreateSprite>();
	uiSprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, move1_);
	uiSprite_[1]->SetTextureInitialSize();
	uiSprite_[1]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[2] = std::make_unique <CreateSprite>();
	uiSprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, move2_);
	uiSprite_[2]->SetTextureInitialSize();
	uiSprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[3] = std::make_unique <CreateSprite>();
	uiSprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, move3_);
	uiSprite_[3]->SetTextureInitialSize();
	uiSprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[4] = std::make_unique <CreateSprite>();
	uiSprite_[4]->Initialize(Vector2{ 100.0f,100.0f }, purpose_);
	uiSprite_[4]->SetTextureInitialSize();
	uiSprite_[4]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[5] = std::make_unique <CreateSprite>();
	uiSprite_[5]->Initialize(Vector2{ 100.0f,100.0f }, pause_);
	uiSprite_[5]->SetTextureInitialSize();
	uiSprite_[5]->SetAnchor(Vector2{ 0.5f,0.5f });

	//Playerパーティクルの初期化
	playerEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	playerEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	playerEmitter_.transform.scale = { 0.2f,0.2f,0.2f };
	playerEmitter_.count = 5;
	playerEmitter_.frequency = 0.1f;//0.1秒ごとに発生
	playerEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	playerAccelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	playerAccelerationField_.area.min = { -0.1f,-0.1f,-0.1f };
	playerAccelerationField_.area.max = { 0.1f,0.1f,0.1f };

	playerParticleResource_ = textureManager_->Load("project/gamedata/resources/circle.png");

	playerParticle_ = std::make_unique <CreateParticle>();
	playerParticle_->Initialize(100, playerEmitter_, playerAccelerationField_, playerParticleResource_);
	playerParticle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });

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
		for (int i = 0; i < flagCount_; i++) {
			flagWorldTransforms_[i].UpdateMatrix();
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

		//Explosion更新
		explosion_->Update();

		//プレイヤーと地面の当たり判定
		if (IsCollision(groundObb_, player_->GetStructSphere())) {
			player_->SetIsHitOnFloor(true);
			player_->SetObjectPos(ground_->GetWorldTransform());
		}
		else {
			player_->SetIsHitOnFloor(false);
		}

		//エネミーと地面の当たり判定
		for (Enemy* enemy : enemys_) {
			if (IsCollision(groundObb_, enemy->GetStructSphere())) {
				enemy->SetIsHitOnFloor(true);
				enemy->SetObjectPos(ground_->GetWorldTransform());
			}
			else {
				enemy->SetIsHitOnFloor(false);
			}
		}
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

	uiSprite_[0]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);

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
	for (int i = 0; i < flagCount_; i++) {
		flagModel_[i]->SkinningDraw(flagWorldTransforms_[i], viewProjection_, Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	playerParticle_->Draw(viewProjection_);

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
				uiSprite_[1]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
			}
			if (player_->GetMoveMode() == 1) {
				uiSprite_[2]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
			}
			if (player_->GetMoveMode() == 2) {
				uiSprite_[3]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
			}

			uiSprite_[4]->Draw(uiSpriteTransform4_, uiSpriteuvTransform_, uiSpriteMaterial_);
		}
	}

	if (isGamePause_ == true) {
		uiSprite_[5]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
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
	player_->SetTranslate(Vector3{ 0.0f,0.2f,-20.0f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });

	//ライト初期化
	directionalLight_ = { { 1.0f, 0.8f, 0.6f, 1.0f },{0.0f,-1.0f,0.0f},0.5f };
	pointLight_ = { {1.0f,0.8f,0.6f,1.0f},{0.0f,0.0f,0.0f},0.2f ,10.0f,7.0f };

	//
	maskData_.maskThreshold = 1.0f;
	postEffect_->SetMaskTexture(noiseTexture_);

	//カメラ初期化
	followCamera_->SetTarget(&ground_->GetWorldTransformForCameraReference());//カメラをGroundにセット
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
	uiSpriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f - 300.0f,0.0f} };
	uiSpriteTransform4_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,-1.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };

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
		if (startCameraChangeTimer_ >= startCameraChangeTime_) {
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
		uiSpriteTransform4_.scale.num[0] += magnificationPower_;
		uiSpriteTransform4_.scale.num[1] += magnificationPower_;
		uiSpriteTransform4_.scale.num[2] += magnificationPower_;

		uiSpriteTransform4_.rotate.num[2] += magnificationPower_;

		//最大値になったら
		if (uiSpriteTransform4_.scale.num[0] >= 1.0f) {
			uiSpriteTransform4_.scale = { 1.0f,1.0f,1.0f };
			uiSpriteTransform4_.rotate.num[2] = 0.0f;

			startCameraChangeTimer_++;
			if (startCameraChangeTimer_ >= startCameraChangeTime_ / 2.0f) {
				startCameraChangeTimer_ = 0;
				entryCount_ = 2;

				followCamera_->SetTarget(&player_->GetWorldTransformCameraPlayer());
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
		if (startCameraChangeTimer_ >= startCameraChangeTime_) {
			startCameraChangeTimer_ = 0;
			entryCount_ = 0;
			isGameEntry_ = false;

			//Playerを動けるように
			player_->SetIsMove(true);
		}

		uiSpriteTransform_.translate = Lerp(uiSpriteTransform_.translate, Vector3{ 1280.0f / 2.0f,720.0f / 2.0f,0.0f }, 0.1f);
		uiSpriteTransform4_.translate = Lerp(uiSpriteTransform4_.translate, Vector3{ 1063.0f,62.0f,0.0f }, 0.1f);
		uiSpriteTransform4_.scale = Lerp(uiSpriteTransform4_.scale, Vector3{ 0.38f,0.38f,1.0f }, 0.1f);

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
		maskData_.maskThreshold -= maskThresholdSpeed_;
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