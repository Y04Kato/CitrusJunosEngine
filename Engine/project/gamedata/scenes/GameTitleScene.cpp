/**
 * @file GameTitleScene.cpp
 * @brief ゲームタイトルのシーンを管理
 * @author KATO
 * @date 未記録
 */

#include"GameTitleScene.h"

void GameTitleScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//テクスチャの初期化と読み込み
	textureManager_ = TextureManager::GetInstance();

	//SceneNumber
	sceneNumber_ = SceneNumber::GetInstance();

	//テクスチャ読み込み
	pageAll_ = textureManager_->Load("project/gamedata/resources/paper.png");
	start_ = textureManager_->Load("project/gamedata/resources/ui/pressSpace.png");
	title_ = textureManager_->Load("project/gamedata/resources/ui/title.png");
	tutorial_ = textureManager_->Load("project/gamedata/resources/ui/tutorial.png");
	skyboxTex_ = textureManager_->Load("project/gamedata/resources/vz_empty_space_cubemap_ue.dds");

	//Audioの初期化
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/system.mp3");

	//Spriteの初期化
	uiSpriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	uiSpriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	uiSpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	uiSprite_[0] = std::make_unique <CreateSprite>();
	uiSprite_[0]->Initialize(Vector2{ 100.0f,100.0f }, pageAll_);
	uiSprite_[0]->SetTextureInitialSize();
	uiSprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[1] = std::make_unique <CreateSprite>();
	uiSprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, start_);
	uiSprite_[1]->SetTextureInitialSize();
	uiSprite_[1]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[2] = std::make_unique <CreateSprite>();
	uiSprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, title_);
	uiSprite_[2]->SetTextureInitialSize();
	uiSprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	uiSprite_[3] = std::make_unique <CreateSprite>();
	uiSprite_[3]->Initialize(Vector2{ 100.0f,100.0f }, tutorial_);
	uiSprite_[3]->SetTextureInitialSize();
	uiSprite_[3]->SetAnchor(Vector2{ 0.5f,0.5f });

	//シーン遷移
	transition_ = Transition::GetInstance();
	transition_->Initialize();

	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());
	player_->SetScale(Vector3{ 5.0f,5.0f,5.0f });

	//パーティクルの初期化
	particleEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	particleEmitter_.count = 5;
	particleEmitter_.frequency = 0.2f;//0.2秒ごとに発生
	particleEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 10.0f,15.0f,10.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/circle.png");

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, particleEmitter_, accelerationField, particleResourceNum_);
	particle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	//各種モデル座標
	for (int i = 0; i < 3; i++) {
		worldModels_[i].Initialize();
	}
	worldModels_[1].translation_.num[1] = 12.0f;
	worldModels_[1].translation_.num[2] = 150.0f;

	//ステージモデルの初期化
	stage_[0].reset(Model::CreateModel("project/gamedata/resources/GarageMain", "GarageMain.obj"));
	stage_[1].reset(Model::CreateModel("project/gamedata/resources/GarageDoor", "GarageDoor.gltf"));
	stage_[0]->SetDirectionalLightFlag(true, 3);
	stage_[1]->SetDirectionalLightFlag(true, 3);
	stage_[1]->SetAnimationTime(stageAnimationTimer_);

	//
	for (int i = 0; i < modelMaxCount_; i++) {
		bgModel_[i] = std::make_unique <Model>();
	}
	bgModel_[0].reset(playerModel_.get());
	bgModel_[1].reset(Model::CreateModel("project/gamedata/resources/chest", "chest.obj"));
	bgModel_[2].reset(Model::CreateModel("project/gamedata/resources/chest", "chest.obj"));
	posterModelData_ = playerModel_->LoadModelFile("project/gamedata/resources/poster", "poster.obj");
	posterTexture_ = textureManager_->Load("project/gamedata/resources/poster/poster1.png");
	bgModel_[3].reset(Model::CreateModel(posterModelData_,posterTexture_));
	for (int i = 0; i < modelMaxCount_; i++) {
		worldTransformBGModel_[i].Initialize();
		bgModelMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		bgModel_[i]->SetDirectionalLightFlag(true, 3);
	}
	worldTransformBGModel_[0].translation_ = { -29.0f,1.0f,79.0f };
	worldTransformBGModel_[0].rotation_ = { -0.7f,0.0f,-0.25f };
	worldTransformBGModel_[0].scale_ = { 5.0f,5.0f,5.0f };
	bgModelMaterial_[0] = { 1.0f,1.0f,1.0f,1.0f };

	worldTransformBGModel_[1].translation_ = { -50.0f,0.0f,41.0f };
	worldTransformBGModel_[1].rotation_ = { 0.0f,-1.15f,0.0f };
	worldTransformBGModel_[1].scale_ = { 2.0f,2.0f,2.0f };
	bgModelMaterial_[1] = { 0.5f,0.25f,0.1f,1.0f };

	worldTransformBGModel_[2].translation_ = { 30.0f,0.0f,95.0f };
	worldTransformBGModel_[2].rotation_ = { 0.0f,0.35f,0.0f };
	worldTransformBGModel_[2].scale_ = { 2.0f,2.0f,2.0f };
	bgModelMaterial_[2] = { 0.9f,0.6f,0.4f,1.0f };

	worldTransformBGModel_[3].translation_ = { 69.0f,13.0f,149.0f };
	worldTransformBGModel_[3].rotation_ = { 0.0f,-1.65f,0.0f };
	worldTransformBGModel_[3].scale_ = { 1.0f,10.0f,9.0f };

	//SkyBox
	skyBox_ = std::make_unique <CreateSkyBox>();
	skyBox_->Initialize();
	worldTransformSkyBox_.Initialize();
	worldTransformSkyBox_.scale_ = { 1000.0f,1000.0f,1000.0f };
	skyBoxMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	skyBox_->SetDirectionalLightFlag(true, 3);

	//デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	//ViewProjectionの初期化
	viewProjection_.Initialize();

	//ライトの初期化
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.2f };
	pointLight_ = { {1.0f,0.8f,0.6f,1.0f},{player_->GetWorldTransform().translation_.num[0],4.6f,player_->GetWorldTransform().translation_.num[2]},0.15f ,1000.0f,0.1f };
}

