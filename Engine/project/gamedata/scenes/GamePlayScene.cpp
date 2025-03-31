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

	//SceneNumber
	sceneNumber_ = SceneNumber::GetInstance();

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

	//背景モデル
	for (int i = 0; i < modelMaxCount_; i++) {
		bgModel_[i] = std::make_unique <Model>();
	}
	bgModel_[0].reset(Model::CreateModel("project/gamedata/resources/bike", "bike.obj"));
	bgModel_[1].reset(Model::CreateModel("project/gamedata/resources/bike", "bike.obj"));
	bgModel_[2].reset(Model::CreateModel("project/gamedata/resources/chest", "chest.obj"));
	bgModel_[3].reset(Model::CreateModel("project/gamedata/resources/chest", "chest.obj"));
	for (int i = 0; i < modelMaxCount_; i++) {
		worldTransformBGModel_[i].Initialize();
		bgModelMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		bgModel_[i]->SetDirectionalLightFlag(true, 3);
	}
	worldTransformBGModel_[0].translation_ = { 61.0f,9.0f,59.0f };
	worldTransformBGModel_[0].rotation_ = { -0.7f,0.0f,-0.25f };

	worldTransformBGModel_[1].translation_ = { -78.0f,12.0f,11.0f };
	worldTransformBGModel_[1].rotation_ = { 0.0f,-1.15f,0.2f };

	worldTransformBGModel_[2].translation_ = { 5.0f,8.0f,-90.0f };
	worldTransformBGModel_[2].rotation_ = { 0.2f,0.35f,0.7f };

	worldTransformBGModel_[3].translation_ = { -59.0f,9.0f,56.0f };
	worldTransformBGModel_[3].rotation_ = { 0.8f,-1.65f,0.14f };

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

	//接触時パーティションの初期化
	collisionEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.scale = { 0.5f,0.5f,0.5f };
	collisionEmitter_.count = 0;
	collisionEmitter_.frequency = 0.0f;//0.0秒ごとに発生
	collisionEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	collisionAccelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	collisionAccelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	collisionAccelerationField_.area.max = { 1.0f,1.0f,1.0f };

	collisionParticleResource_ = textureManager_->Load("project/gamedata/resources/hit.png");

	collisionParticle_ = std::make_unique <CreateParticle>();
	collisionParticle_->Initialize(100, collisionEmitter_, collisionAccelerationField_, collisionParticleResource_);
	collisionParticle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
	collisionParticle_->SetisVelocity(true, 10.0f);
	collisionParticle_->SetLifeTime(3.0f);
	collisionParticle_->SetisBillBoard(false);

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
}

