#include "GamePlayScene.h"
#include "components/utilities/globalVariables/GlobalVariables.h"

void GamePlayScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//テクスチャ
	texture_ = 0;
	uvResourceNum_ = textureManager_->Load("project/gamedata/resources/uvChecker.png");

	monsterBallResourceNum_ = textureManager_->Load("project/gamedata/resources/monsterBall.png");

	//三角形
	for (int i = 0; i < 2; i++) {
		triangle_[i] = std::make_unique <CreateTriangle>();
		triangle_[i]->Initialize();
		worldTransformTriangle_[i].Initialize();
		triangleMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
	}

	worldTransformTriangle_[1].rotation_.num[1] = 0.7f;

	isTriangleDraw1_ = false;
	isTriangleDraw2_ = false;

	//スプライト
	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	SpriteuvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	sprite_ = std::make_unique <CreateSprite>();
	sprite_->Initialize(Vector2{ 100.0f,100.0f }, uvResourceNum_, false, false);
	//sprite_->SetTextureLTSize(Vector2{ 0.0f,0.0f }, Vector2{ 100.0f,100.0f });
	sprite_->SetTextureInitialSize();

	isSpriteDraw_ = false;

	//球体
	sphere_ = std::make_unique <CreateSphere>();
	sphere_->Initialize();
	worldTransformSphere_.Initialize();
	sphereMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	isSphereDraw_ = false;

	//objモデル
	model_.reset(Model::CreateModelFromObj("project/gamedata/resources/fence", "fence.obj"));
	worldTransformModel_.Initialize();
	modelMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

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
	collisionManager_ = CollisionManager::GetInstance();

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GamePlayScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
}

void GamePlayScene::Update() {

	ApplyGlobalVariables();

	collisionManager_->ClearColliders();
	collisionManager_->CheckAllCollision();

	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	if (input_->PressKey(DIK_A)) {
		OutputDebugStringA("Press A\n");
	}
	if (input_->ReleaseKey(DIK_S)) {
		OutputDebugStringA("Release S\n");
	}
	if (input_->TriggerKey(DIK_D)) {
		OutputDebugStringA("Trigger D\n");
	}

	for (int i = 0; i < 2; i++) {
		worldTransformTriangle_[i].UpdateMatrix();
	}
	worldTransformSphere_.UpdateMatrix();
	worldTransformModel_.UpdateMatrix();

	ImGui::Begin("debug");
	ImGui::Text("GamePlayScene");
	if (ImGui::TreeNode("Triangle")) {//三角形
		if (ImGui::Button("DrawTriangle1")) {
			if (isTriangleDraw1_ == false) {
				isTriangleDraw1_ = true;
			}
			else {
				isTriangleDraw1_ = false;
			}
		}
		if (ImGui::Button("DrawTriangle2")) {
			if (isTriangleDraw2_ == false) {
				isTriangleDraw2_ = true;
			}
			else {
				isTriangleDraw2_ = false;
			}
		}
		if (isTriangleDraw1_ == true) {
			if (ImGui::TreeNode("Triangle1")) {
				ImGui::DragFloat3("Translate", worldTransformTriangle_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformTriangle_[0].rotation_.num, 0.05f);
				ImGui::DragFloat2("Scale", worldTransformTriangle_[0].scale_.num, 0.05f);
				ImGui::ColorEdit4("", triangleMaterial_[0].num, 0);
				ImGui::TreePop();
			}
		}
		if (isTriangleDraw2_ == true) {
			if (ImGui::TreeNode("Triangle2")) {
				ImGui::DragFloat3("Translate", worldTransformTriangle_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformTriangle_[1].rotation_.num, 0.05f);
				ImGui::DragFloat2("Scale", worldTransformTriangle_[1].scale_.num, 0.05f);
				ImGui::ColorEdit4("", triangleMaterial_[1].num, 0);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sphere")) {//球体
		if (ImGui::Button("DrawSphere")) {
			if (isSphereDraw_ == false) {
				isSphereDraw_ = true;
			}
			else {
				isSphereDraw_ = false;
			}
		}
		ImGui::DragFloat3("Translate", worldTransformSphere_.translation_.num, 0.05f);
		ImGui::DragFloat3("Rotate", worldTransformSphere_.rotation_.num, 0.05f);
		ImGui::DragFloat3("Scale", worldTransformSphere_.scale_.num, 0.05f);
		ImGui::ColorEdit4("", sphereMaterial_.num, 0);
		ImGui::SliderInt("ChangeTexture", &texture_, 0, 1);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sprite")) {//スプライト
		if (ImGui::Button("DrawSprite")) {
			if (isSpriteDraw_ == false) {
				isSpriteDraw_ = true;
			}
			else {
				isSpriteDraw_ = false;
			}
		}
		ImGui::DragFloat2("Translate", spriteTransform_.translate.num, 0.05f);
		ImGui::DragFloat3("Rotate", spriteTransform_.rotate.num, 0.05f);
		ImGui::DragFloat2("Scale", spriteTransform_.scale.num, 0.05f);
		ImGui::ColorEdit4("", spriteMaterial_.num, 0);
		ImGui::DragFloat2("uvScale", SpriteuvTransform_.scale.num, 0.1f);
		ImGui::DragFloat3("uvTranslate", SpriteuvTransform_.translate.num, 0.1f);
		ImGui::DragFloat("uvRotate", &SpriteuvTransform_.rotate.num[2], 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Model")) {//objモデル
		if (ImGui::Button("DrawModel")) {
			if (isModelDraw_ == false) {
				isModelDraw_ = true;
			}
			else {
				isModelDraw_ = false;
			}
		}
		ImGui::DragFloat3("Translate", worldTransformModel_.translation_.num, 0.05f);
		ImGui::DragFloat3("Rotate", worldTransformModel_.rotation_.num, 0.05f);
		ImGui::DragFloat3("Scale", worldTransformModel_.scale_.num, 0.05f);
		ImGui::TreePop();
	}

	ImGui::Text("%f", ImGui::GetIO().Framerate);

	ImGui::End();
}

void GamePlayScene::Draw() {
#pragma region 3Dオブジェクト描画
	CJEngine_->PreDraw3D();

	if (isTriangleDraw1_) {//Triangle描画
		triangle_[0]->Draw(worldTransformTriangle_[0], viewProjection_, triangleMaterial_[0], uvResourceNum_);
	}
	if (isTriangleDraw2_) {//Triangle描画
		triangle_[1]->Draw(worldTransformTriangle_[1], viewProjection_, triangleMaterial_[1], uvResourceNum_);
	}

	if (isSphereDraw_) {
		sphere_->Draw(worldTransformSphere_, viewProjection_, sphereMaterial_, texture_);
	}

	if (isModelDraw_) {
		model_->Draw(worldTransformModel_, viewProjection_, modelMaterial_);
	}
#pragma endregion

#pragma region パーティクル描画
	CJEngine_->PreDrawParticle();


#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();

	if (isSpriteDraw_) {//Sprite描画
		sprite_->Draw(spriteTransform_, SpriteuvTransform_, spriteMaterial_);
	}
#pragma endregion
}

void GamePlayScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
}

void GamePlayScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GamePlayScene";
}