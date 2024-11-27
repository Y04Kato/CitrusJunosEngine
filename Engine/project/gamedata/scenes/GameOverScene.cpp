/**
 * @file GameOverScene.cpp
 * @brief ゲームオーバーシーンを管理
 * @author KATO
 * @date 未記録
 */

#include"GameOverScene.h"

void GameOverScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//Input
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	pageAll_ = textureManager_->Load("project/gamedata/resources/paper.png");
	start_ = textureManager_->Load("project/gamedata/resources/ui/pressSpace.png");
	game_ = textureManager_->Load("project/gamedata/resources/ui/gameover.png");

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

	sceneCount_ = 0;
}

void GameOverScene::Update() {
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

			//トランジション終わりにゲームシーンへ
			if (transition_->GetIsSceneEnd_() == false && player_->GetWorldTransform().translation_.num[2] >= 50.0f) {
				//各種初期化
				sceneCount_ = 0;
				isGameStart_ = true;
				debugCamera_->SetCamera(Vector3{ 26.7f,10.7f,-28.8f }, Vector3{ 0.0f,-0.3f,0.0f });

				//タイトルシーンへ
				sceneNo = TITLE_SCENE;
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

	ImGui::Begin("GameOverScene");
	ImGui::Text("SceneCount : %d", sceneCount_);
	ImGui::End();
}

void GameOverScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	uiSprite_[0]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	player_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GameOverScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	uiSprite_[1]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, Vector4{ 1.0f,1.0f,1.0f,spriteAlpha_ / 256.0f });
	uiSprite_[2]->Draw(uiSpriteTransform_, uiSpriteuvTransform_, uiSpriteMaterial_);

	transition_->Draw();
#pragma endregion
}

void GameOverScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::Random);
}

void GameOverScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GameOverScene::GameStartProcessing() {
	transition_->SceneStart();

	debugCamera_->SetCamera(Vector3{ 18.5f,10.0f,-44.0f }, Vector3{ 0.0f,-0.3f,0.0f });
	player_->SetWorldTransform(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetRotate(Vector3{ 0.0f,0.0f,-0.9f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });

	sceneChangeTimer_ = 0;

	isGameStart_ = false;
}