void GameTitleScene::Update() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	//ステージ初期設定
	if (isGameStart_ == true) {
		GameStartProcessing();
	}

	//SPACE or Aボタンで次のページへ
	if (input_->TriggerKey(DIK_SPACE) && sceneCount_ < maxSceneCount_ || input_->TriggerAButton(joyState) && sceneCount_ < maxSceneCount_) {
		sceneCount_++;
		audio_->SoundPlayWave(soundData1_, 0.5f, false);
	}

	//
	transition_->SceneChangeUpdate();

	//Playerの更新
	player_->UpdateView();
	player_->SetViewProjection(&viewProjection_);

	//SkyBox更新
	worldTransformSkyBox_.UpdateMatrix();

	//
	for (int i = 0; i < modelMaxCount_; i++) {
		worldTransformBGModel_[i].UpdateMatrix();
	}
	
	//Particle更新
	particle_->Update();
	particle_->SetTranslate(player_->GetWorldTransform().translation_);

	//ライト更新
	directionalLights_->SetTarget(directionalLight_);
	pointLights_->SetTarget(pointLight_);

	//UIフェード用
	if (changeAlpha_ == false) {
		spriteAlpha_ -= fadeSpeed_;
		if (spriteAlpha_ <= 0) {
			changeAlpha_ = true;
		}
	}
	else if (changeAlpha_ == true) {
		spriteAlpha_ += fadeSpeed_;
		if (spriteAlpha_ >= 256) {
			changeAlpha_ = false;
		}
	}

	//各カウントの処理
	if (sceneCount_ == 0) {//タイトル
		debugCamera_->SetCamera(Vector3{ 26.7f,10.7f,-28.8f }, Vector3{ 0.0f,-0.3f,0.0f });
		player_->SetTranslate(Vector3{ 0.0f,1.0f,0.0f });

	}
	if (sceneCount_ == 1) {//ルール説明
		debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,-29.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
		player_->SetVelocity({ 0.0f,0.0f,0.0f });
	}
	if (sceneCount_ == 2) {//ゲームスタート
		debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,50.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
		player_->SetVelocity({ 0.0f,0.0f,2.0f });

		if (player_->GetWorldTransform().translation_.num[2] == transitionStartPoint_) {
			transition_->SceneEnd();
		}
		
		//トランジション終わりにゲームシーンへ
		if (transition_->GetIsSceneEnd_() == false && player_->GetWorldTransform().translation_.num[2] >= transitionStartPoint_) {
			//各種初期化
			sceneCount_ = 0;
			stageAnimationTimer_ = stageAnimationStartTime_;
			isGameStart_ = true;
			player_->SetTranslate(Vector3{ 0.0f,1.0f,0.0f });
			debugCamera_->SetCamera(Vector3{ 26.7f,10.7f,-28.8f }, Vector3{ 0.0f,-0.3f,0.0f });

			//ゲームシーンへ
			sceneNumber_->SetSceneNumber(GAME_SCENE);
		}
	}

	//モデルアニメーション開始
	if (sceneCount_ >= 1) {
		stageAnimationTimer_ += stageAnimationTime_;
	}
	stage_[1]->SetAnimationTime(stageAnimationTimer_);

	//ワールド座標更新
	for (int i = 0; i < 3; i++) {
		worldModels_[i].UpdateMatrix();
	}

	//カメラとビュープロジェクション更新
	debugCamera_->Update();
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	//ImGui
	ImGui::Begin("TitleScene");
	ImGui::Text("SceneCount : %d", sceneCount_);
	if (ImGui::Button("DebugScene")) {
		debugCamera_->SetCamera(Vector3{ 0.0f,0.0f,-20.0f }, Vector3{ 0.0f,0.0f,0.0f });
		sceneNumber_->SetSceneNumber(DEBUG_SCENE);
	}
	if (ImGui::Button("SceneEnd")) {
		debugCamera_->SetCamera(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		sceneNumber_->SetSceneNumber(TEST_SCENE);
	}
	if (ImGui::Button("SceneStart")) {
		transition_->SceneStart();
	}
	if (ImGui::Button("SceneEnd")) {
		transition_->SceneEnd();
	}
	ImGui::DragFloat3("LightColor", pointLight_.color.num, 1.0f);
	ImGui::DragFloat3("lightPosition", pointLight_.position.num, 0.1f);
	ImGui::DragFloat("lightIntensity", &pointLight_.intensity, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("lightRadius", &pointLight_.radius, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("lightDecay", &pointLight_.decay, 0.1f, 0.0f, 10.0f);
	ImGui::End();

}

void GameTitleScene::Draw() {
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
	stage_[0]->Draw(worldModels_[0], viewProjection_, Vector4{1.0f,1.0f,1.0f,1.0f});
	stage_[1]->Draw(worldModels_[1], viewProjection_, Vector4{1.0f,1.0f,1.0f,1.0f});
	for (int i = 0; i < modelMaxCount_; i++) {
		bgModel_[i]->Draw(worldTransformBGModel_[i], viewProjection_, bgModelMaterial_[i]);
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

void GameTitleScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	if (sceneCount_ != 2) {
		uiSprite_[1]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
		if (sceneCount_ == 0) {
			uiSprite_[2]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
		}
		if (sceneCount_ == 1) {
			uiSprite_[3]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);
		}
	}

	//
	transition_->Draw();

#pragma endregion
}

void GameTitleScene::DrawPostEffect() {
	if (sceneCount_ == 0) {
		CJEngine_->renderer_->Draw(PipelineType::Gaussian);
	}
	if (sceneCount_ == 1) {
		CJEngine_->renderer_->Draw(PipelineType::Scanlines);
	}
	if (sceneCount_ == 2) {
		CJEngine_->renderer_->Draw(PipelineType::RadialBlur);
	}
}

void GameTitleScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GameTitleScene::GameStartProcessing() {
	transition_->SceneStart();

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.2f };
	pointLight_ = { {1.0f,0.8f,0.6f,1.0f},{player_->GetWorldTransform().translation_.num[0],4.6f,player_->GetWorldTransform().translation_.num[2]},0.15f ,1000.0f,0.1f };

	isGameStart_ = false;
}