void GamePlayScene::Update() {
	switch (scenePhase_) {
	case ScenePhase::SceneStart://ゲーム開始時の初期設定
		GameStartProcessing();
		break;

	case ScenePhase::EntryGame://ゲーム開始演出
		GameEntryProcessing();
		break;

	case ScenePhase::InGame://インゲーム
		GameProcessing();
		break;

	case ScenePhase::PoseGame://ゲームポーズ
		GamePauseProcessing();
		break;

	case ScenePhase::GameClear://ゲームクリア
		GameClearProcessing();
		break;

	case ScenePhase::GameOver://ゲームオーバー
		GameOverProcessing();
		break;
	}

	//ポーズ中でなければ
	if (scenePhase_ != ScenePhase::PoseGame) {
		GameCommonProcessing();
	}

	//Transition更新
	transition_->SceneChangeUpdate();
	transition_->ChangePauseUpdate();

	//ImGui
	ImGui::Begin("PlayScene");
	ImGui::Checkbox("isEditorMode", &isEditorMode_);
	ImGui::DragFloat3("", player_->GetVelocity().num);
	ImGui::DragFloat3("BGmodel", worldTransformBGModel_[1].translation_.num);
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

	for (int i = 0; i < modelMaxCount_; i++) {
		bgModel_[i]->Draw(worldTransformBGModel_[i], viewProjection_, bgModelMaterial_[i]);
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
	collisionParticle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GamePlayScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	//UI
	if (scenePhase_ == ScenePhase::EntryGame && gameEntryPhase_ != GameEntryPhase::RotaryView || scenePhase_ != ScenePhase::EntryGame) {
		if (scenePhase_ != ScenePhase::PoseGame) {
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

	//ポーズ中UI
	if (scenePhase_ == ScenePhase::PoseGame) {
		uiSprite_[5]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
	}

	//
	transition_->Draw();

#pragma endregion
}

void GamePlayScene::DrawPostEffect() {
	if (scenePhase_ == ScenePhase::PoseGame) {//ポーズなら
		CJEngine_->renderer_->Draw(PipelineType::Gaussian);
	}
	else {//インゲームなら
		CJEngine_->renderer_->Draw(PipelineType::Vignette);
	}

	if (scenePhase_ == ScenePhase::GameOver) {//ゲームオーバーなら
		CJEngine_->renderer_->Draw(PipelineType::MaskTexture);
	}
	if (scenePhase_ == ScenePhase::GameClear) {//ゲームクリアなら
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

	viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	isBirdseyeMode_ = false;
	isEditorMode_ = false;

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

	//開始演出の初期設定
	gameEntryPhase_ = GameEntryPhase::RotaryView;

	//ゲーム開始演出へ移行
	scenePhase_ = ScenePhase::EntryGame;
}

void GamePlayScene::GameEntryProcessing() {
	switch (gameEntryPhase_) {
	case GameEntryPhase::RotaryView://ステージ全体を映して回る
		//Playerを行動不能に
		player_->SetIsMove(false);

		startCameraChangeTimer_++;
		if (startCameraChangeTimer_ >= startCameraChangeTime_) {//指定時間経過で
			startCameraChangeTimer_ = 0;
			gameEntryPhase_ = GameEntryPhase::RulePresentation;//ルールの提示フェーズへ移行

		}

		followCamera_->Update();
		viewProjection_.translation_ = followCamera_->GetViewProjection().translation_;
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();

		break;

	case GameEntryPhase::RulePresentation://ステージ全体を映して回転しつつ、ステージ目標を提示
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
			if (startCameraChangeTimer_ >= startCameraChangeTime_ / 2.0f) {//指定時間経過で
				startCameraChangeTimer_ = 0;
				gameEntryPhase_ = GameEntryPhase::ApproachingPlayer;//プレイヤーに近づくフェーズへ移行

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

		break;

	case GameEntryPhase::ApproachingPlayer://プレイヤーに近づいてインゲームへ
		startCameraChangeTimer_++;
		if (startCameraChangeTimer_ >= startCameraChangeTime_) {
			startCameraChangeTimer_ = 0;
			gameEntryPhase_ = GameEntryPhase::RotaryView;//ステージ全体を見るフェーズに戻しておく
			scenePhase_ = ScenePhase::InGame;//インゲームへ移行

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

		break;
	}
}

void GamePlayScene::GameProcessing() {
	//Qを押した時
	if (input_->TriggerKey(DIK_Q)) {
		transition_->PauseStart();//ゲームをポーズ
		scenePhase_ = ScenePhase::PoseGame;//ポーズへ移行
	}

	//カメラ切り替え処理
	//Eを押した時
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

	//ボスのHPが0になった時
	if (boss_->GetisDead() == true) {
		transition_->SceneEnd();
		scenePhase_ = ScenePhase::GameClear;//クリア演出へ移行
	}

	//プレイヤーがゲームオーバーになった時
	if (player_->isGameover()) {
		scenePhase_ = ScenePhase::GameOver;//ゲームオーバー演出へ移行
	}
}

void GamePlayScene::GamePauseProcessing() {
	//Qを押した時
	if (input_->TriggerKey(DIK_Q)) {
		transition_->PauseEnd();//ポーズを解除
		scenePhase_ = ScenePhase::InGame;//インゲームへ移行
	}

	//Eを押した時
	if (input_->TriggerKey(DIK_E) && isGameEnd_ == false) {
		transition_->SceneEnd();
		transition_->PauseEnd();
		isGameEnd_ = true;
	}

	if (isGameEnd_ == true) {
		//遷移終わりにゲーム終了処理
		if (transition_->GetIsSceneEnd_() == false) {
			//各種初期化処理
			StageReset();
			sceneNumber_->SetSceneNumber(TITLE_SCENE);
		}
	}
}

void GamePlayScene::GameClearProcessing() {
	//遷移終わりにゲームクリア処理
	if (transition_->GetIsSceneEnd_() == false) {
		//各種初期化処理
		StageReset();
		sceneNumber_->SetSceneNumber(CLEAR_SCENE);
	}
}

void GamePlayScene::GameOverProcessing() {
	maskData_.maskThreshold -= maskThresholdSpeed_;

	//マスクの閾値が一定を超えたら
	if (maskData_.maskThreshold <= 0.1f && maskData_.maskThreshold >= 0.05f) {
		transition_->SceneEnd();
	}

	//マスクの閾値が0になって演出が終了したのを確認したらゲームオーバー処理
	if (transition_->GetIsSceneEnd_() == false && maskData_.maskThreshold <= 0.0f) {
		//各種初期化処理
		StageReset();
		sceneNumber_->SetSceneNumber(OVER_SCENE);
	}
}

void GamePlayScene::GameCommonProcessing() {
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

	//背景モデル更新
	for (int i = 0; i < modelMaxCount_; i++) {
		worldTransformBGModel_[i].UpdateMatrix();
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

void GamePlayScene::StageReset() {
	//Player初期化
	player_->SetTranslate(Vector3{ 0.0f,0.2f,-20.0f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });
	player_->Update();

	//生き残っている敵を全て削除
	enemys_.clear();

	//各種初期化処理
	isGameEnd_ = false;
	isEditorMode_ = false;
	directionalLight_.intensity = 1.0f;
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
	followCamera_->SetTarget(&ground_->GetWorldTransformForCameraReference());//カメラをGroundにセット
	followCamera_->SetOffset({ 0.0f,20.0f,-100.0f });
	followCamera_->Update();
	debugCamera_->SetCamera(followCamera_->GetViewProjection().translation_, followCamera_->GetViewProjection().rotation_);
	debugCamera_->Update();
	boss_->Finalize();
	editors_->Finalize();
	explosion_->Finalize();
	scenePhase_ = ScenePhase::SceneStart;
}

void GamePlayScene::HandleFragmentCollision(size_t index, StructSphere sphere, Vector3 velocity, OBB& fragmentOBB) {
	Vector3 newVelocity = ComputeVelocitiesAfterCollisionWithOBB(
		sphere, velocity, 1.0f, fragmentOBB, explosion_->GetFragmentVelocity(index), 1.0f, repulsionCoefficient_
	);
	newVelocity.num[1] = 1.0f;
	explosion_->SetFragmentVelocity(index, newVelocity * 0.5f);
	explosion_->SetAngularVelocitie(index, ComputeRotationFromVelocity(newVelocity * 0.5, 0.5f));
}

StructCylinder GamePlayScene::CreateCylinderFromBody(const Body& body) {
	StructCylinder bCylinder;
	bCylinder.radius = body.world.scale_.num[0];

	Vector3 localTopCenter{ 0, 0.5f, 0 };
	Vector3 localBottomCenter{ 0, -0.5f, 0 };

	localTopCenter = localTopCenter * body.world.scale_.num[1];
	localBottomCenter = localBottomCenter * body.world.scale_.num[1];

	bCylinder.topCenter = localTopCenter + body.world.translation_;
	bCylinder.bottomCenter = localBottomCenter + body.world.translation_;
	return bCylinder;
}

void GamePlayScene::HandleSphereCylinderCollision(StructSphere& sphere, StructCylinder& cylinder, Vector3& correction) {
	if (!IsCollision(StructSphere{ sphere.center + correction, sphere.radius }, cylinder)) {
		sphere.center += correction;
	}
	else {
		correction = correction * 0.8f;
		sphere.center += correction;
	}
}

void GamePlayScene::CollisionConclusion() {
	//プレイヤーの判定取得
	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//プレイヤーと地面の当たり判定
	if (IsCollision(groundObb_, pSphere)) {
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

	//ボスと地面の当たり判定
	for (Body body : boss_->GetBody()) {
		StructCylinder bCylinder;
		//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
		bCylinder.radius = body.world.scale_.num[0];

		//ローカル空間での円柱の上端と下端の位置
		Vector3 localTopCenter{ 0, 0.1f, 0 };
		Vector3 localBottomCenter{ 0, -0.1f, 0 };

		//スケールを適用（Y軸方向のスケールを高さに反映）
		localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
		localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

		//平行移動を適用
		bCylinder.topCenter = localTopCenter + body.world.translation_;
		bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

		if (IsCollision(groundObb_, bCylinder)) {
			boss_->SetIsHitOnFloor(true, body);
			boss_->SetObjectPos(ground_->GetWorldTransform());
		}
		else {
			boss_->SetIsHitOnFloor(false, body);
		}
	}

	for (size_t i = 0; i < explosion_->GetFragments().size(); i++) {
		OBB fragmentOBB = CreateOBBFromEulerTransform(EulerTransform(
			explosion_->GetFragmentTransform(i).scale_,
			explosion_->GetFragmentTransform(i).rotation_,
			explosion_->GetFragmentTransform(i).translation_
		));

		// プレイヤー衝突判定
		if (IsCollision(fragmentOBB, pSphere)) {
			HandleFragmentCollision(i, pSphere, player_->GetVelocity(), fragmentOBB);
		}

		// エネミー衝突判定
		for (auto& enemy : enemys_) {
			StructSphere eSphere = enemy->GetStructSphere();
			if (IsCollision(fragmentOBB, eSphere)) {
				HandleFragmentCollision(i, eSphere, enemy->GetVelocity(), fragmentOBB);
			}
		}

		// ボスの部位との衝突判定
		for (Body body : boss_->GetBody()) {
			StructCylinder bCylinder = CreateCylinderFromBody(body);
			if (IsCollision(fragmentOBB, bCylinder)) {
				HandleFragmentCollision(i, pSphere, player_->GetVelocity(), fragmentOBB);
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
	sphere.radius = 1.5f;//大きさは仮決め

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
	for (int i = 0; i < maxAttempts_; ++i) {
		Vector3 pos = GenerateRandomPosition();
		if (IsValidPosition(pos)) {
			return pos;
		}
	}
	return Vector3{ 0.0f,0.0f,0.0f };
}