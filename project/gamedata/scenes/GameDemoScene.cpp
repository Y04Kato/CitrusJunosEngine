#include "GameDemoScene.h"
#include "components/utilities/globalVariables/GlobalVariables.h"

void GameDemoScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//テクスチャ
	uvResourceNum_ = textureManager_->Load("project/gamedata/resources/uvChecker.png");

	monsterBallResourceNum_ = textureManager_->Load("project/gamedata/resources/monsterBall.png");

	kaedeResourceNum_ = textureManager_->Load("project/gamedata/resources/kaede.png");

	cjEngineResourceNum_ = textureManager_->Load("project/gamedata/resources/CitrusJunosEngine.png");

	//三角形
	for (int i = 0; i < 2; i++) {
		triangle_[i] = std::make_unique <CreateTriangle>();
		triangle_[i]->Initialize();
		worldTransformTriangle_[i].Initialize();
		triangleMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		isTriangleDraw_[i] = false;
	}

	worldTransformTriangle_[1].rotation_.num[1] = 0.7f;

	//スプライト
	for (int i = 0; i < 2; i++) {
		spriteMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		spriteTransform_[i] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280/2.0f,720/2.0f,0.0f} };
		SpriteuvTransform_[i] = {
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
		};
		sprite_[i] = std::make_unique <CreateSprite>();

		isSpriteDraw_[i] = false;
	}
	sprite_[0]->Initialize(Vector2{ 200.0f,200.0f }, uvResourceNum_);
	sprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, monsterBallResourceNum_);
	sprite_[0]->SetTextureLTSize(Vector2{ 0.0f,0.0f }, Vector2{ 50.0f,50.0f });
	sprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });
	sprite_[1]->SetTextureInitialSize();

	//パーティクル
	for (int i = 0; i < 2; i++) {
		testEmitter_[i].transform.translate = { 0.0f,0.0f,0.0f };
		testEmitter_[i].transform.rotate = { 0.0f,0.0f,0.0f };
		testEmitter_[i].transform.scale = { 1.0f,1.0f,1.0f };
		testEmitter_[i].count = 5;
		testEmitter_[i].frequency = 0.5f;//0.5秒ごとに発生
		testEmitter_[i].frequencyTime = 0.0f;//発生頻度の時刻

		accelerationField_[i].acceleration = { 15.0f,0.0f,0.0f };
		accelerationField_[i].area.min = { -1.0f,-1.0f,-1.0f };
		accelerationField_[i].area.max = { 1.0f,1.0f,1.0f };

		particle_[i] = std::make_unique <CreateParticle>();
	}
	particle_[0]->Initialize(100, testEmitter_[0], accelerationField_[0], cjEngineResourceNum_);
	particle_[1]->Initialize(100, testEmitter_[1], accelerationField_[1], kaedeResourceNum_);

	//球体
	for (int i = 0; i < 2; i++) {
		sphere_[i] = std::make_unique <CreateSphere>();
		sphere_[i]->Initialize();
		worldTransformSphere_[i].Initialize();
		sphereMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		sphere_[i]->SetDirectionalLightFlag(true, 3);
		texture_[i] = 0;
	}

	//objモデル
	model_[0].reset(Model::CreateModelFromObj("project/gamedata/resources/drum", "drum.obj"));
	model_[1].reset(Model::CreateModelFromObj("project/gamedata/resources/chest", "chest.obj"));
	model_[2].reset(Model::CreateModelFromObj("project/gamedata/resources/terrain", "terrain.obj"));
	for (int i = 0; i < 3; i++) {
		worldTransformModel_[i].Initialize();
		modelMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		model_[i]->SetDirectionalLightFlag(true, 3);
	}

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoadWave("project/gamedata/resources/LethargicWitch.wav");
	soundData2_ = audio_->SoundLoadWave("project/gamedata/resources/system.wav");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, true);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();

	//CollisionManager
	collisionManager_ = CollisionManager::GetInstance();

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "GameDemoScene";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
}

