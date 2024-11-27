/**
 * @file DebugScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "DebugScene.h"

void DebugScene::Initialize() {
	//CJEngine
	CJEngine_ = CitrusJunosEngine::GetInstance();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//TextureManager
	textureManager_ = TextureManager::GetInstance();

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/metal.mp3");

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();

	//Player
	player_ = std::make_unique<Player>();
	playerModel_.reset(Model::CreateModel("project/gamedata/resources/player", "player.obj"));
	playerModel_->SetDirectionalLightFlag(true, 3);
	player_->Initialize(playerModel_.get());

	//Enemy
	enemyModel_.reset(Model::CreateModel("project/gamedata/resources/enemy", "enemy.obj"));
	enemyModel_->SetDirectionalLightFlag(true, 2);

	//Ground
	ground_ = std::make_unique<Ground>();
	groundModel_.reset(Model::CreateModel("project/gamedata/resources/floor", "Floor.obj"));
	groundModel_->SetDirectionalLightFlag(true, 3);
	ground_->Initialize(groundModel_.get(), { 0.0f,0.0f,-5.0f }, { 30.0f,1.0f,30.0f });

	//Objects
	ObjModelData_ = playerModel_->LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	//Editors
	editors_ = Editors::GetInstance();
	editors_->AddGroupName((char*)"DebugScene");

	//Lights
	directionalLights_ = DirectionalLights::GetInstance();
	pointLights_ = PointLights::GetInstance();
}

void DebugScene::Update() {
	ImGui::Begin("DebugOperate");
	if (ImGui::Button("TitleScene")) {
		SceneEndProcessing();
		sceneNo = TITLE_SCENE;
	}
	if (ImGui::Button("GameClearScene")) {
		SceneEndProcessing();
		sceneNo = CLEAR_SCENE;
	}
	if (ImGui::Button("GameOverScene")) {
		SceneEndProcessing();
		sceneNo = OVER_SCENE;
	}
	ImGui::Text("CameraReset:Q key");
	ImGui::Checkbox("isEditorMode", &isEditorMode_);

	if (ImGui::Button("PlayerReset")) {//Player配置をリセット
		player_->SetTranslate(Vector3{ 0.0f,0.2f,0.0f });
		player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
		player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });
	}

	if (ImGui::Button("EnemyAllDelete")) {//敵を全て削除
		for (Enemy* enemy : enemys_) {
			enemy->SetisDead();
		}
		enemys_.remove_if([&](Enemy* enemy) {
			if (enemy->GetisDead()) {
				delete enemy;
				return true;
			}
			return false;
			});
	}

	ImGui::DragFloat("PushbackMultiplier", &pushbackMultiplier_, 0.5f, 1.0f, 20.0f);
	ImGui::DragFloat("RepulsionCoefficient", &repulsionCoefficient_, 0.5f, 0.0f, 10.0f);

	if (ImGui::Button("EnemyHorizontalPosition")) {//敵を横に配置
		SetEnemy(Vector3{ 24.0f,1.5f,0.0f }, Vector3{ -1.0f,0.0f,0.0f });
		SetEnemy(Vector3{ -24.0f,1.5f,0.0f }, Vector3{ 1.0f,0.0f,0.0f });
	}

	if (ImGui::Button("EnemyVerticalPosition")) {//敵を縦に配置
		SetEnemy(Vector3{ 0.0f,1.5f,24.0f }, Vector3{ 0.0f,0.0f,-1.0f });
		SetEnemy(Vector3{ 0.0f,1.5f,-24.0f }, Vector3{ 0.0f,0.0f,1.0f });
	}

	if (ImGui::Button("EnemyCrossPosition")) {//敵を十字に配置
		SetEnemy(Vector3{ 24.0f,1.5f,0.0f }, Vector3{ -1.0f,0.0f,0.0f });
		SetEnemy(Vector3{ -24.0f,1.5f,0.0f }, Vector3{ 1.0f,0.0f,0.0f });
		SetEnemy(Vector3{ 0.0f,1.5f,24.0f }, Vector3{ 0.0f,0.0f,-1.0f });
		SetEnemy(Vector3{ 0.0f,1.5f,-24.0f }, Vector3{ 0.0f,0.0f,1.0f });
	}

	ImGui::End();

	ImGui::Begin("DebugData");
	ImGui::Text("PlayerPos : %f %f %f", player_->GetWorldTransform().translation_.num[0], player_->GetWorldTransform().translation_.num[1], player_->GetWorldTransform().translation_.num[2]);
	ImGui::End();

	//状態初期化処理
	if (isReset == true) {
		ResetProcessing();
		isReset = false;
	}

	//Player更新
	player_->SetViewProjection(&viewProjection_);
	if (isEditorMode_ == false) {
		player_->Update();
	}

	//Enemy更新
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	//Enemyが場外へ行ったら削除する
	enemys_.remove_if([&](Enemy* enemy) {
		if (enemy->GetisDead()) {
			delete enemy;
			return true;
		}
		return false;
		});

	//Ground更新
	ground_->Update();
	groundObb_.center = ground_->GetWorldTransform().GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(ground_->GetWorldTransform().rotation_), groundObb_.orientation);
	groundObb_.size = ground_->GetWorldTransform().scale_;

	//Edirots更新
	editors_->Update();

	//Camera更新
	debugCamera_->Update();

	//Light更新
	directionalLights_->SetTarget(directionalLight_);
	pointLight_.position.num[0] = player_->GetWorldTransform().GetWorldPos().num[0];
	pointLight_.position.num[2] = player_->GetWorldTransform().GetWorldPos().num[2];
	pointLights_->SetTarget(pointLight_);

	//ビュープロジェクション更新
	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	//当たり判定
	CollisionConclusion();

	//カメラリセット
	if (input_->TriggerKey(DIK_Q)) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);
	}
}

void DebugScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	//Player
	player_->Draw(viewProjection_);

	//Enemy
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}

	//Ground
	ground_->Draw(viewProjection_);

	//Editors
	editors_->Draw(viewProjection_);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	audio_->SoundUnload(&soundData1_);

	editors_->Finalize();
}

void DebugScene::ResetProcessing() {
	player_->SetTranslate(Vector3{ 10.0f,0.2f,10.0f });
	player_->SetVelocity(Vector3{ 0.0f,0.0f,0.0f });
	player_->SetScale(Vector3{ 1.0f,1.0f,1.0f });

	debugCamera_->MovingCamera(Vector3{ 0.0f,44.7f,-55.2f }, Vector3{ 0.8f,0.0f,0.0f }, 0.05f);

	editors_->SetModels(ObjModelData_, ObjTexture_);
	editors_->SetGroupName((char*)"DebugScene");

	directionalLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,-1.0f,0.0f},0.5f };
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };
}

void DebugScene::SceneEndProcessing() {
	isReset = true;
	directionalLight_.intensity = 1.0f;
	pointLight_ = { {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},1.0f ,5.0f,1.0f };

	editors_->Finalize();
}

void DebugScene::CollisionConclusion() {
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

				player_->SetTranslate(pSphere.center);
				enemy->SetWorldTransform(eSphere.center);
			}

			//反発処理
			std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, player_->GetVelocity(), 1.0f, enemy->GetVelocity(), repulsionCoefficient_, Normalize(player_->GetWorldTransform().GetWorldPos() - enemy->GetWorldTransform().GetWorldPos()));
			player_->SetVelocity(pair.first);
			enemy->SetVelocity(pair.second);

			audio_->SoundPlayWave(soundData1_, 0.1f, false);
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
					Vector3 correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
					eSphere1.center = eSphere1.center - correction;
					eSphere2.center = eSphere2.center + correction;

					enemy1->SetWorldTransform(eSphere1.center);
					enemy2->SetWorldTransform(eSphere2.center);
				}

				//反発処理
				std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, enemy1->GetVelocity(), 1.0f, enemy2->GetVelocity(), repulsionCoefficient_, Normalize(enemy1->GetWorldTransform().GetWorldPos() - enemy2->GetWorldTransform().GetWorldPos()));
				enemy1->SetVelocity(pair.first);
				enemy2->SetVelocity(pair.second);

				audio_->SoundPlayWave(soundData1_, 0.1f, false);
			}
		}
	}

	//プレイヤーとオブジェクトの当たり判定
	for (Obj obj : editors_->GetObj()) {
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

				player_->SetTranslate(pSphere.center);
			}

			//反発処理
			Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(pSphere, player_->GetVelocity(), objOBB, repulsionCoefficient_);
			player_->SetVelocity(velocity);

			audio_->SoundPlayWave(soundData1_, 0.1f, false);
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
					Vector3 correction = Normalize(direction) * overlap * pushbackMultiplierObj_;
					eSphere.center += correction;

					enemy->SetWorldTransform(eSphere.center);
				}

				//反発処理
				Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), objOBB, repulsionCoefficient_);
				enemy->SetVelocity(velocity);

				audio_->SoundPlayWave(soundData1_, 0.1f, false);
			}
		}
	}
}

void DebugScene::SetEnemy(Vector3 pos, Vector3 velocity) {
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModel_.get());
	enemy->SetWorldTransform(pos);
	enemy->SetVelocity(velocity);
	enemys_.push_back(enemy);
}