/**
 * @file DebugScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "DebugScene.h"

#include "GlobalVariablesGroup.h"

void DebugScene::Initialize() {
	Iscene::Initialize();

	//テクスチャ
	testTex_ = textureManager_->Load("project/gamedata/resources/default/uvChecker.png");
	skyboxTex_ = textureManager_->Load("project/gamedata/resources/default/vz_empty_space_cubemap_ue.dds");
	particleTex_ = textureManager_->Load("project/gamedata/resources/default/circle.png");

	//サウンドリソース
	audio_->LoadSE("click", "project/gamedata/resources/default/click.mp3");

	//2DSprite
	sprite_ = std::make_unique <Sprite2D>();

	sprite_->transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	sprite_->uvTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	sprite_->material = { 1.0f,1.0f,1.0f,1.0f };

	sprite_->sprite->Initialize(Vector2{ 100.0f,100.0f }, testTex_);
	sprite_->sprite->SetTextureInitialSize();//これがない場合、Initialize第一引数sizeで指定したサイズになる
	sprite_->sprite->SetAnchor(Vector2{ 0.5f,0.5f });//アンカーは授業で作った通り

	//3DModel(NoAnimation)
	modelNAnimation_ = std::make_shared<ModelInstance>();
	modelNAnimation_->model = ModelManager::GetInstance()->LoadModel("project/gamedata/resources/block", "block.obj");
	modelNAnimation_->model->SetDirectionalLightFlag(true, 3);

	renderer_.AddInstance(modelNAnimation_);

	//3DModel(Animation)
	modelAnimation_ = std::make_shared<ModelInstance>();
	modelAnimation_->model = ModelManager::GetInstance()->LoadModel("project/gamedata/resources/flag", "flag.gltf", true);
	modelAnimation_->model->SetDirectionalLightFlag(true, 3);
	modelAnimation_->isSkinning = true;

	renderer_.AddInstance(modelAnimation_);

	//SkyBox
	skyBox_ = std::make_unique <CreateSkyBox>();
	skyBox_->Initialize();
	skyBox_->SetDirectionalLightFlag(true, 3);
	skyBoxWorldTransform_.Initialize();
	skyBoxWorldTransform_.scale_ = { 1000.0f,1000.0f,1000.0f };
	skyBoxMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	//Particle
	particleEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	particleEmitter_.count = 5;
	particleEmitter_.frequency = 0.2f;//0.2秒ごとに発生
	particleEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 10.0f,15.0f,10.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, particleEmitter_, accelerationField, particleTex_);
	particle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	//DataReceipt
	datareceipt_.Initialize(50001);
	datareceipt_.start();

	//LineShapes
	lineShapes_.Initialize(16, 0.5f);

	//GlobalVariablesGroup
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("DebugScene");
	gvg->SetValue("Test", &testData_);
}

void DebugScene::Update() {
	Iscene::Update();
	ImGui::Begin("DebugOperate");
	if (ImGui::Button("GameScene")) {
		sceneNumber_->SetSceneNumber(GAME_SCENE);
	}
	ImGui::DragFloat3("test", modelNAnimation_->worldTransform.translation_.ptr());
	ImGui::SliderFloat3("LineStartPoint", lineStartPoint_.ptr(), -100.0f, 100.0f);
	ImGui::SliderFloat3("LineEndPoint", lineEndPoint_.ptr(), -100.0f, 100.0f);
	if (ImGui::Button("SetLine")) {
		lineShapes_.AddCustomLine(lineStartPoint_, lineEndPoint_, { 1.0f, 0.0f, 0.0f, 1.0f });
	}
	if (ImGui::Button("ResetLine")) {
		lineShapes_.ClearCustomLines();
	}
	ImGui::Text("CameraReset:Q key");
	ImGui::End();

	//3DModel(NoAnimation)
	modelNAnimation_->worldTransform.UpdateMatrix();

	//3DModel(Animation)
	modelAnimation_->worldTransform.UpdateMatrix();

	//SkyBox
	skyBoxWorldTransform_.UpdateMatrix();

	//DataReceipt
	datareceipt_.receiveMessage();

	//Particle
	particle_->Update();

	//カメラリセット
	if (Input::GetInstance()->TriggerKey(DIK_Q)) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,0.0f,-20.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
	}
}

void DebugScene::Draw() {
#pragma region SkyBox描画
	CJEngine_->renderer_->Draw(PipelineType::SkyBox);

	skyBox_->Draw(skyBoxWorldTransform_, viewProjection_, skyBoxMaterial_, skyboxTex_);
#pragma endregion

#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	//3DModel(NoAnimation)
	renderer_.DrawNoAnimationAll(viewProjection_);

	//Line
	//lineShapes_.DrawSphere(sphere_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawPlane(plane_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawAABB(aabb_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawOBB(obb_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawCylinder(cylinder_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	lineShapes_.DrawCustomLines(viewProjection_);

	//Editors
	editors_->Draw(viewProjection_);

	//Houdini
	datareceipt_.Draw(viewProjection_);

#pragma endregion

#pragma region 3DSkinningオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);

	//3DModel(Animation)
	renderer_.DrawAnimationAll(viewProjection_);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	//Particle
	//particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawUI() {//ここで描画するとポストエフェクトの影響を受けないよ
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	//2DSprite
	//sprite_->sprite->Draw(spriteTransform_, spriteUVTransform_, spriteMaterial_);

#pragma endregion
}

void DebugScene::DrawPostEffect() {
	Iscene::DrawPostEffect();
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	Iscene::Finalize();
	datareceipt_.stop();
}