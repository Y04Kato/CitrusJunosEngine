/**
 * @file GameClearScene.cpp
 * @brief ゲームクリアシーンを管理
 * @author KATO
 * @date 未記録
 */

#include"GameClearScene.h"

void GameClearScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	//SceneNumber
	sceneNumber_ = SceneNumber::GetInstance();

	pageAll_ = textureManager_->Load("project/gamedata/resources/paper.png");
	start_ = textureManager_->Load("project/gamedata/resources/ui/pressSpace.png");
	game_ = textureManager_->Load("project/gamedata/resources/ui/gameclear.png");

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/system.mp3");

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
	uiSprite_[2]->Initialize(Vector2{ 100.0f,100.0f }, game_);
	uiSprite_[2]->SetTextureInitialSize();
	uiSprite_[2]->SetAnchor(Vector2{ 0.5f,0.5f });

	//プレイヤーの初期化
	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());
	player_->SetScale(Vector3{ 5.0f,5.0f,5.0f });

	//シーン遷移
	transition_ = Transition::GetInstance();

	//デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//ViewProjectionの初期化
	viewProjection_.Initialize();

	//パーティクルの初期化
	particleEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.scale = { 0.5f,0.5f,0.5f };
	particleEmitter_.count = 5;
	particleEmitter_.frequency = 0.2f;//0.2秒ごとに発生
	particleEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationFieldRight.acceleration = { 1.0f,15.0f,1.0f };
	accelerationFieldRight.area.min = { 12.0f,-1.0f,-1.0f };
	accelerationFieldRight.area.max = { 14.0f,1.0f,1.0f };

	accelerationFieldLeft.acceleration = { 1.0f,15.0f,1.0f };
	accelerationFieldLeft.area.min = { -14.0f,-1.0f,-1.0f };
	accelerationFieldLeft.area.max = { -12.0f,1.0f,1.0f };

	particleResourceNum_ = textureManager_->Load("project/gamedata/resources/transition/baseBox.png");

	particleRight_ = std::make_unique <CreateParticle>();
	particleRight_->Initialize(100, particleEmitter_, accelerationFieldRight, particleResourceNum_);
	particleRight_->SetisBillBoard(false);

	particleLeft_ = std::make_unique <CreateParticle>();
	particleLeft_->Initialize(100, particleEmitter_, accelerationFieldLeft, particleResourceNum_);
	particleLeft_->SetisBillBoard(false);

	sceneCount_ = 0;
}

void GameClearScene::Update() {
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	//ステージ初期設定
	if (isGameStart_ == true) {
		GameStartProcessing();
	}

	if (input_->TriggerKey(DIK_SPACE) && sceneCount_ < 1 || input_->TriggerAButton(joyState) && sceneCount_ < 1) {
		sceneCount_++;
		audio_->SoundPlayWave(soundData1_, 0.5f, false);
	}

	//Playerの更新
	player_->UpdateView();
	player_->SetViewProjection(&viewProjection_);

	transition_->Update();

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

	if (sceneCount_ == 0) {

	}
	if (sceneCount_ == 1) {
		if (sceneChangeTimer_ >= sceneChangeMaxTime_) {
			debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,50.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
			player_->SetVelocity({ 0.0f,0.0f,2.0f });

			if (player_->GetWorldTransform().translation_.num[2] == 50.0f) {
				transition_->SceneEnd();
			}

			//トランジション終わりにタイトルシーンへ
			if (transition_->GetIsSceneEnd_() == false && player_->GetWorldTransform().translation_.num[2] >= 50.0f) {
				//各種初期化
				sceneCount_ = 0;
				isGameStart_ = true;
				debugCamera_->SetCamera(Vector3{ 26.7f,10.7f,-28.8f }, Vector3{ 0.0f,-0.3f,0.0f });

				//タイトルシーンへ
				sceneNumber_->SetSceneNumber(TITLE_SCENE);
			}
		}
		else if (sceneChangeTimer_ >= sceneChangeMaxTime_ / 2) {
			player_->SetRotate(Vector3{ 0.0f,player_->GetWorldTransform().rotation_.num[1],0.0f });

			sceneChangeTimer_++;
		}
		else {
			debugCamera_->MovingCamera(Vector3{ 0.0f,10.7f,-29.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
			player_->SetRotate(Lerp(player_->GetWorldTransform().rotation_, Vector3{ 0.0f,player_->GetWorldTransform().rotation_.num[1],0.0f }, 0.1f));
			sceneChangeTimer_++;
		}
	}

	//カメラとビュープロジェクション更新
	debugCamera_->Update();
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	//Particle更新
	particleRight_->Update();
	particleLeft_->Update();

	particleRight_->SetTranslate(particleposRight_);
	particleLeft_->SetTranslate(particleposLeft_);

	ImGui::Begin("GameClearScene");
	ImGui::Text("SceneCount : %d", sceneCount_);
	ImGui::DragFloat3("particlePos", particleposRight_.num, 0.1f);
	ImGui::End();
}

void GameClearScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	uiSprite_[0]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	player_->Draw(viewProjection_);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	particleRight_->Draw(viewProjection_);
	particleLeft_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GameClearScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	uiSprite_[1]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
	uiSprite_[2]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);

	transition_->Draw();
#pragma endregion
}

void GameClearScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::LensDistortion);
}

void GameClearScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GameClearScene::GameStartProcessing() {
	transition_->SceneStart();

	debugCamera_->SetCamera(Vector3{ 0.0f,10.7f,-29.0f }, Vector3{ 0.0f,0.0f,0.0f });
	player_->SetTranslate(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetRotate(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });

	sceneChangeTimer_ = 0;

	isGameStart_ = false;
}