void GameDemoScene::Update() {

	ApplyGlobalVariables();

	collisionManager_->ClearColliders();
	collisionManager_->CheckAllCollision();

	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	if (input_->PressKey(DIK_A)) {
		OutputDebugStringA("Press A\n");
		audio_->SoundStopWave(&soundData1_);
	}
	if (input_->ReleaseKey(DIK_S)) {
		OutputDebugStringA("Release S\n");
	}
	if (input_->TriggerKey(DIK_D)) {
		OutputDebugStringA("Trigger D\n");
		audio_->SoundPlayWave(soundData2_, 0.1f, false);
	}

	for (int i = 0; i < 2; i++) {
		worldTransformTriangle_[i].UpdateMatrix();
		worldTransformSphere_[i].UpdateMatrix();
	}

	for (int i = 0; i < 3; i++) {
		worldTransformModel_[i].UpdateMatrix();
	}

	ImGui::Begin("debug");
	ImGui::Text("GameDemoScene");
	if (ImGui::TreeNode("Triangle")) {//三角形
		if (ImGui::Button("DrawTriangle1")) {
			if (isTriangleDraw_[0] == false) {
				isTriangleDraw_[0] = true;
			}
			else {
				isTriangleDraw_[0] = false;
			}
		}
		if (ImGui::Button("DrawTriangle2")) {
			if (isTriangleDraw_[1] == false) {
				isTriangleDraw_[1] = true;
			}
			else {
				isTriangleDraw_[1] = false;
			}
		}
		if (isTriangleDraw_[0] == true) {
			if (ImGui::TreeNode("Triangle1")) {
				ImGui::DragFloat3("Translate", worldTransformTriangle_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformTriangle_[0].rotation_.num, 0.05f);
				ImGui::DragFloat2("Scale", worldTransformTriangle_[0].scale_.num, 0.05f);
				ImGui::ColorEdit4("", triangleMaterial_[0].num, 0);
				ImGui::TreePop();
			}
		}
		if (isTriangleDraw_[1] == true) {
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
		if (ImGui::Button("DrawSphere1")) {
			if (isSphereDraw_[0] == false) {
				isSphereDraw_[0] = true;
			}
			else {
				isSphereDraw_[0] = false;
			}
		}
		if (ImGui::Button("DrawSphere2")) {
			if (isSphereDraw_[1] == false) {
				isSphereDraw_[1] = true;
			}
			else {
				isSphereDraw_[1] = false;
			}
		}
		if (isSphereDraw_[0] == true) {
			if (ImGui::TreeNode("Sphere1")) {
				ImGui::DragFloat3("Translate", worldTransformSphere_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformSphere_[0].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformSphere_[0].scale_.num, 0.05f);
				ImGui::ColorEdit4("", sphereMaterial_[0].num, 0);
				ImGui::SliderInt("ChangeTexture", &texture_[0], 0, 1);
				ImGui::TreePop();
			}
		}
		if (isSphereDraw_[1] == true) {
			if (ImGui::TreeNode("Sphere2")) {
				ImGui::DragFloat3("Translate", worldTransformSphere_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformSphere_[1].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformSphere_[1].scale_.num, 0.05f);
				ImGui::ColorEdit4("", sphereMaterial_[1].num, 0);
				ImGui::SliderInt("ChangeTexture", &texture_[1], 0, 1);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sprite")) {//スプライト
		if (ImGui::Button("DrawSprite1")) {
			if (isSpriteDraw_[0] == false) {
				isSpriteDraw_[0] = true;
			}
			else {
				isSpriteDraw_[0] = false;
			}
		}
		if (ImGui::Button("DrawSprite2")) {
			if (isSpriteDraw_[1] == false) {
				isSpriteDraw_[1] = true;
			}
			else {
				isSpriteDraw_[1] = false;
			}
		}
		if (isSpriteDraw_[0] == true) {
			if (ImGui::TreeNode("Sprite1")) {
				ImGui::DragFloat2("Translate", spriteTransform_[0].translate.num, 0.05f);
				ImGui::DragFloat3("Rotate", spriteTransform_[0].rotate.num, 0.05f);
				ImGui::DragFloat2("Scale", spriteTransform_[0].scale.num, 0.05f);
				ImGui::ColorEdit4("", spriteMaterial_[0].num, 0);
				ImGui::DragFloat2("uvScale", SpriteuvTransform_[0].scale.num, 0.1f);
				ImGui::DragFloat3("uvTranslate", SpriteuvTransform_[0].translate.num, 0.1f);
				ImGui::DragFloat("uvRotate", &SpriteuvTransform_[0].rotate.num[2], 0.1f);
				ImGui::TreePop();
			}
		}
		if (isSpriteDraw_[1] == true) {
			if (ImGui::TreeNode("Sprite2")) {
				ImGui::DragFloat2("Translate", spriteTransform_[1].translate.num, 0.05f);
				ImGui::DragFloat3("Rotate", spriteTransform_[1].rotate.num, 0.05f);
				ImGui::DragFloat2("Scale", spriteTransform_[1].scale.num, 0.05f);
				ImGui::ColorEdit4("", spriteMaterial_[1].num, 0);
				ImGui::DragFloat2("uvScale", SpriteuvTransform_[1].scale.num, 0.1f);
				ImGui::DragFloat3("uvTranslate", SpriteuvTransform_[1].translate.num, 0.1f);
				ImGui::DragFloat("uvRotate", &SpriteuvTransform_[1].rotate.num[2], 0.1f);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Model")) {//objモデル
		if (ImGui::Button("DrawModel1")) {
			if (isModelDraw_[0] == false) {
				isModelDraw_[0] = true;
			}
			else {
				isModelDraw_[0] = false;
			}
		}
		if (ImGui::Button("DrawModel2")) {
			if (isModelDraw_[1] == false) {
				isModelDraw_[1] = true;
			}
			else {
				isModelDraw_[1] = false;
			}
		}
		if (ImGui::Button("DrawModel3")) {
			if (isModelDraw_[2] == false) {
				isModelDraw_[2] = true;
			}
			else {
				isModelDraw_[2] = false;
			}
		}
		if (isModelDraw_[0] == true) {
			if (ImGui::TreeNode("Model1")) {
				ImGui::DragFloat3("Translate", worldTransformModel_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformModel_[0].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformModel_[0].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		if (isModelDraw_[1] == true) {
			if (ImGui::TreeNode("Model2")) {
				ImGui::DragFloat3("Translate2", worldTransformModel_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate2", worldTransformModel_[1].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale2", worldTransformModel_[1].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		if (isModelDraw_[2] == true) {
			if (ImGui::TreeNode("Model3")) {
				ImGui::DragFloat3("Translate3", worldTransformModel_[2].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate3", worldTransformModel_[2].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale3", worldTransformModel_[2].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle")) {//パーティクル
		if (ImGui::Button("DrawParticle1")) {
			if (isParticleDraw_[0] == false) {
				isParticleDraw_[0] = true;
			}
			else {
				isParticleDraw_[0] = false;
			}
		}
		if (ImGui::Button("DrawParticle2")) {
			if (isParticleDraw_[1] == false) {
				isParticleDraw_[1] = true;
			}
			else {
				isParticleDraw_[1] = false;
			}
		}
		if (isParticleDraw_[0] == true) {
			if (ImGui::TreeNode("Particle1")) {
				int amount = testEmitter_[0].count;
				ImGui::Checkbox("UseBillBoard", &isBillBoard_[0]);
				ImGui::DragFloat3("EmitterTranslate", testEmitter_[0].transform.translate.num, 0.1f);
				ImGui::DragFloat3("AccelerationField", accelerationField_[0].acceleration.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMin", accelerationField_[0].area.min.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMax", accelerationField_[0].area.max.num, 0.1f);
				ImGui::DragInt("Amount", &amount, 1, 0, particle_[0]->GetkNumMaxInstance());
				testEmitter_[0].count = amount;
				ImGui::DragFloat("Frequency", &testEmitter_[0].frequency, 0.1f, 0.0f, 100.0f);
				if (ImGui::Button("isSetColor")) {
					if (isColor_[0] == false) {
						isColor_[0] = true;
					}
					else {
						isColor_[0] = false;
					}
				}
				if (isColor_[0]) {
					ImGui::ColorEdit4("Color", particleColor_[0].num, 0);
				}
				ImGui::TreePop();
			}
		}
		if (isParticleDraw_[1] == true) {
			if (ImGui::TreeNode("Particle2")) {
				int amount = testEmitter_[1].count;
				ImGui::Checkbox("UseBillBoard", &isBillBoard_[1]);
				ImGui::DragFloat3("EmitterTranslate", testEmitter_[1].transform.translate.num, 0.1f);
				ImGui::DragFloat3("AccelerationField", accelerationField_[1].acceleration.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMin", accelerationField_[1].area.min.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMax", accelerationField_[1].area.max.num, 0.1f);
				ImGui::DragInt("Amount", &amount, 1, 0, particle_[1]->GetkNumMaxInstance());
				testEmitter_[1].count = amount;
				ImGui::DragFloat("Frequency", &testEmitter_[1].frequency, 0.1f, 0.0f, 100.0f);
				if (ImGui::Button("isSetColor")) {
					if (isColor_[1] == false) {
						isColor_[1] = true;
					}
					else {
						isColor_[1] = false;
					}
				}
				if (isColor_[1]) {
					ImGui::ColorEdit4("Color", particleColor_[1].num, 0);
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Text("%f", ImGui::GetIO().Framerate);

	ImGui::End();

	for (int i = 0; i < 2; i++) {
		if (isParticleDraw_[i]) {
			particle_[i]->Update();
			particle_[i]->SetEmitter(testEmitter_[i]);
			particle_[i]->SetAccelerationField(accelerationField_[i]);
			particle_[i]->SetisBillBoard(isBillBoard_[i]);
			particle_[i]->SetisColor(isColor_[i]);
			if (isColor_[i] == true) {
				particle_[i]->SetColor(particleColor_[i]);
			}
		}
	}

	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (input_->PushAButton(joyState)) {
		OutputDebugStringA("A button\n");
	}
}

void GameDemoScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->PreDraw2D();

	for (int i = 0; i < 2; i++) {
		if (isSpriteDraw_[i]) {//Sprite描画
			sprite_[i]->Draw(spriteTransform_[i], SpriteuvTransform_[i], spriteMaterial_[i]);
		}
	}

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->PreDraw3D();

	for (int i = 0; i < 2; i++) {
		if (isTriangleDraw_[i]) {//Triangle描画
			triangle_[i]->Draw(worldTransformTriangle_[i], viewProjection_, triangleMaterial_[i], uvResourceNum_);
		}

		if (isSphereDraw_[i]) {
			sphere_[i]->Draw(worldTransformSphere_[i], viewProjection_, sphereMaterial_[i], texture_[i]);
		}
	}

	for (int i = 0; i < 3; i++) {
		if (isModelDraw_[i]) {
			model_[i]->Draw(worldTransformModel_[i], viewProjection_, modelMaterial_[i]);
		}
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->PreDrawParticle();

	for (int i = 0; i < 2; i++) {
		if (isParticleDraw_[i]) {
			particle_[i]->Draw(viewProjection_);
		}
	}

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->PreDraw2D();

#pragma endregion
}

void GameDemoScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	audio_->SoundUnload(&soundData2_);
}

void GameDemoScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameDemoScene";
}